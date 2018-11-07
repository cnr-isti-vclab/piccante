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
    #include "../common_code/gl_core_4_0.h"
#endif

#include <QKeyEvent>
#include <QtCore/QCoreApplication>
#include <QtOpenGL/QGLWidget>
#include <QApplication>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QLabel>

#include "piccante.hpp"

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{  
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;
    pic::FilterGLConv2D *conv2D;
    pic::ImageGL img, weights, *imgRec, *img_flt_tmo;
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
        img.Read("../data/input/bottles.hdr");
        img.generateTextureGL();

        //read weights
        weights.Read("../data/input/star.bmp", pic::LT_NOR);
        weights.generateTextureGL();

        float *sum = weights.getSumVal();

        if(sum != NULL) {
            weights /= sum[0];
        }

        //create a screen aligned quad
        pic::QuadGL::getTechnique(technique,
                            pic::QuadGL::getVertexProgramV3(),
                            pic::QuadGL::getFragmentProgramForView());
        quad = new pic::QuadGL(true);

        //allocate a new filter for simple tone mapping
        tmo = new pic::FilterGLSimpleTMO();
        tmo->update(2.2f, 0.0f);

        conv2D = new pic::FilterGLConv2D(GL_TEXTURE_2D);
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

        //simple tone mapping: gamma + exposure correction
        if(method == 1) {
            imgRec = conv2D->Process(DoubleGL(&img, &weights), imgRec);
            img_flt_tmo = tmo->Process(pic::SingleGL(imgRec), img_flt_tmo);
        } else {
            img_flt_tmo = tmo->Process(pic::SingleGL(&img), img_flt_tmo);
        }

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
        setFixedWidth(912);
        setFixedHeight(684);

        method = 0;

        imgRec = NULL;
        img_flt_tmo = NULL;
        imgRec = NULL;
        quad = NULL;
    }

    /**
     * @brief update
     */
    void update()
    {
        method = (method + 1) % 2;
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
        resize(912, 684 + 64);

        window_gl = new GLWidget(format, this);

        layout = new QVBoxLayout();

        layout->addWidget(window_gl);

        label = new QLabel(
        "Pease hit the space bar in order to switch from the original image (with a black hole) to the reconstructed one using Push-Pull.", this);
        label->setFixedWidth(912);
        label->setFixedHeight(64);

        layout->addWidget(label);

        setLayout(layout);

        setWindowTitle(tr("Push-Pull Example"));
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
