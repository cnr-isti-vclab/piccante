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

#ifndef PIC_GL_TONE_MAPPING_HYBRID_TMO_HPP
#define PIC_GL_TONE_MAPPING_HYBRID_TMO_HPP

#include "gl/tone_mapping/segmentation_tmo_approx.hpp"

#include "gl/algorithms/pyramid.hpp"

#include "gl/filtering/filter_remapping.hpp"

#include "gl/tone_mapping/drago_tmo.hpp"

#include "gl/tone_mapping/reinhard_tmo.hpp"

namespace pic {

/**
 * @brief The HybridTMOGL class
 */
class HybridTMOGL
{
protected:
    SegmentationGL seg;
    FilterGLRemapping remap;
    ReduxGL*check;
    ImageGL *seg_map;
    ImageGL *imgDrago, *imgReinhard, *remapped;
    PyramidGL *pyrA, *pyrB, *pyrWeight;
    float Ld_Max, b;
    bool bFirst, bAllocate;

    DragoTMOGL *flt_drago;
    ReinhardTMOGL *flt_reinhard;

    /**
     * @brief allocateFilters
     */
    void allocateFilters()
    {
        bAllocate = true;
        flt_drago = new DragoTMOGL();
        flt_reinhard = new ReinhardTMOGL();
        check = ReduxGL::createCheck();
    }

public:

    /**
     * @brief HybridTMOGL
     */
    HybridTMOGL()
    {
        bAllocate = false;
        bFirst = true;

        flt_reinhard = NULL;
        flt_reinhard = NULL;
        check = NULL;

        imgDrago = NULL;
        imgReinhard = NULL;
        pyrA = NULL;
        pyrB = NULL;
        pyrWeight = NULL;
        seg_map = NULL;
        remapped = NULL;

        Ld_Max = 100.0f;
        b = 0.95f;
    }

    ~HybridTMOGL()
    {

    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGL *imgIn, ImageGL *imgOut)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(!imgIn->isValid()) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = new ImageGL(1, imgIn->width, imgIn->height, imgIn->channels,
                                    IMG_GPU, GL_TEXTURE_2D);
        }

        if(bAllocate) {
            allocateFilters();
        }

        //compute segmentation map
#ifdef PIC_DEBUG
        float ms, tot_ms;
        GLuint testTQ1 = glBeginTimeQuery();
#endif

        //compute segmentation map
        seg_map = seg.execute(imgIn, seg_map);


#ifdef PIC_DEBUG
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        ms = float(double(timeVal) / 1000000.0);
        tot_ms = ms;
        printf("GPU time segmentation: %f ms\n", ms);
#endif
        remapped = remap.Process(SingleGL(seg_map), remapped);

        /*	0 ---> Drago et al. 2003
        	1 ---> Reinhard et al. 2002
        	LumZone     = [-2, -1, 0, 1, 2, 3, 4];
        	TMOForZone =  [ 0,  0, 1, 0, 1, 0, 0];	*/

        //Checking if we have different zones
#ifdef PIC_DEBUG
        testTQ1 = glBeginTimeQuery();
#endif

        float check_value;
        remapped->getVal(&check_value, check);
        int value = int(check_value);

#ifdef PIC_DEBUG
        timeVal = glEndTimeQuery(testTQ1);
        ms = float(double(timeVal) / 1000000.0);
        tot_ms += ms;

    printf("GPU time Checking Different Zones: %f ms\n", ms);
        testTQ1 = glBeginTimeQuery();
#endif

        switch(value) {
        case 0: {
            imgOut = flt_drago->execute(imgIn, imgOut);
        }
        break;

        case 1: {
            flt_reinhard->update(-1.0f, -1.0f, true);
            imgOut = flt_reinhard->execute(imgIn, imgOut);
        }
        break;

        case 10: {
            //Drago TMO
            imgDrago = flt_drago->execute(imgIn, imgDrago);
            imgDrago->loadToMemory();
            imgDrago->Write("tmp.pfm");

            //Reinhard TMO
            imgReinhard = flt_reinhard->execute(imgIn, imgReinhard);

            //generate/update pryamids
            if(pyrA == NULL) {
                pyrA = new PyramidGL(imgDrago, true);
            } else {
                pyrA->update(imgDrago);
            }

            if(pyrB == NULL) {
                pyrB = new PyramidGL(imgReinhard, true);
            } else {
                pyrB->update(imgReinhard);
            }

            if(pyrWeight == NULL) {
                pyrWeight = new PyramidGL(remapped, false);
            } else {
                pyrWeight->update(remapped);
            }

            //Blending
            pyrA->blend(pyrB, pyrWeight);
            imgOut = pyrA->reconstruct(imgOut);
        }
        break;
        }


#ifdef PIC_DEBUG
        timeVal = glEndTimeQuery(testTQ1);
        ms = float(double(timeVal) / 1000000.0);
        tot_ms += ms;
        printf("GPU time Tone Mapping+Blending: %f ms\n", ms);

        if(!bFirst) {
            printf("Total time: %f\n", tot_ms);
        } else {
            bFirst = false;
        }
#endif

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_HYBRID_TMO_HPP */

