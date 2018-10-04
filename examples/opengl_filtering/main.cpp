/*

PICCANTE
The hottest HDR imaging library!
http://piccantelib.net

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

/**
 * NOTE: if you do not want to use this OpenGL functions loader,
 * please change it with your favorite one. This is just
 * a suggestion for running examples.
*/

#ifdef _MSC_VER
    #define PIC_DISABLE_OPENGL_NON_CORE
    #include "../common_code/gl_core_4_0.h"
#endif

//#define PIC_DISABLE_TINY_EXR

#include <QKeyEvent>
#include <QtCore/QCoreApplication>
#include <QtOpenGL/QGLWidget>
#include <QApplication>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QLabel>

#define PIC_DEBUG

#include "piccante.hpp"

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;
    pic::FilterGLBilateral2DG *fltBilG;
    pic::FilterGLBilateral2DSP *fltBilSP;
    pic::FilterGLBilateral2DS *fltBilS;
    pic::FilterGLGaussian2D *fltGauss;

    pic::ImageGL *img, *img_flt, *img_flt_tmo;
    pic::TechniqueGL technique;

    int method;

    /**
     * @brief initializeGL sets variables up.
     */
    void initializeGL(){

        initializeOpenGLFunctions();

        #ifdef PIC_WIN32
            if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
                printf("OpenGL functions are not loaded!\n");
            }
        #endif

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f );

        //read an input image
        img = new pic::ImageGL();
        img->Read("../data/input/yellow_flowers.png");
        img->generateTextureGL();

        //create a screen aligned quad
        pic::QuadGL::getTechnique(technique,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());

        quad = new pic::QuadGL(true);

        //allocate a new filter for simple tone mapping
        tmo = new pic::FilterGLSimpleTMO();

        float sigma_s = 8.0f;
        float sigma_r = 0.2f;

        fltGauss = new pic::FilterGLGaussian2D(sigma_s);

        //allocate a new bilateral filter
        fltBilG = new pic::FilterGLBilateral2DG(sigma_s, sigma_r);

        //allocate a new bilateral filter
        fltBilSP = new pic::FilterGLBilateral2DSP(sigma_s, sigma_r);

        //allocate a new bilateral filter
        fltBilS = new pic::FilterGLBilateral2DS(sigma_s, sigma_r);

        img_flt_tmo = NULL;
        img_flt = NULL;
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
            break;

            case 1:
                //apply the bilateral grid filter
                img_flt = fltGauss->Process(SingleGL(img), img_flt);
                img_out = img_flt;
            break;

            case 2:
                //apply the bilateral grid filter
                img_flt = fltBilG->Process(SingleGL(img), img_flt);
                img_out = img_flt;
            break;

            case 3:
                //apply the separate bilateral filter
                img_flt = fltBilSP->Process(SingleGL(img), img_flt);
                img_out = img_flt;
            break;

            case 4:
                //apply the sampling bilateral filter
                img_flt = fltBilS->Process(SingleGL(img), img_flt);
                img_out = img_flt;
            break;

        default:
            img_out = img;
            break;
        }

        //simple tone mapping: gamma + exposure correction
        img_flt_tmo = tmo->Process(SingleGL(img_out), img_flt_tmo);

        //visualization
        quad->Render(technique, img_flt_tmo->getTexture());
    }

public:

    /**
     * @brief GLWidget
     * @param format
     * @param parent
     */
    GLWidget( const QGLFormat& format, QWidget* parent = 0 ): QGLWidget(format, parent, 0)
    {
        setFixedWidth(800);
        setFixedHeight(533);

        tmo = NULL;
        img_flt = NULL;
        img_flt_tmo = NULL;
        method = 0;
    }

    /**
     * @brief update
     */
    void update()
    {
        method = (method + 1) % 5;
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

        layout = new QVBoxLayout();

        layout->addWidget(window_gl);

        label = new QLabel(
        "Pease hit the space bar in order to switch from the filtered image to the original one.", this);
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
