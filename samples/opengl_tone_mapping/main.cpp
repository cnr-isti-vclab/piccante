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

#define PIC_DISABLE_OPENGL_NON_CORE

/**
 * NOTE: if you do not want to use this OpenGL functions loader,
 * please change it with your favorite one. This is just
 * a suggestion for running examples.
*/
#ifdef _MSC_VER
    #define PIC_DISABLE_OPENGL_NON_CORE
    #include "../opengl_common_code/gl_core_4_0.h"
#endif

#include <QKeyEvent>

#include "piccante.hpp"

#include "../opengl_common_code/opengl_window.hpp"

class SimpleIOWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLColorConv *tmo;
    pic::DragoTMOGL        *drago_tmo;
    pic::ReinhardTMOGL     *reinhard_tmo;

public:
    pic::ImageGL    img, *img_tmo, *img_tmo_with_sRGB;
    glw::program    program;

    unsigned int    method;

    SimpleIOWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        img_tmo = NULL;
        img_tmo_with_sRGB = NULL;
        method = 0;
    }

    void init()
    {
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

        //allocating Drago et al.'s TMO
        drago_tmo = new pic::DragoTMOGL();

        //allocating Reinhard et al.'s TMO
        reinhard_tmo = new pic::ReinhardTMOGL();
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        switch(method) {
        case 0:
            //applying Reinhard et al.'s TMO (local version)
            img_tmo = reinhard_tmo->ProcessLocal(&img, 0.18f, 8.0f, NULL, img_tmo);
            break;

        case 1:
            //applying Reinhard et al.'s TMO (global version)
            img_tmo = reinhard_tmo->ProcessGlobal(&img, 0.18f, img_tmo);
            break;

        case 2:
            //applying Drago et al.'s TMO
            img_tmo = drago_tmo->Process(&img, 100.0f, 0.95f, img_tmo);
            break;
        }

        //converting the image color space from linear RGB to sRGB
        img_tmo_with_sRGB = tmo->Process(SingleGL(img_tmo), img_tmo_with_sRGB);

        //img_tmo_with_sRGB visualization
        quad->Render(program, img_tmo_with_sRGB->getTexture());
    }

    void keyPressEvent(QKeyEvent * ev)
    {
        if(ev->type() == QEvent::KeyPress) {
            if(ev->key() == Qt::Key_Space) {
                method = (method + 1) % 3;
            }
        }
    }
};


int main(int argc, char **argv)
{

    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(4);
    format.setMajorVersion(4);
    format.setMinorVersion(0);
    format.setProfile(QSurfaceFormat::CoreProfile );

    SimpleIOWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
