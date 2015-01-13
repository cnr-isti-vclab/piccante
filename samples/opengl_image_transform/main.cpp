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

#ifdef _MSC_VER
    #define PIC_DISABLE_OPENGL_NON_CORE
    #include "../opengl_common_code/gl_core_4_0.h"
#endif

#include "piccante.hpp"

#include "../opengl_common_code/opengl_window.hpp"

class SimpleImageTransformWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *flt_tmo;
    pic::FilterGLWarp2D *flt_warp;

public:
    pic::ImageGL img, *img_flt, *img_flt_tmo;
    glw::program    program;

    pic::ImageGLVec stack;

    SimpleImageTransformWindow() : OpenGLWindow(NULL)
    {
        flt_tmo = NULL;
        flt_warp = NULL;
        img_flt = NULL;
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

        //creating a rotation matrix
        pic::Matrix3x3 h;
        h.SetRotationMatrix(pic::Deg2Rad(45.0f));

        //allocating the warping filter
        flt_warp = new pic::FilterGLWarp2D(h, true, true);

         //allocating a new filter for simple tone mapping
        flt_tmo = new pic::FilterGLSimpleTMO();
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //applying the warping filter
        img_flt = flt_warp->Process(SingleGL(&img), img_flt);

        //simple tone mapping: gamma + exposure correction
        img_flt_tmo = flt_tmo->Process(SingleGL(img_flt), img_flt_tmo);

        //visualization of the warped image
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
    format.setProfile(QSurfaceFormat::CoreProfile);

    SimpleImageTransformWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
