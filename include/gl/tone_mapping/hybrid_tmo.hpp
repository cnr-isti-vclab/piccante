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
#include "gl/filtering/filter_drago_tmo.hpp"
#include "gl/filtering/filter_sigmoid_tmo.hpp"

namespace pic {

class HybridTMOGL
{
protected:
    SegmentationGL		seg;
    FilterGLRemapping	remap;
    ReduxGL     		*check;
    ImageGL             *seg_map;
    FilterGLDragoTMO	*fltDragoTMO;
    FilterGLSigmoidTMO	*fltReinhardTMO;
    PyramidGL			*pyrA, *pyrB, *pyrWeight;
    float				Ld_Max, b;
    bool				bFirst;
    ImageGL             *imgDrago, *imgReinhard, *remapped;

public:
    HybridTMOGL()
    {
        bFirst = true;
        imgDrago = NULL;
        imgReinhard = NULL;
        pyrA = NULL;
        pyrB = NULL;
        pyrWeight = NULL;
        seg_map = NULL;
        remapped = NULL;

        Ld_Max = 100.0f;
        b = 0.95f;

        check = ReduxGL::CreateCheck();

        fltDragoTMO    = new FilterGLDragoTMO(100.0f, 0.85f, 1e6, true);
        fltReinhardTMO = new FilterGLSigmoidTMO(0.18f, false, true);
    }

    ImageGL *Compute(ImageGL *imgIn, ImageGL *imgOut)
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

        float ms, tot_ms;
        //Compute segmentation map
        GLuint testTQ1 = glBeginTimeQuery();
        seg_map = seg.Compute(imgIn, seg_map);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        ms = float(double(timeVal) / 1000000.0);
        tot_ms = ms;

#ifdef PIC_DEBUG
        printf("GPU time segmentation: %f ms\n", ms);
#endif

#ifdef PIC_DEBUG
        seg_map->loadToMemory();
        seg_map->Write("segmentation_map.pfm");
#endif

        remapped = remap.Process(SingleGL(seg_map), remapped);

        /*	0 ---> Drago et al. 2003
        	1 ---> Reinhard et al. 2002
        	LumZone     = [-2, -1, 0, 1, 2, 3, 4];
        	TMOForZone =  [ 0,  0, 1, 0, 1, 0, 0];	*/

        //Checking if we have different zones
        testTQ1 = glBeginTimeQuery();

        float check_value;
        remapped->getVal(&check_value, check);
        int value = int(check_value);
        timeVal = glEndTimeQuery(testTQ1);
        ms = float(double(timeVal) / 1000000.0);
        tot_ms += ms;

#ifdef PIC_DEBUG
        printf("GPU time Checking Different Zones: %f ms\n", ms);
#endif

        testTQ1 = glBeginTimeQuery();

        switch(value) {
        case 0: {
            fltDragoTMO->Update(Ld_Max, b, seg.maxVal, 1.0f);
            fltDragoTMO->Process(SingleGL(imgIn), imgOut);
        }
        break;

        case 1: {
            fltReinhardTMO->Update(0.18f);
            fltReinhardTMO->Process(SingleGL(imgIn), imgOut);
        }
        break;

        case 10: {
            //Drago TMO
            fltDragoTMO->Update(Ld_Max, 100, seg.maxVal, 1.0f);
            imgDrago = fltDragoTMO->Process(SingleGL(imgIn), imgDrago);

            if(pyrA == NULL) {
                pyrA = new PyramidGL(imgDrago, true);
            } else {
                pyrA->Update(imgDrago);
            }

            //Reinhard TMO
            fltReinhardTMO->Update(0.18f);
            imgReinhard = fltReinhardTMO->Process(SingleGL(imgIn), imgReinhard);

            if(pyrB == NULL) {
                pyrB = new PyramidGL(imgReinhard, true);
            } else {
                pyrB->Update(imgReinhard);
            }

            //Blending weight
            if(pyrWeight == NULL) {
                pyrWeight = new PyramidGL(remapped, false);
            } else {
                pyrWeight->Update(remapped);
            }

            //Blending
            pyrA->Blend(pyrB, pyrWeight);
            pyrA->Reconstruct(imgOut);
        }
        break;
        }

        timeVal = glEndTimeQuery(testTQ1);
        ms = float(double(timeVal) / 1000000.0);
        tot_ms += ms;

#ifdef PIC_DEBUG
        printf("GPU time Tone Mapping+Blending: %f ms\n", ms);
#endif

        if(!bFirst) {
            printf("%f\n",
                   tot_ms);    //printf("Total time for Hybrid Tone Mapping: %f ms\n",tot_ms);
        } else {
            bFirst = false;
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_TONE_MAPPING_HYBRID_TMO_HPP */

