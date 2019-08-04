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

#ifndef PIC_FILTERING_FILTER_WHITE_BALANCE_HPP
#define PIC_FILTERING_FILTER_WHITE_BALANCE_HPP

#include "../util/std_util.hpp"

#include "../filtering/filter.hpp"
#include "../util/array.hpp"

namespace pic {

/**
 * @brief The FilterWhiteBalance class
 */
class FilterWhiteBalance: public Filter
{
protected:

    float *white;
    int nWhite;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        if(white == NULL) {
            return;
        }

        int width    = src[0]->width;
        int channels = src[0]->channels;
        float *data  = src[0]->data;

        int transformChannels = MIN(channels, nWhite);

        for(int j = box->y0; j < box->y1; j++) {
            int c = j * width;

            for(int i = box->x0; i < box->x1; i++) {
                int indOut = c + i;
                int ind = indOut * channels;

                for(int k = 0; k < transformChannels; k++) {
                    dst->data[ind + k] = data[ind + k] * white[k];
                }
            }
        }
    }

public:

    /**
     * @brief FilterWhiteBalance
     * @param type
     */
    FilterWhiteBalance()
    {
        white = NULL;
        nWhite = -1;
    }

    /**
     * @brief FilterWhiteBalance
     * @param white
     * @param nWhite
     * @param bComputeScalingFactors
     */
    FilterWhiteBalance(float *white, unsigned int nWhite, bool bComputeScalingFactors)
    {
        this->white = NULL;
        this->nWhite = -1;

        update(white, nWhite, bComputeScalingFactors);
    }

    ~FilterWhiteBalance()
    {
        delete_vec_s(white);
        nWhite = -1;
    }

    /**
     * @brief getScalingFactors
     * @param white
     * @param nWhite
     * @return
     */
    static float *getScalingFactors(float *white, int nWhite)
    {
        if(white == NULL || nWhite < 1) {
            return NULL;
        }

        float white_mean = Arrayf::sum(white, nWhite) / float(nWhite);

        float *out = new float[nWhite];

        for(int i = 0; i < nWhite; i++) {
            if(white[i] > 0.0f) {
                out[i] = white_mean / white[i];
            } else {
                out[i] = 1.0f;
            }
        }

        return out;
    }

    /**
     * @brief update
     * @param white
     * @param nWhite
     * @param bComputeScalingFactors
     */
    void update(float *white, unsigned int nWhite, bool bComputeScalingFactors)
    {
        this->nWhite = nWhite;

        delete_vec_s(this->white);

        if(bComputeScalingFactors) {
             this->white = getScalingFactors(white, nWhite);
        } else {
            this->white = new float[nWhite];
            memcpy(this->white, white, sizeof(float) * nWhite);
        }

        for(unsigned int i = 0; i < nWhite; i++) {
            if(fabsf(this->white[i]) <= 1e-9f) {
                this->white[i] = 1.0f;
            }
        }
    }

    /**
     * @brief execute
     * @param imgIn
     * @param white_color
     * @param out
     * @return
     */
    static Image* execute(Image *imgIn, float *white_color, Image *out)
    {
        if(imgIn == NULL || white_color == NULL) {
            return NULL;
        }

        FilterWhiteBalance flt_wb(white_color, imgIn->channels, true);
        out = flt_wb.Process(Single(imgIn), out);

        return out;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param x
     * @param y
     * @param bRobust
     * @param out
     * @return
     */
    static Image* execute(Image *imgIn, int x, int y, bool bRobust = true, Image *out = NULL)
    {
        if(imgIn == NULL) {
            return NULL;
        }

        float *white_color = NULL;

        int patchSize = 5;

        if(!bRobust) {
            white_color = (*imgIn)(x, y);
        } else {
            BBox patch(x - patchSize, x + patchSize, y - patchSize, y + patchSize);
            white_color = imgIn->getMeanVal(&patch, NULL);
        }

        out = execute(imgIn, white_color, out);

        if(bRobust) {
            delete_vec_s(white_color);
        }

        return out;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_WHITE_BALANCE_HPP */

