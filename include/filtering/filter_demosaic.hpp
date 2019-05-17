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

#ifndef PIC_FILTERING_FILTER_DEMOSAIC_HPP
#define PIC_FILTERING_FILTER_DEMOSAIC_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterDemosaic class
 */
class FilterDemosaic: public Filter
{
protected:

    /**
     * @brief LinearUpSamplingGCGreen this upsamples the green channel with gradient correction
     * @param imgIn
     * @param imgOut
     */
    void LinearUpSamplingGCGreen(Image *imgIn, Image *imgOut)
    {
        int height = imgIn->height;
        int width = imgIn->width;

        float *dataIn = imgIn->data;
        float *dataOut = imgOut->data;

        //copy the original Green pixels into the U16RGB buffer
        for(int j = 0; j < height; j++) {
            int tmp = j * width;
            for(int i = ((j + 1) % 2); i < width; i += 2) {
                int current = tmp + i;
                dataOut[current * 3 + 1] = dataIn[current];
            }
        }

        //edge-aware interpolation for the missing Green pixels
        //#pragma omp parallel for
        for(int k = 0; k < 2; k++) {
            for(int j = k; j < (height); j += 2) {
                float tmpG, sum, Grad, final;

                int tmp = j * width;

                for(int i = k; i < (width); i += 2) {
                    //    -1
                    //     2
                    //-1 2 4 2 -1
                    //     2
                    //    -1

                    int current = tmp + i;

                    tmpG  = (dataIn[imgIn->getAddress(i + 1, j)] +
                             dataIn[imgIn->getAddress(i - 1, j)] +
                             dataIn[imgIn->getAddress(i, j + 1)] +
                             dataIn[imgIn->getAddress(i, j - 1)]) * 0.25f;

                    sum  = (dataIn[imgIn->getAddress(i + 2, j)] +
                            dataIn[imgIn->getAddress(i - 2, j)] +
                            dataIn[imgIn->getAddress(i, j + 2)] +
                            dataIn[imgIn->getAddress(i, j - 2)]);

                    Grad = dataIn[current] - sum * 0.25f;

                    final = tmpG + Grad * 0.5f;

                    dataOut[current * 3 + 1] = CLAMPi(final, 0.0f, 1.0f);
                }
            }
        }
    }

    /**
     * @brief LinearUpSamplingGCRB this linearly upsamples Red and Blue channels
     * @param imgIn
     * @param imgOut
     * @param sx
     * @param sy
     */
    void LinearUpSamplingGCRB(Image *imgIn, Image *imgOut, int sx,
                                         int sy)
    {
        int i, j;
        int shifter = sx + sy;

        int height = imgIn->height;
        int width = imgIn->width;
        float *dataIn = imgIn->data;
        float *data = imgOut->data;

        //copy the original pixels!
        //#pragma omp parallel for
        for(j = sy; j < height; j += 2) {
            for(i = sx; i < width; i += 2) {
                int current = j * width + i;
                data[current * 3 + shifter] = dataIn[current];
            }
        }

        //Edge-aware interpolation for the missing Green pixels
        int ssx = 0;
        int ssy = 0;

        if(shifter == 0) {
            ssx = 1;
            ssy = 0;
        }

        if(shifter == 2) {
            ssx = 0;
            ssy = 1;
        }

        //First Mask
        //#pragma omp parallel for
        for(j = ssy; j < (height); j += 2) {
            float tmp;

            for(i = ssx; i < (width); i += 2) {
                //
                //          0.5
                //      -1   0  -1
                //  -1   4   5   4   -1
                //      -1   0  -1
                //          0.5
                //
                int current = j * width + i;

                tmp =	5.0f *	dataIn[current] +
                        4.0f * (dataIn[imgIn->getAddress(i + 1, j)] +	dataIn[imgIn->getAddress(i - 1,
                                j)]) +
                        0.5f * (dataIn[imgIn->getAddress(i, j + 2)] +	dataIn[imgIn->getAddress(i,
                                j - 2)]) -
                        (dataIn[imgIn->getAddress(i + 1, j + 1)] +	dataIn[imgIn->getAddress(i + 1,
                                j - 1)] +
                         dataIn[imgIn->getAddress(i - 1, j + 1)] +	dataIn[imgIn->getAddress(i - 1,
                                 j - 1)] +
                         dataIn[imgIn->getAddress(i - 2, j)] +	dataIn[imgIn->getAddress(i + 2, j)]);
                tmp /= 8.0f;
                data[current * 3 + shifter] = CLAMPi(tmp, 0.0f, 1.0f);
            }
        }

        //Second Mask
        if(shifter == 0) {
            ssx = 0;
            ssy = 1;
        }

        if(shifter == 2) {
            ssx = 1;
            ssy = 0;
        }

        //#pragma omp parallel for
        for(j = ssy; j < (height); j += 2) {
            float tmp;

            for(i = ssx; i < (width); i += 2) {
                //
                //           -1
                //       -1   4  -1
                //  0.5   0   5   0   0.5
                //       -1   4  -1
                //           -1
                //
                int current = j * width + i;

                tmp =	5.0f * dataIn[current] +
                        4.0f * (dataIn[imgIn->getAddress(i, j + 1)] +	dataIn[imgIn->getAddress(i,
                                j - 1)]) +
                        +0.5f * (dataIn[imgIn->getAddress(i - 2, j)] +	dataIn[imgIn->getAddress(i + 2,
                                 j)]) -
                        (dataIn[imgIn->getAddress(i + 1, j + 1)] +	dataIn[imgIn->getAddress(i + 1,
                                j - 1)] +
                         dataIn[imgIn->getAddress(i - 1, j + 1)] +	dataIn[imgIn->getAddress(i - 1,
                                 j - 1)] +
                         dataIn[imgIn->getAddress(i, j + 2)] +	dataIn[imgIn->getAddress(i, j - 2)]);

                tmp /= 8.0f;
                data[current * 3 + shifter] = CLAMPi(tmp, 0.0f, 1.0f);
            }
        }

        //Third Mask
        if(shifter == 0) {
            ssx = 1;
            ssy = 1;
        }

        if(shifter == 2) {
            ssx = 0;
            ssy = 0;
        }

        //#pragma omp parallel for
        for(j = ssy; j < (height); j += 2) {
            float tmp;

            for(i = ssx; i < (width); i += 2) {
                //
                //           -3/2
                //         2   0   2
                //  -3/2   0   6   0   -3/2
                //         2   0   2
                //           -3/2
                //
                int current = j * width + i;
                tmp =	6.0f *	dataIn[current] +
                        2.0f * (dataIn[imgIn->getAddress(i + 1, j + 1)] +	dataIn[imgIn->getAddress(i + 1,
                                j - 1)] +
                                dataIn[imgIn->getAddress(i - 1, j + 1)] +	dataIn[imgIn->getAddress(i - 1,
                                        j - 1)]) -
                        1.5f * (dataIn[imgIn->getAddress(i + 2, j)] +	dataIn[imgIn->getAddress(i - 2,
                                j)] +
                                dataIn[imgIn->getAddress(i, j + 2)] +	dataIn[imgIn->getAddress(i, j - 2)]);

                tmp /= 8.0f;
                data[current * 3 + shifter] = CLAMPi(tmp, 0.0f, 1.0f);
            }
        }
    }

public:

    /**
     * @brief FilterDemosaic
     * @param type
     */
    FilterDemosaic() : Filter()
    {
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = 3;
        frames      = imgIn[0]->frames;
    }

    /**
      * @brief Filter::Process
      * @param imgIn
      * @param imgOut
      * @return
      */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(imgIn[0] == NULL) {
            return NULL;
        }

        if((!imgIn[0]->isValid()) && (imgIn[0]->channels != 1)) {
            return imgOut;
        }

        imgOut = setupAux(imgIn, imgOut);

        LinearUpSamplingGCGreen(imgIn[0], imgOut);
        LinearUpSamplingGCRB(imgIn[0], imgOut, 0, 0);
        LinearUpSamplingGCRB(imgIn[0], imgOut, 1, 1);

        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut)
    {
        FilterDemosaic flt;
        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DEMOSAIC_HPP */

