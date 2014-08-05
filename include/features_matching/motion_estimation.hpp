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

#ifndef PIC_FEATURES_MATCHING_MOTION_ESTIMATION_HPP
#define PIC_FEATURES_MATCHING_MOTION_ESTIMATION_HPP

#include "image_raw.hpp"

#include "features_matching/patch_comp.hpp"

namespace pic {

class MotionEstimation {
protected:
    int         shift, blockSize, halfBlockSize;
    int         width, height;
    PatchComp   *pmc;

    /**
     * @brief ProcessAux
     * @param tiles
     * @param imgOut
     */
    void ProcessAux(TileList *tiles, ImageRAW *imgOut) {
        bool state = true;

        while(state) {
            unsigned int currentTile = tiles->getNext();

            if(currentTile < tiles->tiles.size()) {
                int x = tiles->tiles[currentTile].startX;
                int y = tiles->tiles[currentTile].startY;

                int x0 = x + halfBlockSize;
                int y0 = y + halfBlockSize;


                int x_e = MAX((x + blockSize), imgOut->width);
                int y_e = MAX((y + blockSize), imgOut->height);

                int dx = 0;
                int dy = 0;
                float err = FLT_MAX;


                for(int k=-shift; k<=shift; k++) {
                    int y1 = y0 + k;

                    for(int l=-shift; l<=shift; l++) {
                        int x1 = x0 + l;

                        float tmp_err = pmc->getSSD(x0, y0, x1, y1);

                        if(tmp_err < err) {
                            err = tmp_err;
                            dx = l;
                            dy = k;
                        }
                    }
                }

                for(int k=y; k<y_e; k++) {
                    for(int l=x; l<x_e; l++) {

                        float *data = (*imgOut)(l, k);
                        data[0] = float(dx);
                        data[1] = float(dy);
                        data[2] = err;
                    }
                }

            } else {
                state = false;
            }
        }
    }

public:

    /**
     * @brief MotionEstimation
     * @param img0
     * @param img1
     * @param blockSize
     * @param maxRadius
     */
    MotionEstimation(ImageRAW *img0, ImageRAW *img1, int blockSize, int maxRadius) {
        pmc = NULL;

        Setup(img0, img1, blockSize, maxRadius);
    }

    ~MotionEstimation() {
        if(pmc != NULL) {
            delete pmc;
        }
    }

    /**
     * @brief Setup
     * @param img0
     * @param img1
     * @param blockSize
     * @param maxRadius
     */
    void Setup(ImageRAW *img0, ImageRAW *img1, int blockSize, int maxRadius) {
        if(img0 == NULL || img1 == NULL) {
            return;
        }

        if(!img0->SimilarType(img1)) {
            return;
        }

        if(maxRadius < 1) {
            maxRadius = 1;
        }

        //estimating the blockSize if not inserted
        if(blockSize < 1) {
            float nPixels = float(img0->nPixels());
            float tmp = ceilf(log10f(nPixels));
            blockSize = MAX(int(powf(2.0f, tmp)), 4);
        }

        this->blockSize = blockSize;
        this->halfBlockSize = blockSize >> 1;
        this->shift = maxRadius * blockSize;

        this->width = img0->width;
        this->height = img0->height;

        pmc = new PatchComp(img0, img1, blockSize);
    }

    /**
     * @brief Process
     * @param imgOut
     * @return
     */
    ImageRAW *Process(ImageRAW *imgOut) {
        if(imgOut == NULL) {
            imgOut = new ImageRAW(1, width, height, 3);
        }

        TileList lst(blockSize, width, height);

        //Creating threads
        int numCores = std::thread::hardware_concurrency();

        std::thread **thrd = new std::thread*[numCores];

        for(int i = 0; i < numCores; i++) {
            thrd[i] = new std::thread(
                std::bind(&MotionEstimation::ProcessAux, this, &lst, imgOut));
        }

        //Threads join
        for(int i = 0; i < numCores; i++) {
            thrd[i]->join();
        }

        return imgOut;
    }

    /**
     * @brief Execute
     * @param img0
     * @param img1
     * @param blockSize
     * @param maxRadius
     * @param imgOut
     * @return
     */
    static ImageRAW *Execute(ImageRAW *img0, ImageRAW *img1, int blockSize, int maxRadius, ImageRAW *imgOut) {
        MotionEstimation me(img0, img1, blockSize, maxRadius);

        return me.Process(imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_MOTION_ESTIMATION_HPP */
