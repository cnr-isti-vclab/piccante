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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_GRAY_MATCH_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_GRAY_MATCH_HPP

#include "../util/std_util.hpp"
#include "../util/nelder_mead_opt_base.hpp"
#include "../util/array.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class NelderMeadOptGrayMatch: public NelderMeadOptBase<float>
{
public:

    float *col0, *col1, *tmp;
    int channels;

    /**
     * @brief NelderMeadOptGrayMatch
     * @param m0
     * @param m1
     * @param inliers
     */
    NelderMeadOptGrayMatch(float *col0, float * col1, int channels) : NelderMeadOptBase()
    {
        this->col0 = new float[channels];
        this->col1 = new float[channels];

        tmp = new float[channels];

        this->channels = channels;

        memcpy(this->col0, col0, sizeof(float) * channels);
        memcpy(this->col1, col1, sizeof(float) * channels);

        Arrayf::normalize(col0, channels);
    }

    ~NelderMeadOptGrayMatch()
    {
        delete_vec_s(col0);
        delete_vec_s(col1);
        delete_vec_s(tmp);
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        Arrayf::mul(x, channels, tmp);
        Arrayf::normalize(tmp, channels);
        return Arrayf::distanceSq(col0, tmp, channels);
    }
};
#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_GRAY_MATCH_HPP
