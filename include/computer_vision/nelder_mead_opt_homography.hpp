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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_HOMOGRAPHY_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_HOMOGRAPHY_HPP

#include "../util/nelder_mead_opt_base.hpp"
#include "../util/std_util.hpp"

#ifndef PIC_DISABLE_EIGEN
#ifndef PIC_EIGEN_NOT_BUNDLED
   #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class NelderMeadOptHomography: public NelderMeadOptBase<float>
{
public:
    std::vector< Eigen::Vector2f > m0, m1;

    /**
     * @brief NelderMeadOptHomography
     * @param m0
     * @param m1
     * @param inliers
     */
    NelderMeadOptHomography(std::vector< Eigen::Vector2f > &m0,
                            std::vector< Eigen::Vector2f > &m1,
                            std::vector< unsigned int > inliers) : NelderMeadOptBase()
    {
        filterInliers(m0, inliers, this->m0);
        filterInliers(m1, inliers, this->m1);
    }

    /**
     * @brief Homography
     * @param H
     * @param p
     * @return
     */
    Eigen::Vector2f Homography(Eigen::Matrix3f &H, Eigen::Vector2f &p)
    {
        Eigen::Vector3f ret = H * Eigen::Vector3f(p[0], p[1], 1.0f);

        return Eigen::Vector2f(ret[0] / ret[2], ret[1] / ret[2]);
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        float err = 0.0f;

        Eigen::Matrix3f H = getMatrixfFromLinearArray(x, 3, 3);
        H(2, 2) = 1.0f;

        Eigen::Matrix3f H_inv = H.inverse();

        for(unsigned int i = 0; i < m0.size(); i++) {
            float dU, dV;

            // | H p0 - p1 | error
            Eigen::Vector2f p0_H = Homography(H, m0[i]);

            dU = m1[i][0] - p0_H[0];
            dV = m1[i][1] - p0_H[1];

            err += dU * dU + dV * dV;

            // | H p1 - p0 | error
            Eigen::Vector2f p1_H = Homography(H_inv, m1[i]);

            dU = m0[i][0] - p1_H[0];
            dV = m0[i][1] - p1_H[1];

            err += dU * dU + dV * dV;

        }

        return err;
    }

    /**
     * @brief run
     * @param x_start
     * @param n
     * @param epsilon
     * @param x
     * @return
     */
    float *run(float *x_start, unsigned int n, float epsilon = 1e-4f, int max_iterations = 1000, float *x = NULL)
    {
        if(n != 8) {
            return x_start;
        }

        if(x == NULL) {
            x = new float[n + 1];
        }

        x = run_aux(x_start, n, epsilon, max_iterations, x);
        x[8] = 1.0f;

        return x;
    }
};

#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_HOMOGRAPHY_HPP
