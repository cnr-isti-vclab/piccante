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

#ifndef PIC_ALGORITHMS_DISCRETE_COSINE_TRANSFORM_HPP
#define PIC_ALGORITHMS_DISCRETE_COSINE_TRANSFORM_HPP

#include "../image.hpp"
#include "../util/tile_list.hpp"

namespace pic {

/**
 * @brief The DCT class provides a reference
 * implementation for Discret Cosine Transform.
 */
class DCT
{
public:

    /**
     * @brief DCT
     */
    DCT()
    {
    }

    /**
     * @brief transform computes the forward DCT transformation.
     * @param imgIn is an input image.
     * @param imgOut is an output image; i.e. imgIn in the DCT domain.
     * @param size is the size of blocks (size * size) for computing the DCT.
     * @return
     */
    static Image *transform(Image *imgIn, Image *imgOut, int size = 8)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->allocateSimilarOne();
        }

        if(size < 1) {
            size = 8;
        }

        float size2 = float(size * 2);
        float squareRoot2 = sqrtf(2.0f);
        float squareRoot = sqrtf(2.0f / float(size));
        float squareRootPow2 = squareRoot * squareRoot;

        int channels = imgIn->channels;

        TileList tiles(size, imgOut->width, imgOut->height);

        for(unsigned int t = 0; t < tiles.tiles.size(); t++) {
            BBox box = tiles.getBBox(t);

            for(int v = box.y0; v < box.y1; v++) {
                int vr = v % size;

                for(int u = box.x0; u < box.x1; u++) {
                    int ur = u % size;

                    float *dataOut = (*imgOut)(u, v);

                    for(int p = 0; p < channels; p++) {
                        dataOut[p] = 0.0f;
                    }

                    for(int y = 0; y < size; y++) {
                        int yi		=  y + box.y0;
                        int	val		= vr * (2 * y + 1);
                        float cosU	= cosf(C_PI * float(val) / size2);

                        for(int x = 0; x < size; x++) {
                            int xi		=  x + box.x0;
                            val			= ur * (2 * x + 1);
                            float tot	= cosU * cosf(C_PI * float(val) / size2);

                            float *dataIn = (*imgIn)(xi, yi);

                            for(int p = 0; p < channels; p++) {
                                dataOut[p] += tot * dataIn[p];
                            }
                        }
                    }

                    if(ur == 0) {
                        for(int p = 0; p < channels; p++) {
                            dataOut[p] /= squareRoot2;
                        }
                    }

                    if(vr == 0) {
                        for(int p = 0; p < channels; p++) {
                            dataOut[p] /= squareRoot2;
                        }
                    }

                    for(int p = 0; p < channels; p++) {
                        dataOut[p] *= squareRootPow2;
                    }
                }
            }
        }

        return imgOut;
    }

    /**
     * @brief inverse computes the inverse DCT transformation.
     * @param imgIn is an input image in the DCT domain.
     * @param imgOut is an output image; i.e. imgIn in spatial domain.
     * @param size is the size of blocks (size * size) for computing the DCT.
     * @return
     */
    static Image *inverse(Image *imgIn, Image *imgOut, int size = 8)
    {
        if(imgIn == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = imgIn->allocateSimilarOne();
        }

        if(size < 1) {
            size = 8;
        }

        float size2 = float(size * 2);
        float squareRoot2 = sqrtf(2.0f);
        float squareRoot = sqrtf(2.0f / float(size));
        float squareRootPow2 = squareRoot * squareRoot;

        int channels = imgIn->channels;

        TileList tiles(size, imgOut->width, imgOut->height);

        for(unsigned int t = 0; t < tiles.tiles.size(); t++) {
            BBox box = tiles.getBBox(t);

            for(int y = box.y0; y < box.y1; y++) {
                int yr = y % size;

                for(int x = box.x0; x < box.x1; x++) {
                    int xr = x % size;

                    float *dataOut = (*imgOut)(x, y);

                    for(int p = 0; p < channels; p++) {
                        dataOut[p] = 0.0f;
                    }

                    for(int v = 0; v < size; v++) {
                        int vi		= box.y0 + v;
                        int	val		= v * (2 * yr + 1);
                        float cosU	= cosf(C_PI * float(val) / size2);

                        if(v == 0) {
                            cosU /= squareRoot2;
                        }

                        for(int u = 0; u < size; u++) {
                            int ui		= box.x0 + u;
                            val			= u * (2 * xr + 1);
                            float tot	= cosU * cosf(C_PI * float(val) / size2);

                            if(u == 0) {
                                tot /= squareRoot2;
                            }

                            float *dataIn = (*imgIn)(ui, vi);

                            for(int p = 0; p < channels; p++) {
                                dataOut[p] += tot * dataIn[p];
                            }
                        }
                    }

                    for(int p = 0; p < channels; p++) {
                        dataOut[p] *= squareRootPow2;
                    }
                }
            }
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_DISCRETE_COSINE_TRANSFORM_HPP */

