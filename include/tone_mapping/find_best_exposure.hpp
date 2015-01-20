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

#ifndef PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP
#define PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP

#include "image.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_simple_tmo.hpp"
#include "histogram.hpp"

namespace pic {

/**
 * @brief FindBestExposure computes the best exposure value for an image, img,
 * @param img
 * @return
 */
float FindBestExposure(Image *img)
{
    if(img == NULL) {
        return 0.0f;
    }

    Image *lum = FilterLuminance::Execute(img, NULL, LT_CIE_LUMINANCE);
    Histogram hist(lum, VS_LOG_2, 1024, 0);

    float fstop = hist.FindBestExposure(8.0f);

    delete lum;

    return fstop;
}

/**
 * @brief FindBestExposureMain
 * @param nameIn
 * @param nameOut
 */
void FindBestExposureMain(std::string nameIn, std::string nameOut)
{
    Image img(nameIn);

    float fstop = FindBestExposure(&img);
    printf("Exposure for image %s is %f\n", nameIn.c_str(), fstop);

    Image *imgOut = FilterSimpleTMO::Execute(&img, NULL, 1.0f / 2.2f, fstop);
    imgOut->Write(nameOut);
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP */

/*
	Image *lum = FilterLuminance::Execute(img,NULL);

	float minValue = lum->getMinVal();
	float fstop;

	if(minValue<=0.0f)
		fstop = -20.0f;
	else
		fstop = logf(minValue)/logf(2.0f);

	int delta = 1;

	FilterSimpleTMO flt(2.2f,fstop);

	Image *tmpTMO = lum->Clone();
	float *data = tmpTMO->data;

	int size = tmpTMO->width*tmpTMO->height*tmpTMO->channels;

	int prevCount = 0;

	bool neverTricked = true;
	while(delta>0||(delta<=0&&neverTricked)){
		flt.ProcessP(Single(lum),tmpTMO);

		int count = 0;
		for(int i=0;i<size;i++){
			if(data[i]>=minRange&&data[i]<=maxRange)
				count++;
		}

		fstop += 0.1f;		//mabye more adaptive?

		printf("%f %d\n",fstop,count);
		delta = 1;//count-prevCount;

		if(delta>0)
			neverTricked = false;

		prevCount = count;
		flt.Update(2.2f,fstop);
	}

	delete lum;
	delete tmpTMO;

	return fstop;
*/

