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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DAS_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DAS_HPP

#include <random>

#include "util/precomputed_gaussian.hpp"

#include "filtering/filter_sampling_map.hpp"

#include "point_samplers/sampler_random_m.hpp"

namespace pic {

/**
 * @brief The FilterBilateral2DAS class
 */
class FilterBilateral2DAS: public Filter
{
protected:
    float					sigma_s, sigma_r;

    MRSamplers<2>			*ms;

    int						halfSizeKernel;
    PrecomputedGaussian		*pg;

    //Process in a box
    void ProcessBBox(Image *dst, ImageVec src, BBox *box);

public:
    /**
     * @brief FilterBilateral2DAS
     */
    FilterBilateral2DAS();

    /**
     * @brief FilterBilateral2DAS
     * @param type
     * @param sigma_s
     * @param sigma_r
     * @param mult
     */
    FilterBilateral2DAS(SAMPLER_TYPE type, float sigma_s, float sigma_r, int mult);

    ~FilterBilateral2DAS();

    /**
     * @brief Signature
     * @return
     */
    std::string Signature()
    {
        return GenBilString("AS", sigma_s, sigma_r);
    }

    /**
     * @brief Execute
     * @param imgIn
     * @param imgOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *Execute(Image *imgIn, Image *imgOut, float sigma_s, float sigma_r)
    {
        FilterSamplingMap fsm(sigma_s);
        Image *samplingMap = fsm.ProcessP(Single(imgIn), NULL);
        *samplingMap /= samplingMap->getMaxVal(NULL, NULL)[0];

        FilterBilateral2DAS fltBil2DAS(ST_DARTTHROWING, sigma_s, sigma_r, 1);
        imgOut = fltBil2DAS.Process(Double(imgIn, samplingMap), imgOut);

        delete samplingMap;

        return imgOut;
    }

    /**
     * @brief Execute
     * @param nameIn
     * @param nameOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *Execute(std::string nameIn, std::string nameOut, float sigma_s,
                             float sigma_r)
    {
        Image imgIn(nameIn);
        
        long t0 = timeGetTime();
        Image *imgOut = Execute(&imgIn, NULL, sigma_s, sigma_r);
        long t1 = timeGetTime();
        printf("Stochastic Adaptive Bilateral Filter time: %ld\n", t1 - t0);

        imgOut->Write(nameOut);
        return imgOut;
    }
};

FilterBilateral2DAS::FilterBilateral2DAS()
{
    pg = NULL;
    ms = NULL;
}

FilterBilateral2DAS::~FilterBilateral2DAS()
{
    if(pg != NULL) {
        delete pg;
    }
       
    if(ms != NULL) {
        delete ms;
    }
}

FilterBilateral2DAS::FilterBilateral2DAS(SAMPLER_TYPE type, float sigma_s,
        float sigma_r, int mult = 1)
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    //Precomputation of the Gaussian Kernel
    pg = new PrecomputedGaussian(sigma_s);

    //Poisson samples
    if(mult > 0) {
        ms = new MRSamplers<2>(type, pg->halfKernelSize, pg->halfKernelSize * mult, 3,
                               64);
    } else if(mult < 0) {
        mult = -mult;
        ms = new MRSamplers<2>(type, pg->halfKernelSize, pg->halfKernelSize / mult, 3,
                               64);
    }
}

void FilterBilateral2DAS::ProcessBBox(Image *dst, ImageVec src, BBox *box)
{
    int width = dst->width;
    int height = dst->height;
    int channels = dst->channels;

    //Filtering
    float Gauss1, Gauss2;
    float  tmp, tmp2, tmp3, sum;
    int c2, ci, cj;

    Image *edge, *base, *samplingMap;

    if(src.size() == 3) {//Joint/Cross Bilateral Filtering
        base = src[0];
        edge = src[1];
        samplingMap = src[2]; 
    } else {
        base = src[0];
        edge = src[0];
        samplingMap = src[1];    
    }

    ImageSamplerBilinear	isb;

    float sigma_r2 = (sigma_r * sigma_r * 2.0f);
    bool sumTest;

    RandomSampler<2> *ps;
    float valOut[3];

    //Mersenne Twister
    std::mt19937 m(rand() % 10000);

    for(int i = box->x0; i < box->x1; i++) {
        float x = float(i) / float(width);

        for(int j = box->y0; j < box->y1; j++) {

            //Convolution kernel
            float *tmp_dst  = (*dst)(j, i);
            float *tmp_base = (*base)(j, i);
            float *tmp_edge = (*edge)(j, i);

            for(int l = 0; l < channels; l++) {
                tmp_dst[l] = 0.0f;
            }

            ps = ms->getSampler(&m);

            //Calculating the number of samples
            float y = float(j) / float(height);
            isb.SampleImage(samplingMap, x, y, valOut);

            float tmpValOut = 1.0f - valOut[0]; //+valOut[1]+valOut[2])/3.0f;
            float levelVal = CLAMPi(tmpValOut, 0.0f, 0.9f) * float(ps->levelsR.size());

            int levelInt = int(floorf(levelVal));
            int nSamples = ps->levelsR[levelInt];

            int levelsRsize = (ps->levelsR.size() - 1);
            if(levelInt < levelsRsize) {
                if((levelVal - float(levelInt)) > 0.0f) {
                    nSamples += int(float(ps->levelsR[levelInt + 1] - ps->levelsR[levelInt]) *
                                (levelVal - float(levelInt)));
                }
            }

            if((nSamples % 2) == 1) {
                nSamples++;
            }

            nSamples = MIN(nSamples, pg->halfKernelSize * pg->halfKernelSize * 2);

            sum = 0.0f;

            for(int k = 0; k < nSamples; k += 2) {
                //Spatial Gaussian kernel
                Gauss1 =	pg->coeff[ps->samplesR[k    ] + pg->halfKernelSize] *
                            pg->coeff[ps->samplesR[k + 1] + pg->halfKernelSize];

                //Address
                ci = CLAMP(i + ps->samplesR[k    ], width);
                cj = CLAMP(j + ps->samplesR[k + 1], height);
                c2 = (cj * width + ci) * channels;

                //Range Gaussian Kernel
                tmp = 0.0f;

                for(int l = 0; l < channels; l++) {
                    tmp3 = edge->data[c2 + l] - tmp_edge[l];
                    tmp += tmp3 * tmp3;
                }

                Gauss2 = expf(-tmp / sigma_r2);

                //Weight
                tmp2 = Gauss1 * Gauss2;
                sum += tmp2;

                //Filtering
                for(int l = 0; l < channels; l++) {
                    tmp_dst[l] += base->data[c2 + l] * tmp2;
                }
            }

            //Normalization
            sumTest = sum > 0.0f;

            for(int l = 0; l < channels; l++) {
                tmp_dst[l] = sumTest ? tmp_dst[l] / sum : tmp_base[l];
            }
        }
    }
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DAS_HPP */

//levelVal = 1.0f-levelVal;
//int nSamples = int(ceilf(levelVal*ps->levelsR[0]));

/*int end,start;
if(levelVal==0)
	start = 0;
else
	start = ps->levelsR[levelVal-1];
end = ps->levelsR[levelVal];*/

/*
			sum = 0.0;
			for(k=start;k<end;k+=2){
				//Spatial Gaussian kernel
				Gauss1=	pg->coeff[ps->samplesR[k]+pg->halfKernelSize]*
						pg->coeff[ps->samplesR[k+1]+pg->halfKernelSize];

				//Address
				ci=CLAMP(i+ps->samplesR[k],width);
				cj=CLAMP(j+ps->samplesR[k+1],height);
				c2=(cj*width+ci)*channels;

				//Range Gaussian Kernel
				tmp = 0.0;
				for(l=0;l<channels;l++){
					tmp3=edge->data[c2+l]-I_val[l];
					tmp+=tmp3*tmp3;
				}

				Gauss2=exp(-tmp*inv_sigma_r2);

				//Weight
				tmp2=Gauss1*Gauss2;
				sum+=tmp2;

				//Filtering
				for(l=0;l<channels;l++)
					tmpC[l]+=base->data[c2+l]*tmp2;
			};
*/
