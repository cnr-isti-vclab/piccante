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

#ifndef PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP
#define PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP

#include "filtering/gl/filter_luminance.hpp"
#include "filtering/gl/filter_drago_tmo.hpp"

namespace pic {


/**
 * @brief ReinhardLocal
 * @param nameIn
 * @param nameOut
 * @param filter
 * @return
 */
static ImageRAWGL *ReinhardLocal(ImageRAWGL imstd::string nameIn, std::string nameOut,
                                        FilterGL *filter)
{
    ImageRAWGL imgIn(nameIn);
    imgIn.generateTextureGL(false, GL_TEXTURE_2D);

    FilterGLLuminance lum;

    ImageRAWGL *L = lum.Process(SingleGL(&imgIn), NULL);
    L->loadToMemory();
    L->Write("lum.pfm");

    GLuint testTQ1 = glBeginTimeQuery();
    ImageRAWGL *adapt = filter->Process(SingleGL(L), NULL);
    GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
    printf("Bilateral 2DG Filter on GPU time: %f ms\n",
           double(timeVal) / 1000000.0);

    adapt->loadToMemory();
    adapt->Write("adapt.pfm");

    float Lav = imgIn.getLogMeanVal()[0];

    FilterGLSigmoidTMO *tmo = new FilterGLSigmoidTMO(0.18f / Lav, true, true);

    testTQ1 = glBeginTimeQuery();
    ImageRAWGL *imgOut = tmo->Process(DoubleGL(&imgIn, adapt), NULL);
    timeVal = glEndTimeQuery(testTQ1);
    printf("Sigmoid Local Filter on GPU time: %f ms\n",
           double(timeVal) / 1000000.0);

    imgOut->loadToMemory();
    imgOut->Write(nameOut);
    return imgOut;
}

/**
 * @brief ReinhardGlobal
 * @param nameIn
 * @param nameOut
 * @return
 */
ImageRAWGL *ReinhardGlobal(std::string nameIn, std::string nameOut)
{
    ImageRAWGL imgIn(nameIn);
    imgIn.generateTextureGL(false, GL_TEXTURE_2D);

    float Lav = imgIn.getLogMeanVal()[0];

    FilterGLSigmoidTMO *filter = new FilterGLSigmoidTMO(0.18f / Lav, false, true);

    GLuint testTQ1 = glBeginTimeQuery();
    ImageRAWGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);
    GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
    printf("Sigmoid Filter on GPU time: %f ms\n", double(timeVal) / 1000000.0);

    imgOut->loadToMemory();
    imgOut->Write(nameOut);
    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP */

