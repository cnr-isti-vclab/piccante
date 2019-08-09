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

#ifndef PIC_FILTERING_FILTER_NEAREST_NEIGHBORS_HPP
#define PIC_FILTERING_FILTER_NEAREST_NEIGHBORS_HPP

#include "../features_matching/patch_comp.hpp"
#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterNearestNeighbors class
 */
class FilterNearestNeighbors: public Filter
{
protected:

    PatchComp *pc;
    int patchSize, halfPatchSize, stride;
    int width_ps, height_ps;

    /**
     * @brief f
     * @param data
     */
    void f(FilterFData *data)
    {
        int xb, yb;
        float db = FLT_MAX;
        for(int i = halfPatchSize; i < height_ps; i+= stride) {
            for(int j = halfPatchSize; j < width_ps; j+= stride) {

                float tmp_d = pc->getSSD(data->x, data->y, j, i);

                if(tmp_d < db) {
                    db = tmp_d;
                    xb = j;
                    yb = i;
                }
            }
        }
        data->out[0] = float(xb);
        data->out[1] = float(yb);
        data->out[1] = db;
    }

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *setupAux(ImageVec imgIn, Image *imgOut)
    {
        height_ps = imgIn[1]->height - halfPatchSize;
        width_ps = imgIn[1]->width - halfPatchSize;

        pc = new PatchComp(imgIn[0], imgIn[1], patchSize);

        return allocateOutputMemory(imgIn, imgOut, bDelete);
    }

public:

    /**
     * @brief FilterNearestNeighbors
     */
    FilterNearestNeighbors() : Filter()
    {
        pc = NULL;
        this->minInputImages = 2;
    }

    ~FilterNearestNeighbors()
    {
        delete_s(pc);
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
     * @brief setup
     * @param patchSize
     * @param stride
     */
    void setup(int patchSize, int stride)
    {
        patchSize = (patchSize > 2) ? patchSize : 3;

        if((patchSize % 2) == 0) {
            patchSize++;
        }

        stride = stride > 0 ? stride : 1;

        this->stride = stride;
        this->patchSize = patchSize;

        pc = delete_s(pc);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_NEAREST_NEIGHBORS_HPP */

