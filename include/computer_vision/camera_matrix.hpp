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

#ifndef PIC_COMPUTER_VISION_CAMERA_MARTIX_HPP
#define PIC_COMPUTER_VISION_CAMERA_MARTIX_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "util/math.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#include "externals/Eigen/SVD"
#include "externals/Eigen/Geometry"

#include "util/eigen_util.hpp"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief computeEpipole computes the epipole of a fundamental matrix F.
 * @param F is a fundamental matrix.
 * @return It returns the epipole of F.
 */
Eigen::Vector3d computeEpipole(Eigen::Matrix3d &F)
{
    Eigen::JacobiSVD< Eigen::Matrix3d > svdF(F, Eigen::ComputeFullV);
    Eigen::Matrix3d V = svdF.matrixV();

    Eigen::Vector3d e;

    e[0] = V(0, 2);
    e[1] = V(1, 2);
    e[2] = V(2, 2);

    return e;
}

/**
 * @brief getCameraMatrixFromHomography
 * @param H is 3x3 homography matrix.
 * @param K
 * @return
 */
Eigen::Matrix34d getCameraMatrixFromHomography(Eigen::Matrix3d &H, Eigen::Matrix3d &K)
{
    Eigen::Matrix34d m;
    m.setZero();

    Eigen::Matrix3d K_inv = K.inverse();

    Eigen::Matrix3d H_p = K_inv * H;

    Eigen::Vector3d r_0(H_p(0, 0), H_p(1, 0), H_p(2, 0));
    Eigen::Vector3d r_1(H_p(0, 1), H_p(1, 1), H_p(2, 1));

    r_0.normalize();
    r_1.normalize();
    Eigen::Vector3d r_2 = r_0.cross(r_1);

    Eigen::Vector3d t(H_p(0, 2), H_p(1, 2), H_p(2, 2));

    m(0, 0) = r_0[0];
    m(1, 0) = r_0[1];
    m(2, 0) = r_0[2];

    m(0, 1) = r_1[0];
    m(1, 1) = r_1[1];
    m(2, 1) = r_1[2];

    m(0, 2) = r_2[0];
    m(1, 2) = r_2[1];
    m(2, 2) = r_2[2];

    m(0 , 3) = t[0];
    m(1 , 3) = t[1];
    m(2 , 3) = t[2];

    return K * m;
}

/**
 * @brief RectifyCameras
 * @param K0
 * @param R0
 * @param t0
 * @param K1
 * @param R1
 * @param t1
 * @param T0
 * @param T1
 */
void RectifyCameras(Eigen::Matrix3d &K0, Eigen::Matrix3d &R0, Eigen::Vector3d &t0,
                    Eigen::Matrix3d &K1, Eigen::Matrix3d &R1, Eigen::Vector3d &t1,
                    Eigen::Matrix3d &T0, Eigen::Matrix3d &T1)
{
    Eigen::Matrix3d K0_inv = K0.inverse();
    Eigen::Matrix3d K1_inv = K1.inverse();

    Eigen::Matrix3d R0_t = Eigen::Transpose<Eigen::Matrix3d>(R0);
    Eigen::Matrix3d R1_t = Eigen::Transpose<Eigen::Matrix3d>(R1);

    Eigen::Vector3d c0 = -R0_t * K0_inv * t0;
    Eigen::Vector3d c1 = -R1_t * K1_inv * t1;

    Eigen::Vector3d v0 = c1 - c0;

    Eigen::Vector3d R0_t_2 = Eigen::Vector3d(R0_t(2, 0), R0_t(2, 1), R0_t(2, 2));
    Eigen::Vector3d v1 = R0_t_2.cross(v0);
    Eigen::Vector3d v2 = v0.cross(v1);

}

/**
 * @brief getCameraMatrixIdentity
 * @param K
 * @return
 */
Eigen::Matrix34d getCameraMatrixIdentity(Eigen::Matrix3d &K)
{
    Eigen::Matrix34d m;
    m.setZero();

    m(0, 0) = 1.0;
    m(1, 1) = 1.0;
    m(2, 2) = 1.0;

    return K * m;
}

/**
 * @brief getCameraMatrix
 * @param K
 * @param R
 * @param t
 * @return
 */
Eigen::Matrix34d getCameraMatrix(Eigen::Matrix3d &K, Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    Eigen::Matrix34d m;

    m(0, 0) = R(0, 0);
    m(1, 0) = R(1, 0);
    m(2, 0) = R(2, 0);

    m(0, 1) = R(0, 1);
    m(1, 1) = R(1, 1);
    m(2, 1) = R(2, 1);

    m(0, 2) = R(0, 2);
    m(1, 2) = R(1, 2);
    m(2, 2) = R(2, 2);

    m(0, 3) = t[0];
    m(1, 3) = t[1];
    m(2, 3) = t[2];

    return K * m;
}

/**
 * @brief cameraMatrixProject projects a point, p, using the camera
 * matrix, M.
 * @param M
 * @param p is a 3D point encoded in homogenous coordinate (4D vector)
 * @return
 */
Eigen::Vector2i cameraMatrixProject(Eigen::Matrix34d &M, Eigen::Vector4d &p)
{
    Eigen::Vector3d proj = M * p;
    proj[0] /= proj[2];
    proj[1] /= proj[2];

    return Eigen::Vector2i(int(proj[0]), int(proj[1]));
}

/**
 * @brief cameraMatrixProject projects a point, p, using the camera
 * matrix, M.
 * @param M
 * @param p is a 3D point (3D vector)
 * @return
 */
Eigen::Vector2i cameraMatrixProject(Eigen::Matrix34d &M, Eigen::Vector3d &p)
{
    Eigen::Vector4d p4d(p[0], p[1], p[2], 1.0);
    return cameraMatrixProject(M, p4d);
}

/**
 * @brief cameraMatrixProjection
 * @param M
 * @param p
 * @param cx
 * @param cy
 * @param fx
 * @param fy
 * @param lambda
 * @return
 */
Eigen::Vector2i cameraMatrixProjection(Eigen::Matrix34d &M, Eigen::Vector3d &p, double cx, double cy, double fx, double fy, double lambda)
{
    Eigen::Vector4d p_t = Eigen::Vector4d(p[0], p[1], p[2], 1.0);
    Eigen::Vector2i out;
    Eigen::Vector3d proj = M * p_t;
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

    out[0] = int(proj[0]);
    out[1] = int(proj[1]);

    return out;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_CAMERA_MARTIX_HPP
