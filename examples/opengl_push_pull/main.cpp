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
        #ifndef _MSC_VER
        , protected QOpenGLFunctions
        #endif
{
protected:
    pic::DisplayGL *display;
    pic::ImageGL img, *imgRec;
    int method;
    pic::PushPullGL *pp;

    /**
     * @brief initializeGL sets variables up.
     */
    void initializeGL(){

    #ifndef _MSC_VER
        initializeOpenGLFunctions();
    #endif

    #ifdef _MSC_VER
        if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
            printf("OpenGL functions are not loaded!\n");
        }
    #endif

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f );

        //read an input image
        img.Read("../data/input/bottles.hdr");

        pic::Image img_black(1, 32, 32, 3);
        img_black.setZero();

        //add a hole in the image
        img.copySubImage(&img_black, 292, 130);

        img.generateTextureGL();

        //create a screen aligned quad
        display = new pic::DisplayGL();

        //create push pull method
        pp = new pic::PushPullGL();
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
        pic::ImageGL *img_out;
        if(method == 1) {
            pp->update(pic::Arrayf::zeros(img.channels), 1e-4f);

            imgRec = pp->Process(&img, imgRec);

            img_out = imgRec;
            window_ext->setWindowTitle(tr("PushPull Example: Reconstructed Image"));

        } else {
            window_ext->setWindowTitle(tr("PushPull Example: Input Image with a Hole (black square)"));
            img_out = &img;
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
        setFixedWidth(912);
        setFixedHeight(684);

        method = 0;
        imgRec = NULL;
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
        window_gl->window_ext = this;

        layout = new QVBoxLayout();

        layout->addWidget(window_gl);

        label = new QLabel(
                    "Please hit the space bar in order to switch from the original "
                    "image (with a black hole) to the reconstructed one using Push-Pull.",
                    this);
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
