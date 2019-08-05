/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

/**
 * NOTE: if you do not want to use this OpenGL functions loader,
 * please change it with your favorite one. This is just
 * a suggestion for running examples.
*/

#include "../common_code/gl_include.hpp"

#include <QKeyEvent>
#include <QtCore/QCoreApplication>
#include <QtOpenGL/QGLWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>

#include "piccante.hpp"

class GLWidget : public QGLWidget
        #ifndef PIC_INCLUDE_GL
        , protected QOpenGLFunctions
        #endif
{
protected:
    pic::FilterGLBilateral2DF *fltBilF;
    pic::FilterGLBilateral2DG *fltBilG;
    pic::FilterGLBilateral2DSP *fltBilSP;
    pic::FilterGLBilateral2DS *fltBilS;
    pic::FilterGLBilateral2DAS *fltBilAS;
    pic::FilterGLGaussian2D *fltGauss;
    pic::FilterGLAnisotropicDiffusion *fltAD;

    pic::DisplayGL *display;
    pic::ImageGL *img, *img_flt;

    int method;

    /**
     * @brief initializeGL sets variables up.
     */
    void initializeGL(){

    #ifndef PIC_INCLUDE_GL
        initializeOpenGLFunctions();
    #endif

    #ifdef PIC_INCLUDE_GL
        if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
            printf("OpenGL functions are not loaded!\n");
        }
    #endif

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f );

        //read an input image
        img = new pic::ImageGL();
        img->Read("../data/input/yellow_flowers.png", pic::LT_NOR_GAMMA);
        img->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

        float sigma_s = 16.0f;
        float sigma_r = 0.1f;

        fltGauss = new pic::FilterGLGaussian2D(sigma_s);

        //allocate a new bilateral filter
        fltBilG = new pic::FilterGLBilateral2DG(sigma_s, sigma_r);

        //allocate a new bilateral filter
        fltBilSP = new pic::FilterGLBilateral2DSP(sigma_s, sigma_r);

        //allocate a new bilateral filter
        fltBilS = new pic::FilterGLBilateral2DS(sigma_s, sigma_r);

        //allocate a new bilateral filter
        fltBilAS = new pic::FilterGLBilateral2DAS(sigma_s, sigma_r);

        //allocate a new bilateral filter
        fltBilF = new pic::FilterGLBilateral2DF(sigma_s, sigma_r);

        //allocate a new anisotropic diffusion filter
        fltAD = new pic::FilterGLAnisotropicDiffusion(sigma_s, sigma_r);
        img_flt = NULL;

        display = new pic::DisplayGL();
    }

    /**
     * @brief resizeGL
     * @param w
     * @param h
     */
    void resizeGL( int w, int h ){
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, w * retinaScale, h * retinaScale);
    }

    /**
     * @brief paintGL
     */
    void paintGL(){
        if(parentWidget() != NULL) {
            if(!parentWidget()->isVisible()) {
                return;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        pic::ImageGL *img_out = NULL;
        switch(method)
        {
            case 0:
                //input image
                img_out = img;
                window_ext->setWindowTitle(tr("Filtering Example: Original Image"));

            break;

            case 1:
                //apply the gaussian filter
                img_flt = fltGauss->Process(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Guassian Filter"));

            break;

            case 2:
                //apply the sampling bilateral filter
                img_flt = fltBilF->Process(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Full Bilateral"));

            break;

            case 3:
                //apply the bilateral grid filter
                img_flt = fltBilG->Process(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Bilateral Grid"));

            break;

            case 4:
                //apply the separate bilateral filter
                img_flt = fltBilSP->Process(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Separate Bilateral"));

            break;

            case 5:
                //apply the sampling bilateral filter
                img_flt = fltBilS->Process(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Sub-Sampled Bilateral"));
            break;

            case 6:
                //apply the sampling bilateral filter
                img_flt = fltBilAS->Process(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Sub-Sampled Adaptive Bilateral"));
            break;

            case 7:
                //apply the anisotropic diffusion filter
                img_flt = fltAD->AnisotropicDiffusion(SingleGL(img), img_flt);
                img_out = img_flt;
                window_ext->setWindowTitle(tr("Filtering Example: Anisotropic Diffusion"));

            break;

        default:
            img_out = img;
            break;
        }

        display->Process(img_out);
    }

public:

    QWidget *window_ext;

    /**
     * @brief GLWidget
     * @param format
     * @param parent
     */
    GLWidget( const QGLFormat& format, QWidget* parent = 0 ): QGLWidget(format, parent, 0)
    {
        setFixedWidth(800);
        setFixedHeight(533);

        img_flt = NULL;
        method = 0;
    }

    /**
     * @brief update
     */
    void update()
    {
        method = (method + 1) % 8;
    }
};

class Window : public QWidget
{
protected:
    GLWidget *window_gl;
    QVBoxLayout *layout;
    QLabel *label;

public:

    /**
     * @brief Window
     * @param format
     */
    Window(const QGLFormat &format)
    {
        resize(800, 533 + 64);

        window_gl = new GLWidget(format, this);
        window_gl->window_ext = this;

        layout = new QVBoxLayout();

        layout->addWidget(window_gl);

        label = new QLabel(
        "Pease hit the space bar in order to switch from the original one to the filtered one using different filters.", this);
        label->setAlignment(Qt::AlignHCenter);
        label->setFixedWidth(800);
        label->setFixedHeight(64);

        layout->addWidget(label);

        setLayout(layout);

        setWindowTitle(tr("Filtering Example"));
    }

    ~Window()
    {
        delete window_gl;
        delete layout;
        delete label;
    }

    /**
     * @brief keyPressEvent
     * @param e
     */
    void keyPressEvent( QKeyEvent* e ){
        if(e->type() == QEvent::KeyPress) {
            if(e->key() == Qt::Key_Space) {
                window_gl->update();
                window_gl->updateGL();
            }
        }
    }
};

int main(int argc, char **argv)
{
    QApplication app( argc, argv );

    QGLFormat glFormat;
    glFormat.setVersion( 4, 0 );
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setSampleBuffers( true );

    //Creating a window with OpenGL 4.0 Core profile
    Window w( glFormat );
    w.show();

    app.installEventFilter(&w);

    return app.exec();
}
