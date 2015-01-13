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

#include "piccante.hpp"

#include "../opengl_common_code/opengl_window.hpp"

class SimpleIOWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::ExposureFusionGL  *ef;

public:
    pic::ImageGL    img, *img_tmo;
    pic::ImageGLVec img_vec;
    glw::program    program;

    unsigned int    method;

    SimpleIOWindow() : OpenGLWindow(NULL)
    {
        ef = NULL;
        img_tmo = NULL;
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
        
        //allocating an exposure fusion instance
        ef = new pic::ExposureFusionGL();

        //computing a stack of LDR images from an HDR image
        img_vec = pic::getAllExposuresImagesGL(&img);
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //computing exposure fusion for the stack (img_vec)
        img_tmo = ef->Process(img_vec, 0.2f, 1.0f, 0.2f, img_tmo);

        //imgOut visualization
        quad->Render(program, img_tmo->getTexture());
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
