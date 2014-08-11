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

    std::vector< Eigen::Matrix34d > M;
    std::vector< Eigen::Vector2f > p;

    /**
     * @brief NelderMeadOptTriangulation
     * @param M0
     * @param M1
     */
    NelderMeadOptTriangulation(Eigen::Matrix34d M0, Eigen::Matrix34d M1) : NelderMeadOptBase()
    {
        this->M.push_back(M0);
        this->M.push_back(M1);
    }

    /**
     * @brief NelderMeadOptTriangulation
     * @param M0
     * @param M1
     */
    NelderMeadOptTriangulation(std::vector< Eigen::Matrix34d> &M) : NelderMeadOptBase()
    {
        this->M.assign(M.begin(), M.end());
    }

    /**
     * @brief update
     * @param p0
     * @param p1
     */
    void update(Eigen::Vector2f &p0, Eigen::Vector2f &p1)
    {
        this->p.clear();
        this->p.push_back(p0);
        this->p.push_back(p1);
    }

    /**
     * @brief update
     * @param p0
     * @param p1
     */
    void update(std::vector< Eigen::Vector2f> &p)
    {
        this->p.clear();
        this->p.assign(p.begin(), p.end());
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {
        Eigen::Vector4d point(x[0], x[1], x[2], 1.0);

        double err = 0.0;
        for(unsigned int i = 0; i < M.size(); i++) {
            Eigen::Vector3d proj = M[i] * point;

            proj[0] /= proj[2];
            proj[1] /= proj[2];

            double dx = p[i][0] - proj[0];
            double dy = p[i][1] - proj[1];

            err += dx * dx + dy * dy;
        }

        return float(err);
    }
};

#endif

}

#endif // PIC_NELDER_MEAD_OPT_TRIANGULATION_HPP
