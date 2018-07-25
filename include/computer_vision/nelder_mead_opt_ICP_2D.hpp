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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_ICP_2D_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_ICP_2D_HPP

#include "../util/eigen_util.hpp"
#include "../util/std_util.hpp"
#include "../util/matrix_3_x_3.hpp"
#include "../util/nelder_mead_opt_base.hpp"

#include "../computer_vision/iterative_closest_point_2D.hpp"

#ifndef PIC_DISABLE_EIGEN
#ifndef PIC_EIGEN_NOT_BUNDLED
   #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class NelderMeadOptICP2D: public NelderMeadOptBase<float>
{
public:
    std::vector< Eigen::Vector2f > points_pattern, points;

    /**
     * @brief NelderMeadOptICP2D
     * @param points_pattern
     * @param points
     */
    NelderMeadOptICP2D(std::vector< Eigen::Vector2f > &points_pattern,
                       std::vector< Eigen::Vector2f > &points) : NelderMeadOptBase()
    {
        std::copy(points_pattern.begin(), points_pattern.end(),
                  std::back_inserter(this->points_pattern));

        std::copy(points.begin(), points.end(),
                  std::back_inserter(this->points));
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        float scale = 1.0f;
        if(n == 4) {
            if(x[3] < 1.0f) {
                return FLT_MAX;
            }

            scale = x[3];
        }

        ICP2DTransform t(x[0], x[1], x[2], scale);

        std::vector< Eigen::Vector2f > out;
        t.applyC(points_pattern, out);

        return getErrorPointsList(out, points);
    }
};
#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_ICP_2D_HPP
