/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle










This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

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
static ImageGL *ReinhardLocal(ImageGL imstd::string nameIn, std::string nameOut,
                                        FilterGL *filter)
{
    ImageGL imgIn(nameIn);
    imgIn.generateTextureGL(false, GL_TEXTURE_2D);

    FilterGLLuminance lum;

    ImageGL *L = lum.Process(SingleGL(&imgIn), NULL);
    L->loadToMemory();
    L->Write("lum.pfm");

    GLuint testTQ1 = glBeginTimeQuery();
    ImageGL *adapt = filter->Process(SingleGL(L), NULL);
    GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
    printf("Bilateral 2DG Filter on GPU time: %f ms\n",
           double(timeVal) / 1000000.0);

    adapt->loadToMemory();
    adapt->Write("adapt.pfm");

    float Lav = imgIn.getLogMeanVal()[0];

    FilterGLSigmoidTMO *tmo = new FilterGLSigmoidTMO(0.18f / Lav, true, true);

    testTQ1 = glBeginTimeQuery();
    ImageGL *imgOut = tmo->Process(DoubleGL(&imgIn, adapt), NULL);
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
ImageGL *ReinhardGlobal(std::string nameIn, std::string nameOut)
{
    ImageGL imgIn(nameIn);
    imgIn.generateTextureGL(false, GL_TEXTURE_2D);

    float Lav = imgIn.getLogMeanVal()[0];

    FilterGLSigmoidTMO *filter = new FilterGLSigmoidTMO(0.18f / Lav, false, true);

    GLuint testTQ1 = glBeginTimeQuery();
    ImageGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);
    GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
    printf("Sigmoid Filter on GPU time: %f ms\n", double(timeVal) / 1000000.0);

    imgOut->loadToMemory();
    imgOut->Write(nameOut);
    return imgOut;
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_DRAGO_TMO_GL_HPP */

