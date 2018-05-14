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

#ifndef PIC_FEATURES_MATCHING_LUCID_DESCRIPTOR_HPP
#define PIC_FEATURES_MATCHING_LUCID_DESCRIPTOR_HPP

#include "../util/math.hpp"
#include "../image.hpp"

namespace pic {

/**
 * @brief The LUCIDDescriptor class
 */
class LUCIDDescriptor
{
protected:
    int patchSize, halfPatchSize;
    int area;

public:

    /**
     * @brief LUCIDDescriptor
     * @param patchSize
     */
    LUCIDDescriptor(int patchSize = 31)
    {
        if(patchSize < 2) {
            patchSize = 31;
        }

        this->patchSize = patchSize;
        this->halfPatchSize = patchSize >> 1;

        int tmp = (halfPatchSize << 1 ) + 1;
        area = tmp * tmp;
    }

    /**
     * @brief get computes a descriptor at position (x0,y0) with size n.
     * @param img
     * @param x0
     * @param y0
     * @param desc
     * @param nDesc
     * @return
     */
    unsigned int *get(Image *img, int x0, int y0, unsigned int *desc, unsigned int &nDesc)
    {
        if(img == NULL) {
            return NULL;
        }

        nDesc = area * img->channels;

        if(desc == NULL) {
            desc = new unsigned int[nDesc];
        }

        std::vector< std::pair<float, unsigned int> > data;


        for(int k = 0; k < img->channels; k++) {
            for(int i = -halfPatchSize; i <= halfPatchSize; i++) {
                int y = y0 + i;

                for(int j = -halfPatchSize; j <= halfPatchSize; j++) {
                    int x = x0 + j;

                    float *tmp_val = (*img)(x, y);

                    float f = tmp_val[k];
                    int s = int(data.size());

                    std::pair<float, int> pair = std::make_pair(f, s);
                    data.push_back(pair);
                }
            }
        }

        std::sort(data.begin(), data.end());

        for(unsigned int i = 0; i < data.size(); i++) {
            desc[i] = data[i].second;
        }

        return desc;
    }

    /**
     * @brief match matches two descriptors. Note: higher scores means better matching.
     * @param fv0
     * @param fv1
     * @param nfv
     * @return
     */
    static unsigned int match(unsigned int *fv0, unsigned int *fv1, unsigned int nfv)
    {
        if((fv0 == NULL) && (fv1 == NULL)) {
            return 0;
        }

        unsigned int ret = 0;

        for(unsigned int i = 0; i < nfv; i++) {
            ret += (fv0[i] == fv1[i]) ? 1 : 0;
        }

        return ret;
    }
};

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_LUCID_DESCRIPTOR_HPP */

