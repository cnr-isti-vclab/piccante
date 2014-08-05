/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_NELDER_MEAD_OPT_HOMOGRAPHY_HPP
#define PIC_NELDER_MEAD_OPT_HOMOGRAPHY_HPP

#include "util/matrix_3_x_3.hpp"
#include "util/nelder_mead_opt_base.hpp"

namespace pic {

class NelderMeadOptHomography: public NelderMeadOptBase<float>
{
public:
    std::vector< pic::Vec<2, float> > m0, m1;

    /**
     * @brief NelderMeadOptHomography
     * @param m0
     * @param m1
     * @param inliers
     */
    NelderMeadOptHomography(std::vector< pic::Vec<2, float> > &m0,
                            std::vector< pic::Vec<2, float> > &m1,
                            std::vector< unsigned int > inliers) : NelderMeadOptBase()
    {
        for(unsigned int i = 0; i < inliers.size(); i++) {
            this->m0.push_back(m0[inliers[i]]);
            this->m1.push_back(m1[inliers[i]]);
        }
    }

    /**
     * @brief Homography
     * @param H
     * @param p
     * @return
     */
    pic::Vec<2, float> Homography(float *H, pic::Vec<2, float> p)
    {
        pic::Vec<2, float> ret;

        ret[0]  = H[0] * p[0] + H[1] * p[1] + H[2];
        ret[1]  = H[3] * p[0] + H[4] * p[1] + H[5];
        float d = H[6] * p[0] + H[7] * p[1] + 1.0f;

        ret[0] /= d;
        ret[1] /= d;

        return ret;
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

        Matrix3x3 H, H_inv;

        for(unsigned int i = 0; i < 8; i++) {
            H.data[i] = x[i];
        }
        H.data[8] = 1.0f;

        H.Inverse(&H_inv);

        for(unsigned int i = 0; i < m0.size(); i++) {
            pic::Vec<2, float> p0 = m0[i];
            pic::Vec<2, float> p1 = m1[i];

            float dU, dV;

            // | H p0 - p1 | error
            pic::Vec<2, float> p0_H = Homography(H.data, p0);

            dU = p1[0] - p0_H[0];
            dV = p1[1] - p0_H[1];

            err += dU * dU + dV * dV;

            // | H p1 - p0 | error
            pic::Vec<2, float> p1_H = Homography(H_inv.data, p1);

            dU = p0[0] - p1_H[0];
            dV = p0[1] - p1_H[1];

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

}

#endif // PIC_NELDER_MEAD_OPT_HOMOGRAPHY_HPP
