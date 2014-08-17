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
    FilterGLRedux		*check;
    ImageRAWGL			*seg_map;
    FilterGLDragoTMO	*fltDragoTMO;
    FilterGLSigmoidTMO	*fltReinhardTMO;
    PyramidGL			*pyrA, *pyrB, *pyrWeight;
    float				Ld_Max, b;
    bool				bFirst;
    ImageRAWGL			*imgDrago, *imgReinhard, *remapped;

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

        check = FilterGLRedux::CreateCheck();
        fltDragoTMO    = new FilterGLDragoTMO(100.0f, 0.85f, 1e6, true);
        fltReinhardTMO = new FilterGLSigmoidTMO(0.18f, false, true);
    }

    ImageRAWGL *Compute(ImageRAWGL *imgIn, ImageRAWGL *imgOut)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(!imgIn->isValid()) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = new ImageRAWGL(1, imgIn->width, imgIn->height, imgIn->channels,
                                    IMG_GPU, GL_TEXTURE_2D);
        }

        float ms, tot_ms;
        //Compute segmentation map
        GLuint testTQ1 = glBeginTimeQuery();
        seg_map = seg.Compute(imgIn, seg_map);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        ms = double(timeVal) / 1000000.0;
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
        ImageRAWGL *check_value = check->Redux(remapped, seg.stack);
        check_value->loadToMemory();
        int value = int(check_value->data[0]);
        timeVal = glEndTimeQuery(testTQ1);
        ms = double(timeVal) / 1000000.0;
        tot_ms += ms;

#ifdef PIC_DEBUG
        printf("GPU time Checking Different Zones: %f ms\n", ms);
#endif

        testTQ1 = glBeginTimeQuery();

        switch(value) {
        case 0: {
            fltDragoTMO->Update(Ld_Max, b, seg.maxVal);
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
            fltDragoTMO->Update(Ld_Max, 100, seg.maxVal);
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
        ms = double(timeVal) / 1000000.0;
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

