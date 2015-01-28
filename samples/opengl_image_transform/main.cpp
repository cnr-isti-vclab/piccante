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
    pic::FilterGLSimpleTMO *flt_tmo;
    pic::FilterGLWarp2D *flt_warp;
    pic::ImageGL img, *img_flt, *img_flt_tmo;
    glw::program    program;
    pic::Matrix3x3 h;

    int degrees;

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

        //creating a rotation matrix
        h.SetRotationMatrix(pic::Deg2Rad(float(degrees)));

        //allocating the warping filter
        flt_warp = new pic::FilterGLWarp2D(h, true, true);

         //allocating a new filter for simple tone mapping
        flt_tmo = new pic::FilterGLSimpleTMO();
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

        //applying the warping filter
        h.SetRotationMatrix(pic::Deg2Rad(float(degrees)));
        flt_warp->Update(h, true, true);

        img_flt = flt_warp->Process(SingleGL(&img), img_flt);

        //simple tone mapping: gamma + exposure correction
        img_flt_tmo = flt_tmo->Process(SingleGL(img_flt), img_flt_tmo);

        //visualization
        quad->Render(program, img_flt_tmo->getTexture());
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

        degrees = 45;

        flt_tmo = NULL;
        flt_warp = NULL;
        img_flt = NULL;
        img_flt_tmo = NULL;
    }

    /**
     * @brief update
     */
    void update()
    {
        degrees = (degrees + 1) % 360;
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
        "Pease hit the space bar in order to increase of one degree the image's rotation.", this);
        label->setAlignment(Qt::AlignHCenter);
        label->setFixedWidth(912);
        label->setFixedHeight(64);

        layout->addWidget(label);

        setLayout(layout);

        setWindowTitle(tr("Image Transform Example"));
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
