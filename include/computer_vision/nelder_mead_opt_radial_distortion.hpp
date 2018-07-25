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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_RADIAL_DISTORTION_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_RADIAL_DISTORTION_HPP

#include "../util/matrix_3_x_3.hpp"
#include "../util/nelder_mead_opt_base.hpp"

#ifndef PIC_DISABLE_EIGEN
#ifndef PIC_EIGEN_NOT_BUNDLED
   #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class NelderMeadOptRadialDistortion: public NelderMeadOptBase<float>
{
public:

    std::vector< Eigen::Matrix34d > M;
    std::vector< Eigen::Vector3d >  *p3d;
    std::vector< std::vector< Eigen::Vector2f > * > p2d;

    /**
     * @brief NelderMeadOptRadialDistortion
     * @param M0
     * @param M1
     */
    NelderMeadOptRadialDistortion(Eigen::Matrix34d &M0, Eigen::Matrix34d &M1,
                               std::vector< Eigen::Vector2f > *p2d_0,
                               std::vector< Eigen::Vector2f > *p2d_1,
                               std::vector< Eigen::Vector3d > *p3d) : NelderMeadOptBase()
    {
        this->M.push_back(M0);
        this->M.push_back(M1);

        this->p2d.push_back(p2d_0);
        this->p2d.push_back(p2d_1);
        this->p3d = p3d;
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        float lambda = x[0];

        double err = 0.0;

        double cx = M[0](0,2);
        double cy = M[0](1,2);
        double fx = M[0](0, 0);
        double fy = M[0](1, 1);

        for(unsigned int i = 0; i < M.size(); i++) {
            for(unsigned int j = 0; j < p3d->size(); j++) {
                Eigen::Vector3d tmp = p3d->at(j);
                Eigen::Vector4d point = Eigen::Vector4d(tmp[0], tmp[1], tmp[2], 1.0);
                Eigen::Vector3d proj = M[i] * point;

                proj[0] /= proj[2];
                proj[1] /= proj[2];

                double x_cx =  (proj[0] - cx);
                double y_cy =  (proj[1] - cy);

                double dx = x_cx / fx;
                double dy = y_cy / fy;
                double rho_sq = dx * dx + dy * dy;


                double factor = 1.0 / (1.0 + rho_sq * lambda);

                proj[0] = x_cx * factor + cx;
                proj[1] = y_cy * factor + cy;

                Eigen::Vector2f tmp2d = p2d[i]->at(j);
                double d_err_x = tmp2d[0] - proj[0];
                double d_err_y = tmp2d[1] - proj[1];

                err += d_err_x * d_err_x + d_err_y * d_err_y;
            }
        }

        err /= p3d->size();

        //err += 10.0f * lambda * lambda;

        return float(err);
    }
};

#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_RADIAL_DISTORTION_HPP
