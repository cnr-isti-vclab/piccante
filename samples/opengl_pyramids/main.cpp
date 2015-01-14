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

#include "../opengl_common_code/opengl_window.hpp"

class SimpleOperatorsWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;

public:
    pic::ImageGL img, *imgRec, *img_flt_tmo;
    glw::program    program;
    pic::PyramidGL  *pyr;

    SimpleOperatorsWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        img_flt_tmo = NULL;

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
        tmo = new pic::FilterGLSimpleTMO();

        imgRec = NULL;
        pyr = new pic::PyramidGL(&img, true);
        //pyr->Update(&img);

        pic::Pyramid *pyrCPU = new pic::Pyramid(&img, true);
        //pyrCPU->Update(&img2);

   }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //simple tone mapping: gamma + exposure correction

        pyr->Update(&img);
        imgRec = pyr->Reconstruct(imgRec);

        img_flt_tmo = tmo->Process(SingleGL(imgRec), img_flt_tmo);

        //imgOut visualization
        quad->Render(program, img_flt_tmo->getTexture());
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

    SimpleOperatorsWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
