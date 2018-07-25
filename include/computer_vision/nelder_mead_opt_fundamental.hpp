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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_FUNDAMENTAL_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_FUNDAMENTAL_HPP

#include "../util/eigen_util.hpp"
#include "../util/std_util.hpp"
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

class NelderMeadOptFundamental: public NelderMeadOptBase<float>
{
public:
    std::vector< Eigen::Vector2f > m0, m1;

    /**
     * @brief NelderMeadOptFundamental
     * @param m0
     * @param m1
     * @param inliers
     */
    NelderMeadOptFundamental(std::vector< Eigen::Vector2f > &m0,
                             std::vector< Eigen::Vector2f > &m1,
                             std::vector< unsigned int> inliers) : NelderMeadOptBase()
    {
        filterInliers(m0, inliers, this->m0);
        filterInliers(m1, inliers, this->m1);
    }

    /**
     * @brief Fundamental
     * @param F
     * @param p
     * @return
     */
    double FundamentalDistance(Eigen::Matrix3d &F, Eigen::Vector3d &p0, Eigen::Vector3d  &p1)
    {        
        Eigen::Vector3d F_p0 = F * p0;

        double norm = F_p0[0] * F_p0[0] + F_p0[1] * F_p0[1];
        if(norm > 0.0) {
            norm = sqrt(norm);
            F_p0 /= norm;
        }

        //computing distance

        return F_p0.dot(p1);
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        Eigen::Matrix3d F = getMatrixdFromLinearArray(x, 3, 3);
        Eigen::Matrix3d F_t = Eigen::Transpose<Eigen::Matrix3d>(F);

        double err = 0.0;
        for(unsigned int i = 0; i < m0.size(); i++) {

            Eigen::Vector3d p0 = Eigen::Vector3d(m0[i][0], m0[i][1], 1.0);
            Eigen::Vector3d p1 = Eigen::Vector3d(m1[i][0], m1[i][1], 1.0);

            double tmp_err;

            // | p1^t F p0 | error           
            tmp_err = FundamentalDistance(F, p0, p1);
            err += tmp_err * tmp_err;

            // | p0^t F^t p1 | error
            tmp_err = FundamentalDistance(F_t, p1, p0);
            err += tmp_err * tmp_err;
        }

        return float(err);
    }
};
#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_FUNDAMENTAL_HPP
