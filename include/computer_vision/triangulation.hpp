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

#ifndef PIC_COMPUTER_VISION_TRIANGULATION_HPP
#define PIC_COMPUTER_VISION_TRIANGULATION_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../util/math.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "../externals/Eigen/Dense"
#include "../externals/Eigen/SVD"
#include "../externals/Eigen/Geometry"
#include "../util/eigen_util.hpp"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief triangulationLonguetHiggins computes triangulation using Longuet-Higgins equations.
 * @param point_0 is the point from the first view that matches point_1
 * @param point_1 is the point from the second view that matches point_0
 * @param R is the rotation matrix between the two views.
 * @param t is the translation matrix between the two views.
 * @return
 */
PIC_INLINE Eigen::Vector3d triangulationLonguetHiggins(Eigen::Vector3d &point_0, Eigen::Vector3d &point_1, Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    Eigen::Vector3d ret;

    Eigen::Vector3d r_0 = Eigen::Vector3d(R(0, 0), R(0, 1), R(0, 2));
    Eigen::Vector3d r_2 = Eigen::Vector3d(R(2, 0), R(2, 1), R(2, 2));

    Eigen::Vector3d tmp = r_0 - point_1[0] * r_2;

    ret[2] = tmp.dot(t) / tmp.dot(point_0);

    ret[0] = point_0[0] * ret[2];
    ret[1] = point_0[1] * ret[2];

    return ret;
}

/**
 * @brief triangulationHartl
 * Sturm
 * @param point_0
 * @param point_1
 * @param R
 * @param t
 * @return
 */
PIC_INLINE Eigen::Vector4d triangulationHartleySturm(Eigen::Vector3d &point_0, Eigen::Vector3d &point_1,
                                          Eigen::Matrix34d &M0, Eigen::Matrix34d &M1, int maxIter = 100)
{
    Eigen::Vector4d M0_row[3], M1_row[3];

    for(int i = 0; i < 3; i++) {
        M0_row[i] = Eigen::Vector4d(M0(i, 0), M0(i, 1), M0(i, 2), M0(i, 3));
        M1_row[i] = Eigen::Vector4d(M1(i, 0), M1(i, 1), M1(i, 2), M1(i, 3));
    }

    Eigen::Vector4d x;
    double weight0 = 1.0;
    double weight0_prev = 1.0;

    double weight1 = 1.0;
    double weight1_prev = 1.0;

    int j = 0;
    while(j < maxIter) {
        Eigen::Vector4d A0 = (M0_row[0] - point_0[0] * M0_row[2]) / weight0;
        Eigen::Vector4d A1 = (M0_row[1] - point_0[1] * M0_row[2]) / weight0;

        Eigen::Vector4d A2 = (M1_row[0] - point_1[0] * M1_row[2]) / weight1;
        Eigen::Vector4d A3 = (M1_row[1] - point_1[1] * M1_row[2]) / weight1;

        Eigen::MatrixXd A(4, 4);
        for(int i = 0; i < 4; i++) {
            A(0, i) = A0[i];
            A(1, i) = A1[i];
            A(2, i) = A2[i];
            A(3, i) = A3[i];
        }

        Eigen::JacobiSVD< Eigen::MatrixXd > svdA(A, Eigen::ComputeFullV);
        Eigen::MatrixXd V = svdA.matrixV();
        int n = V.cols() - 1;

        x[0] = V(0, n);
        x[1] = V(1, n);
        x[2] = V(2, n);
        x[3] = V(3, n);
        x /= x[3];

        weight0_prev = weight0;
        weight1_prev = weight1;

        weight0 = x.dot(M0_row[2]);
        weight1 = x.dot(M1_row[2]);

        double d0 = weight0_prev - weight0;
        double d1 = weight1_prev - weight1;
        double err = sqrt(d0 * d0 + d1 * d1);

        if(err < 1e-12){
            break;
        }

        j++;
    }

    #ifdef PIC_DEBUG
        printf("triangulationHartleySturm's Iterations: %d\n",j);
    #endif

    return x;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_TRIANGULATION_HPP
