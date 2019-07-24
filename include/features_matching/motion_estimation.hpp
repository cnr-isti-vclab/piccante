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

#ifndef PIC_FEATURES_MATCHING_MOTION_ESTIMATION_HPP
#define PIC_FEATURES_MATCHING_MOTION_ESTIMATION_HPP

#include <functional>

#include "../image.hpp"
#include "../util/std_util.hpp"
#include "../features_matching/patch_comp.hpp"

namespace pic {

/**
 * @brief The MotionEstimation class
 */
class MotionEstimation
{
protected:
    int shift, blockSize, halfBlockSize;
    int width, height;
    PatchComp *pmc;

    /**
     * @brief processAux
     * @param tiles
     * @param imgOut
     */
    void processAux(TileList *tiles, Image *imgOut)
    {
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
    MotionEstimation(Image *img0, Image *img1, int blockSize, int maxRadius)
    {
        pmc = NULL;

        setup(img0, img1, blockSize, maxRadius);
    }

    ~MotionEstimation()
    {
        delete_s(pmc);
    }

    /**
     * @brief setup
     * @param img0
     * @param img1
     * @param blockSize
     * @param maxRadius
     */
    void setup(Image *img0, Image *img1, int blockSize, int maxRadius)
    {
        if(img0 == NULL || img1 == NULL) {
            return;
        }

        if(!img0->isSimilarType(img1)) {
            return;
        }

        if(maxRadius < 1) {
            maxRadius = 1;
        }

        //estimate the blockSize if not given
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
     * @brief execute
     * @param imgOut
     * @return
     */
    Image *process(Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, width, height, 3);
        }

        TileList lst(blockSize, width, height);

        //create threads
        int numCores = std::thread::hardware_concurrency();

        std::thread **thrd = new std::thread*[numCores];

        for(int i = 0; i < numCores; i++) {
            thrd[i] = new std::thread(
                std::bind(&MotionEstimation::processAux, this, &lst, imgOut));
        }

        //threads join
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
    static Image *execute(Image *img0, Image *img1, int blockSize, int maxRadius, Image *imgOut)
    {
        MotionEstimation me(img0, img1, blockSize, maxRadius);

        return me.process(imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_MOTION_ESTIMATION_HPP */
