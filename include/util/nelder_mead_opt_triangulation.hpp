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

#ifndef PIC_NELDER_MEAD_OPT_TRIANGULATION_HPP
#define PIC_NELDER_MEAD_OPT_TRIANGULATION_HPP

#include "util/matrix_3_x_3.hpp"
#include "util/nelder_mead_opt_base.hpp"
#include "util/computer_vision_functions.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class NelderMeadOptTriangulation: public NelderMeadOptBase<float>
{
public:
    Eigen::MatrixXd M0, M1;
    Eigen::Vector3d p0, p1;

    /**
     * @brief NelderMeadOptTriangulation
     * @param M0
     * @param M1
     */
    NelderMeadOptTriangulation(Eigen::MatrixXd M0, Eigen::MatrixXd M1) : NelderMeadOptBase()
    {
        this->M0 = M0;
        this->M1 = M1;
    }

    /**
     * @brief update
     * @param p0
     * @param p1
     */
    void update(Eigen::Vector3d &p0, Eigen::Vector3d &p1)
    {
        this->p0 = p0;
        this->p1 = p1;
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        Eigen::Vector4d p(x[0], x[1], x[2], 1.0);

        //First view
        Eigen::Vector3d proj = M0 * p;

        proj[0] /= proj[2];
        proj[1] /= proj[2];

        double dx = p0[0] - proj[0];
        double dy = p0[1] - proj[1];

        double err = dx * dx + dy * dy;

        //Second view
        proj = M1 * p;
        proj[0] /= proj[2];
        proj[1] /= proj[2];

        dx = p1[0] - proj[0];
        dy = p1[1] - proj[1];

        err += dx * dx + dy * dy;

        return err;
    }
};

#endif

}

#endif // PIC_NELDER_MEAD_OPT_TRIANGULATION_HPP
