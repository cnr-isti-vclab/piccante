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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_TRIANGULATION_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_TRIANGULATION_HPP

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

class NelderMeadOptTriangulation: public NelderMeadOptBase<double>
{
public:

    std::vector< Eigen::Matrix34d > M;
    std::vector< Eigen::Vector2f > p;

    /**
     * @brief NelderMeadOptTriangulation
     * @param M0
     * @param M1
     */
    NelderMeadOptTriangulation(Eigen::Matrix34d &M0, Eigen::Matrix34d &M1) : NelderMeadOptBase()
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
    double function(double *x, unsigned int n)
    {
        Eigen::Vector4d point(x[0], x[1], x[2], 1.0);

        double err = 0.0;
        for(unsigned int i = 0; i < M.size(); i++) {
            Eigen::Vector3d proj = M[i] * point;

            proj[0] /= proj[2];
            proj[1] /= proj[2];

            double dx = p[i][0] - proj[0];
            double dy = p[i][1] - proj[1];

            err += (dx * dx) + (dy * dy);
        }

        return err;
    }
};

#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_TRIANGULATION_HPP
