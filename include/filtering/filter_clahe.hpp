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

#ifndef PIC_FILTERING_FILTER_CLAHE_HPP
#define PIC_FILTERING_FILTER_CLAHE_HPP

#include "../base.hpp"
#include "../histogram.hpp"
#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilerCLAHE class
 */
class FilerCLAHE: public Filter
{
protected:
    int halfSize, nBin;
    uint value;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        Histogram hist, hist_uni;
        hist_uni.uniform(0.0f,
                         1.0f,
                         value, VS_LIN, nBin);
        float *c_t = hist_uni.cumulativef(true);
        int channels = src[0]->channels;

        for(int j = box->y0; j < box->y1; j++) {
            for(int i = box->x0; i < box->x1; i++) {
                float *in  = (*src[0]) (i, j);
                float *out = (*dst) (i, j);

                BBox box_ij(i - halfSize, i + halfSize, j - halfSize, j + halfSize);

                for(int ch = 0; ch < channels; ch++) {
                    hist.calculate(src[0], VS_LIN, nBin, &box_ij, ch);
                    float *c_s = hist.cumulativef(true);

                    hist_uni.update(hist.getfMin(), hist.getfMax());

                    int ind_source = hist.project(in[ch]);

                    float x = c_s[ind_source];
                    float *ptr = std::upper_bound(c_t, c_t + nBin, x);
                    int ind_target = MAX((int)(ptr - c_t), 0);

                    out[ch] = hist_uni.unproject(ind_target);
                }
            }
        }
    }

public:
    /**
     * @brief FilerCLAHE
     * @param size
     */
    FilerCLAHE(int size) : Filter()
    {
        update(size);
    }

    /**
     * @brief update
     * @param size
     */
    void update(int size)
    {
        this->halfSize = checkHalfSize(size);
        this->nBin = 1024;//size;

        uint area = halfSize * halfSize;
        this->value = MAX(area / nBin, 1);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param size
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int size)
    {
        FilerCLAHE filter(size);
        return filter.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_CLAHE_HPP */

