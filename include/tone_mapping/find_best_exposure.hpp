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

#ifndef PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP
#define PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP

#include "image_raw.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_simple_tmo.hpp"
#include "histogram.hpp"

namespace pic {

/**This function returns the best exposure value for an image, img,
in f-stops values.*/
float FindBestExposure(ImageRAW *img)
{
    if(img==NULL) {
        return 0.0f;
    }

    ImageRAW *lum = FilterLuminance::Execute(img, NULL, LT_CIE_LUMINANCE);
    Histogram hist(lum, VS_LOG_2, 1024, 0);

    float fstop = hist.FindBestExposure(8.0f);

    delete lum;

    return fstop;
}


void FindBestExposureMain(std::string nameIn, std::string nameOut)
{
    ImageRAW img(nameIn);

    float fstop = FindBestExposure(&img);
    printf("Exposure for image %s is %f\n", nameIn.c_str(), fstop);

    ImageRAW *imgOut = FilterSimpleTMO::Execute(&img, NULL, 1.0f / 2.2f, fstop);
    imgOut->Write(nameOut);
}

} // end namespace pic

#endif /* PIC_TONE_MAPPING_FIND_BEST_EXPOSURE_HPP */

/*
	ImageRAW *lum = FilterLuminance::Execute(img,NULL);

	float minValue = lum->getMinVal();
	float fstop;

	if(minValue<=0.0f)
		fstop = -20.0f;
	else
		fstop = logf(minValue)/logf(2.0f);

	int delta = 1;

	FilterSimpleTMO flt(2.2f,fstop);

	ImageRAW *tmpTMO = lum->Clone();
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

