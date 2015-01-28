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
    #include "../opengl_common_code/gl_core_4_0.h"
#endif

#include "piccante.hpp"

#include <QKeyEvent>
#include <QtCore/QCoreApplication>
#include <QtOpenGL/QGLWidget>
#include <QApplication>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QLabel>

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLColorConv *tmo;
    pic::ImageGL img, *imgOut;
    glw::program    program;

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

        //reading an input image
        img.Read("../data/input/bottles.hdr");
        img.generateTextureGL();

        //creating a screen aligned quad
        pic::QuadGL::getProgram(program,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());

        quad = new pic::QuadGL(true);

        //allocating a new filter for simple tone mapping
        tmo = new pic::FilterGLColorConv(new pic::ColorConvGLRGBtosRGB());
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
        imgOut = tmo->Process(SingleGL(&img), imgOut);

        //visualization
        quad->Render(program, imgOut->getTexture());
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

        imgOut = NULL;
        quad = NULL;
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
        "This example opens an HDR image and shows it using OpenGL.", this);
        label->setFixedWidth(912);
        label->setFixedHeight(64);
        label->setAlignment(Qt::AlignHCenter);

        layout->addWidget(label);

        setLayout(layout);

        setWindowTitle(tr("Simple OpenGL I/O"));
    }

    ~Window()
    {
        delete window_gl;
        delete layout;
        delete label;
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

    return app.exec();
}
