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

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "piccante.hpp"

class SimpleFilteringWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;
    pic::FilterGLGaussian2D *fltGauss;

public:
    pic::ImageRAWGL img, *img_flt, *img_flt_tmo;
    glw::program    program;

    SimpleFilteringWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        img_flt = NULL;
        img_flt_tmo = NULL;
    }

    void init()
    {
        //reading an input image
        img.Read("../data/input/bottles.hdr");
        img.generateTextureGL(false);

        //creating a screen aligned quad
        pic::QuadGL::getProgram(program,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());
        quad = new pic::QuadGL(true);

        //allocating a new filter for simple tone mapping
        tmo = new pic::FilterGLSimpleTMO();

        //allocating a Gaussian filter with sigma = 4.0
        fltGauss = new pic::FilterGLGaussian2D(4.0);
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //Gaussian filtering...
        img_flt = fltGauss->Process(SingleGL(&img), img_flt);

        //simple tone mapping: gamma + exposure correction
        img_flt_tmo = tmo->Process(SingleGL(img_flt), img_flt_tmo);

        //visualization of the Gaussian filtering
        glw::bind_program(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, img_flt_tmo->getTexture());

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
    format.setProfile(QSurfaceFormat::CoreProfile);

    SimpleFilteringWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
