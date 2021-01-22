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

#ifndef PIC_ALGORITHMS_MULTI_RESOLUTION_OPERATOR_HPP
#define PIC_ALGORITHMS_MULTI_RESOLUTION_OPERATOR_HPP

#include "../util/math.hpp"
#include "../image.hpp"
#include "../image_vec.hpp"
#include "../algorithms/pyramid.hpp"

namespace pic {

class MultiResolutionOperator
{
protected:
    int pyramid_limit;

public:

    MultiResolutionOperator()
    {
        pyramid_limit = 4;
    }

    /**
     * @brief setup
     * @param imgIn
     * @param pIn
     * @param imgOut
     * @return
     */
    virtual Image *setup(ImageVec imgIn, std::vector<Pyramid *> pIn, Image *imgOut)
    {
        return imgOut;
    }

    /**
     * @brief f
     * @param imgIn
     * @param imgOut
     * @param level
     * @return
     */
    virtual Image* f(ImageVec imgIn, Image *imgOut, int level)
    {
        return imgOut;
    }

    virtual Image* upsample(ImageVec imgIn, Image *imgOut)
    {
        return imgOut;
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        std::vector<Pyramid *> pIn;

        int n = (1 << 30) - 1;
        for(uint i = 0; i < imgIn.size(); i++) {
            Pyramid *s_i = new Pyramid(imgIn[i], false, pyramid_limit);
            pIn.push_back(s_i);

            n = MIN(n, s_i->size());
        }

        imgOut = setup(imgIn, pIn, imgOut);

        bool bFirst = true;

        for(int i = (n - 1); i >= 0; i--) {

            ImageVec imgIn_i;
            for(uint j = 0; j < pIn.size(); j++) {
                imgIn_i.push_back(pIn[j]->get(i));
            }

            if(!bFirst) {
                #ifdef PIC_DEBUG
                    printf("Upsampling..");
                #endif

                imgOut = upsample(imgIn_i, imgOut);

                #ifdef PIC_DEBUG
                    printf("ok\n");
                #endif
            }

            int level = n - i;
            #ifdef PIC_DEBUG
                printf("Level: %d\n", level);
            #endif

            imgOut = f(imgIn, imgOut, level);
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_MULTI_RESOLUTION_OPERATOR_HPP */

