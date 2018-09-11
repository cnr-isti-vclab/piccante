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

#ifndef PIC_COMPUTER_VISION_ESSENTIAL_MATRIX_HPP
#define PIC_COMPUTER_VISION_ESSENTIAL_MATRIX_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../util/math.hpp"
#include "../util/eigen_util.hpp"

#include "../computer_vision/triangulation.hpp"
#include "../computer_vision/camera_matrix.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
    #include "../externals/Eigen/SVD"
    #include "../externals/Eigen/Geometry"
#else
    #include <Eigen/Dense>
    #include <Eigen/SVD>
    #include <Eigen/Geometry>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief computeEssentialMatrix computes the essential matrix, E, from the fundamental
 * matrix, F12, and the two intrics matrices K1 and K2
 * @param F is the fundamental matrix which maps points from camera 1 into camera 2
 * @param K1 is the camera 1 intrics matrix
 * @param K2 is the camera 2 intrics matrix
 * @return
 */
PIC_INLINE Eigen::Matrix3d computeEssentialMatrix(Eigen::Matrix3d &F, Eigen::Matrix3d &K1, Eigen::Matrix3d &K2)
{
    Eigen::Matrix3d K2t = Eigen::Transpose<Eigen::Matrix3d>(K2);
    return K2t * F * K1;
}

/**
 * @brief computeEssentialMatrix computes the essential matrix, E, from the fundamental
 * matrix, F, and a single instrics camera, K.
 * @param F
 * @param K
 * @return
 */
PIC_INLINE Eigen::Matrix3d computeEssentialMatrix(Eigen::Matrix3d &F, Eigen::Matrix3d &K)
{
    return computeEssentialMatrix(F, K, K);
}

/**
 * @brief decomposeEssentialMatrix decomposes an essential matrix E.
 * @param E is the essential matrix. Input.
 * Note1: E = S * R
 * Note2: S = [t]_x
 * Note3: there are four possible cases:
 * 1:     [R1 |  t]
 * 2:     [R1 | -t]
 * 3:     [R2 |  t]
 * 4:     [R2 | -t]
 * @param R1 is one possible rotation matrix. Output.
 * @param R2 is one possible rotation matrix. Output.
 * @param t is the translation vector which is not normalized. Output.
 */
PIC_INLINE void decomposeEssentialMatrix(Eigen::Matrix3d &E, Eigen::Matrix3d &R1, Eigen::Matrix3d &R2, Eigen::Vector3d &t)
{
    //Solving the linear system
    Eigen::JacobiSVD< Eigen::MatrixXd > svd(E, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Matrix3d U = svd.matrixU();
    Eigen::Matrix3d V = svd.matrixV();

    //Z matrix
    Eigen::Matrix3d Z;
    Z.setZero();
    Z(0, 1) =  1.0;
    Z(1, 0) = -1.0;

    //W matrix
    Eigen::Matrix3d W;
    W.setZero();
    W(0, 1) = -1.0;
    W(1, 0) =  1.0;
    W(2, 2) =  1.0;

    //Rotation matrices R1 and R2
    Eigen::Matrix3d Vt = Eigen::Transpose<Eigen::Matrix3d>(V);
    Eigen::Matrix3d Wt = Eigen::Transpose<Eigen::Matrix3d>(W);

    Eigen::Matrix3d  UVt = U * Vt;
    double detUVt = UVt.determinant();

    R1 = detUVt * U * W  * Vt;
    R2 = detUVt * U * Wt * Vt;

    R1 = RotationMatrixRefinement(R1);
    R2 = RotationMatrixRefinement(R2);

    //Translation vector
    Eigen::Matrix3d Ut = Eigen::Transpose<Eigen::Matrix3d>(U);
    Eigen::Matrix3d S = U * Z * Ut;

    t[0] = S(2, 1);
    t[1] = S(0, 2);
    t[2] = S(1, 0);

    t.normalize();
}

/**
 * @brief decomposeEssentialMatrixWithConfiguration decomposes an essential matrix E.
 * @param E is the essential matrix.
 * @param K0
 * @param K1
 * @param points0
 * @param points1
 * @param R
 * @param t
 * @return
 */
PIC_INLINE bool decomposeEssentialMatrixWithConfiguration(Eigen::Matrix3d &E, Eigen::Matrix3d &K0, Eigen::Matrix3d &K1,
                                               std::vector< Eigen::Vector2f > &points0, std::vector< Eigen::Vector2f > &points1,
                                               Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    if(points0.size() != points1.size()) {
        return false;
    }

    Eigen::Matrix3d R0, R1;
    Eigen::Vector3d T;
    decomposeEssentialMatrix(E, R0, R1, T);

    //for each configuration (R0, -T), (R0, T), (R1, -T), (R1, T)
    //the sign of reconstructed points is checked
    int type = -1;
    int counter = -1;

    for(unsigned int j = 0; j < 4; j++) {

        Eigen::Matrix3d tmp_R = (j < 2) ? R0 : R1;
        Eigen::Vector3d tmp_T;

        if((j % 2) == 0) {
            tmp_T = T;
        } else {
            tmp_T = -T;
        }

        Eigen::Matrix34d M0 = getCameraMatrixIdentity(K0);
        Eigen::Matrix34d M1 = getCameraMatrix(K1, tmp_R, tmp_T);

        int tmp_counter = 0;
        for(unsigned int i = 0; i < points0.size(); i++) {
            //homogeneous coordinates
            Eigen::Vector3d point_0 = Eigen::Vector3d(points0[i][0], points0[i][1], 1.0);
            Eigen::Vector3d point_1 = Eigen::Vector3d(points1[i][0], points1[i][1], 1.0);

            Eigen::Vector4d p0 = triangulationHartleySturm(point_0, point_1, M0, M1);
            Eigen::Vector3d p0_euc = Eigen::Vector3d(p0[0], p0[1], p0[2]);
            Eigen::Vector3d p1 = rigidTransform(p0_euc, tmp_R, tmp_T);

            if((p0[2] >= 0.0) && (p1[2] >= 0.0)) {
                tmp_counter++;
            }

        }

        #ifdef PIC_DEBUG
            printf("Front: %d %d\n",tmp_counter,j);
        #endif

        if(tmp_counter > counter) {
            type = j;
            counter = tmp_counter;
        }
    }

    if(type > -1) {

        R = (type < 2) ? R0 : R1;

        if((type % 2) == 0) {
            t = T;
        } else {
            t = -T;
        }

        return true;

    } else {
        R.setZero();
        t.setZero();

        return false;
    }
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_ESSENTIAL_MATRIX_HPP
