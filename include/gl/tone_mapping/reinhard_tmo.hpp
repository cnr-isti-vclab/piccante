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

#ifndef PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP
#define PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP

#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_drago_tmo.hpp"

namespace pic {


class ReinhardTMOGL
{
protected:
    FilterGLLuminance  *flt_lum;
    FilterGLSigmoidTMO *flt_tmo;

    ImageGL            *img_lum;

public:
    /**
     * @brief DragoTMOGL
     */
    DragoTMOGL()
    {
        flt_lum = new FilterGLLuminance();
        flt_tmo = new FilterGLSigmoidTMO();

        img_lum = NULL;
    }

    ~DragoTMOGL()
    {
        if(flt_lum != NULL) {
            delete flt_lum;
            flt_lum = NULL;
        }

        if(flt_tmo != NULL) {
            delete flt_tmo;
            flt_tmo = NULL;
        }

        if(img_lum != NULL) {
            delete img_lum;
            img_lum = NULL;
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param Ld_Max
     * @param bias
     * @param imgOut
     * @return
     */
    ImageGL *ProcessGlobal(ImageGL *imgIn, float alpha = 0.18f, ImageGL *imgOut = NULL)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        img_lum = flt_lum->Process(SingleGL(imgIn), img_lum);

        float Lwa;
        img_lum->getMeanVal(&Lwa);

        FilterGLSigmoidTMO *filter = new FilterGLSigmoidTMO(0.18f / Lav, false, true);

        #ifdef PIC_DEBUG
            GLuint testTQ1 = glBeginTimeQuery();
        #endif

        ImageGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);

        flt_tmo->Update(alpha);
        imgOut = flt_tmo->Process(DoubleGL(imgIn, img_lum), imgOut);

        return imgOut;
    }
};


/**
 * @brief ReinhardLocal
 * @param nameIn
 * @param nameOut
 * @param filter
 * @return
 */
static ImageGL *ReinhardLocal(ImageGL *imgIn, ImageGL *imgOut, FilterGL *filter)
{
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
ImageGL *ReinhardGlobal(ImageGL *imgIn, ImageGL *imgOut)
{
    float Lav = imgIn->getLogMeanVal()[0];

    FilterGLSigmoidTMO *filter = new FilterGLSigmoidTMO(0.18f / Lav, false, true);

    #ifdef PIC_DEBUG
        GLuint testTQ1 = glBeginTimeQuery();
    #endif

    ImageGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);

    #ifdef PIC_DEBUG
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Sigmoid Filter on GPU time: %f ms\n", double(timeVal) / 1000000.0);
    #endif

    imgOut->loadToMemory();
    imgOut->Write(nameOut);
    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_REINHARD_TMO_HPP */

