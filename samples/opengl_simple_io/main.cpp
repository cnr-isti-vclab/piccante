/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

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

#define PIC_DEBUG

#include "piccante.hpp"

#include "../opengl_common_code/opengl_window.hpp"

class SimpleIOWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLColorConv *tmo;

public:
    pic::ImageRAWGL img, *imgOut;
    glw::program    program;

    SimpleIOWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        imgOut = NULL;
    }

    void init()
    {
        //reading an input image
        img.Read("../data/input/bottles.hdr");
        img.generateTextureGL(false, GL_TEXTURE_2D);

        //creating a screen aligned quad
        pic::QuadGL::getProgram(program,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());

        quad = new pic::QuadGL(true);
        
        //allocating a new filter for simple tone mapping
        tmo = new pic::FilterGLColorConv(new pic::ColorConvGLRGBtosRGB());
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //simple tone mapping: gamma + exposure correction
        imgOut = tmo->Process(SingleGL(&img), imgOut);

        //imgOut visualization
        glw::bind_program(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imgOut->getTexture());

        quad->Render();

        glw::bind_program(0);

        //Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
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
