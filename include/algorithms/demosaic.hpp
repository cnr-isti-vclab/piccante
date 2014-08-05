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

#ifndef PIC_ALGORITHMS_DEMOSAIC_HPP
#define PIC_ALGORITHMS_DEMOSAIC_HPP

#include "image.hpp"

namespace pic {

//Linear upsampling of green channel with gradient correction
PIC_INLINE void LinearUpSamplingGCGreen(Image *imgIn, Image *imgOut)
{
    int height = imgIn->height;
    int width = imgIn->width;

    float *dataIn = imgIn->data;
    float *dataOut = imgOut->data;

    //Copying the original Green pixels into the U16RGB buffer
    for(int j = 0; j < height; j++) {
        int tmp = j * width;
        for(int i = ((j + 1) % 2); i < width; i += 2) {
            int current = tmp + i;
            dataOut[current * 3 + 1] = dataIn[current];
        }
    }

    //Edge-aware interpolation for the missing Green pixels
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

                tmpG  = (dataIn[imgIn->Address(i + 1, j)] +
                         dataIn[imgIn->Address(i - 1, j)] +
                         dataIn[imgIn->Address(i, j + 1)] +
                         dataIn[imgIn->Address(i, j - 1)]) * 0.25f;

                sum  = (dataIn[imgIn->Address(i + 2, j)] +
                        dataIn[imgIn->Address(i - 2, j)] +
                        dataIn[imgIn->Address(i, j + 2)] +
                        dataIn[imgIn->Address(i, j - 2)]);

                Grad = dataIn[current] - sum * 0.25f;

                final = tmpG + Grad * 0.5f;

                dataOut[current * 3 + 1] = CLAMPi(final, 0.0f, 1.0f);
            }
        }
    }
}

//Linear UpSampling with gradients: Red and Blue channels
PIC_INLINE void LinearUpSamplingGCRB(Image *imgIn, Image *imgOut, int sx,
                                     int sy)
{
    int i, j;
    int shifter = sx + sy;

    int height = imgIn->height;
    int width = imgIn->width;
    float *dataIn = imgIn->data;
    float *data = imgOut->data;

    //Copying the original pixels!
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
            //		    0.5
            //      -1   0  -1
            //  -1   4   5   4   -1
            //      -1   0  -1
            //		    0.5
            //
            int current = j * width + i;

            tmp =	5.0f *	dataIn[current] +
                    4.0f * (dataIn[imgIn->Address(i + 1, j)] +	dataIn[imgIn->Address(i - 1,
                            j)]) +
                    0.5f * (dataIn[imgIn->Address(i, j + 2)] +	dataIn[imgIn->Address(i,
                            j - 2)]) -
                    (dataIn[imgIn->Address(i + 1, j + 1)] +	dataIn[imgIn->Address(i + 1,
                            j - 1)] +
                     dataIn[imgIn->Address(i - 1, j + 1)] +	dataIn[imgIn->Address(i - 1,
                             j - 1)] +
                     dataIn[imgIn->Address(i - 2, j)] +	dataIn[imgIn->Address(i + 2, j)]);
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
            //		     -1
            //       -1   4  -1
            //  0.5   0   5   0   0.5
            //       -1   4  -1
            //		     -1
            //
            int current = j * width + i;

            tmp =	5.0f * dataIn[current] +
                    4.0f * (dataIn[imgIn->Address(i, j + 1)] +	dataIn[imgIn->Address(i,
                            j - 1)]) +
                    +0.5f * (dataIn[imgIn->Address(i - 2, j)] +	dataIn[imgIn->Address(i + 2,
                             j)]) -
                    (dataIn[imgIn->Address(i + 1, j + 1)] +	dataIn[imgIn->Address(i + 1,
                            j - 1)] +
                     dataIn[imgIn->Address(i - 1, j + 1)] +	dataIn[imgIn->Address(i - 1,
                             j - 1)] +
                     dataIn[imgIn->Address(i, j + 2)] +	dataIn[imgIn->Address(i, j - 2)]);

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
            //		     -3/2
            //         2   0   2
            //  -3/2   0   6   0   -3/2
            //         2   0   2
            //		     -3/2
            //
            int current = j * width + i;
            tmp =	6.0f *	dataIn[current] +
                    2.0f * (dataIn[imgIn->Address(i + 1, j + 1)] +	dataIn[imgIn->Address(i + 1,
                            j - 1)] +
                            dataIn[imgIn->Address(i - 1, j + 1)] +	dataIn[imgIn->Address(i - 1,
                                    j - 1)]) -
                    1.5f * (dataIn[imgIn->Address(i + 2, j)] +	dataIn[imgIn->Address(i - 2,
                            j)] +
                            dataIn[imgIn->Address(i, j + 2)] +	dataIn[imgIn->Address(i, j - 2)]);

            tmp /= 8.0f;
            data[current * 3 + shifter] = CLAMPi(tmp, 0.0f, 1.0f);
        }
    }
}

//Debayer
PIC_INLINE Image *Demosaic(Image *imgIn, Image *imgOut)
{
    if(imgIn == NULL) {
        return imgOut;
    }

    if((!imgIn->isValid()) && (imgIn->channels != 1)) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = new Image(1, imgIn->width, imgIn->height, 3);
    }
/*
    if(!imgOut->isValid()) {
        imgOut->Allocate(imgIn->width, imgIn->height, 3, imgIn->frames);
    }*/

    LinearUpSamplingGCGreen(imgIn, imgOut);
    LinearUpSamplingGCRB(imgIn, imgOut, 0, 0);
    LinearUpSamplingGCRB(imgIn, imgOut, 1, 1);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_DEMOSAIC_HPP */

