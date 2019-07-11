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
    pic::ImageGL *img, *img_strokes, *seeds, *imgGC;
    int method;
    pic::GrowCutGL *gcGL;

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
        img = new pic::ImageGL();
        img->Read("../data/input/yellow_flowers.png", pic::LT_NOR_GAMMA);
        img->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

        //read the strokes
        img_strokes = new pic::ImageGL();
        img_strokes->Read("../data/input/yellow_flowers_segmentation_strokes.png", pic::LT_NOR_GAMMA);
        img_strokes->generateTextureGL(GL_TEXTURE_2D, GL_FLOAT, false);

        //create a screen aligned quad
        display = new pic::DisplayGL();

        //create a GrowCut
        gcGL = new pic::GrowCutGL();

        imgGC = NULL;

        method = 0;
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
        pic::ImageGL *img_out = NULL;

        switch(method)
        {
        case 0:
        {
            img_out = img;
            window_ext->setWindowTitle(tr("GrowCut Example: Input Image"));
        } break;

        case 1:
        {
            img_out = img_strokes;
            window_ext->setWindowTitle(tr("GrowCut Example: Strokes"));
        } break;

        case 2:
        {

        } break;

        default:
        {
            img_out = img;
        } break;
        }

        display->Process(img_out);

        /*
        if(method == 1) {
            pp->update(pic::Arrayf::zeros(img.channels), 1e-4f);

            imgGC = imgRec = gcGL->Process(DoubleGL(&img, seeds), imgGC);

            img_out = imgRec;
            window_ext->setWindowTitle(tr("PushPull Example: Reconstructed Image"));

        } else {
            window_ext->setWindowTitle(tr("PushPull Example: Input Image with a Hole (black square)"));
            img_out = &img;
        }
        */

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

        method = 0;
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
        resize(800, 533 + 64);

        window_gl = new GLWidget(format, this);
        window_gl->window_ext = this;

        layout = new QVBoxLayout();

        layout->addWidget(window_gl);

        label = new QLabel(
                    "Please hit the space bar in order to switch from the original "
                    "image to the segmentation strokes, and to the segmented mask.",
                    this);
        label->setFixedWidth(800);
        label->setFixedHeight(64);

        layout->addWidget(label);

        setLayout(layout);

        setWindowTitle(tr("Grow-Cut Example"));
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
