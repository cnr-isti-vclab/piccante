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

#ifndef PIC_TONE_MAPPING_HYBRID_TMO_HPP
#define PIC_TONE_MAPPING_HYBRID_TMO_HPP

#include "tone_mapping/segmentation_tmo_approx.hpp"
#include "algorithms/pyramid.hpp"
#include "filtering/filter_drago_tmo.hpp"
#include "filtering/filter_sigmoid_tmo.hpp"

namespace pic {

class HybridTMO
{
protected:
    Segmentation		seg;
    FilterDragoTMO		fltDragoTMO;
    FilterSigmoidTMO	fltReinhardTMO;
    Pyramid				*pyrA, *pyrB, *pyrWeight;
    float				Ld_Max, b;

    Image			*imgDrago, *imgReinhard, *seg_map;

public:
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

    void ReinhardApprox(float &alpha1, float &alpha2)
    {
        alpha2		= powf(1.6f, 9.0f);			//sigma_r
        alpha1		= 1.0f / (2.0f * sqrtf(2.0f));	//sigma_s
    }

    Image *Compute(Image *imgIn, Image *imgOut)
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

        //Compute segmentation map
        seg_map = seg.Compute(imgIn, seg_map);

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

        //Checking if we have different zones
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
            fltDragoTMO.ProcessP(Single(imgIn), imgOut);
        }
        break;

        case 1: {
            fltReinhardTMO.ProcessP(Single(imgIn), imgOut);
        }
        break;

        case 10: {
            //Drago TMO
            imgDrago = fltDragoTMO.ProcessP(Single(imgIn), imgDrago);

            if(pyrA == NULL) {
                pyrA = new Pyramid(imgDrago, true);
            } else {
                pyrA->Update(imgDrago);
            }

            //Reinhard TMO
            imgReinhard = fltReinhardTMO.Process(Single(imgIn), imgReinhard);

            if(pyrB == NULL) {
                pyrB = new Pyramid(imgReinhard, true);
            } else {
                pyrB->Update(imgReinhard);
            }

            //Blending weight
            if(pyrWeight == NULL) {
                pyrWeight = new Pyramid(seg_map, false);
            } else {
                pyrWeight->Update(seg_map);
            }

            //Blending
            pyrA->Blend(pyrB, pyrWeight);
            pyrA->Reconstruct(imgOut);
        }
        break;
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_TONE_MAPPING_HYBRID_TMO_HPP */

