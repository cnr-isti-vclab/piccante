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

#ifndef PIC_TONE_MAPPING_HYBRID_TMO_HPP
#define PIC_TONE_MAPPING_HYBRID_TMO_HPP

#include "../algorithms/segmentation_tmo_approx.hpp"
#include "../algorithms/pyramid.hpp"
#include "../filtering/filter_drago_tmo.hpp"
#include "../filtering/filter_sigmoid_tmo.hpp"

namespace pic {

/**
 * @brief The HybridTMO class
 */
class HybridTMO
{
protected:
    Segmentation seg;
    FilterDragoTMO fltDragoTMO;
    FilterSigmoidTMO fltReinhardTMO;
    Pyramid *pyrA, *pyrB, *pyrWeight;
    float Ld_Max, b;

    Image *imgDrago, *imgReinhard, *seg_map;

public:
    /**
     * @brief HybridTMO
     */
    HybridTMO()
    {
        imgDrago = NULL;
        imgReinhard = NULL;
        pyrA = NULL;
        pyrB = NULL;
        pyrWeight = NULL;
        seg_map = NULL;

        Ld_Max = 100.0f;
        b = 0.95f;
    }

    /**
     * @brief ReinhardApprox
     * @param alpha1
     * @param alpha2
     */
    void ReinhardApprox(float &alpha1, float &alpha2)
    {
        alpha1 = 1.0f / (2.0f * sqrtf(2.0f));	//sigma_s
        alpha2 = powf(1.6f, 9.0f); //sigma_r
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *execute(Image *imgIn, Image *imgOut)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        if(!imgIn->isValid()) {
            return NULL;
        }

        if(imgOut == NULL) {
            imgOut = new Image(1, imgIn->width, imgIn->height, imgIn->channels);
        }

        //compute segmentation map
        seg_map = seg.Process(imgIn, seg_map);

        /*	0 ---> Drago et al. 2003
        	1 ---> Reinhard et al. 2002
        	LumZone     = [-2, -1, 0, 1, 2, 3, 4];
        	TMOForZone =  [ 0,  0, 1, 0, 1, 0, 0];	*/

        int count[2];
        count[0] = 0;
        count[1] = 0;

        for(int i = 0; i < seg_map->size(); i++) {
            int indx = int(seg_map->data[i]);

            if((indx == 2) || (indx == 4)) {
                seg_map->data[i] = 1.0f;
                count[1]++;
            } else {
                seg_map->data[i] = 0.0f;
                count[0]++;
            }
        }

#ifdef PIC_DEBUG
        seg_map->Write("weight_map.pfm");
#endif

        //check if we have different zones
        int value = 10;

        if(count[0] > 0 && count[1] > 0) {
            value = 10;
        }

        if(count[0] > 0 && count[1] == 0) {
            value = 0;
        }

        if(count[0] == 0 && count[1] > 0) {
            value = 1;
        }

        switch(value) {
        case 0: {
            fltDragoTMO.Process(Single(imgIn), imgOut);
        }
        break;

        case 1: {
            fltReinhardTMO.Process(Single(imgIn), imgOut);
        }
        break;

        case 10: {
            //Drago TMO
            imgDrago = fltDragoTMO.Process(Single(imgIn), imgDrago);

            if(pyrA == NULL) {
                pyrA = new Pyramid(imgDrago, true);
            } else {
                pyrA->update(imgDrago);
            }

            //Reinhard TMO
            imgReinhard = fltReinhardTMO.Process(Single(imgIn), imgReinhard);

            if(pyrB == NULL) {
                pyrB = new Pyramid(imgReinhard, true);
            } else {
                pyrB->update(imgReinhard);
            }

            //compute blending weight
            if(pyrWeight == NULL) {
                pyrWeight = new Pyramid(seg_map, false);
            } else {
                pyrWeight->update(seg_map);
            }

            //blend
            pyrA->blend(pyrB, pyrWeight);
            pyrA->reconstruct(imgOut);
        }
        break;
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_HYBRID_TMO_HPP */

