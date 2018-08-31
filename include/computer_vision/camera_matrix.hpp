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

#include "../base.hpp"

#include "../util/math.hpp"
#include "../util/eigen_util.hpp"

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
 * @brief computeEpipole computes the epipole of a fundamental matrix F.
 * @param F is a fundamental matrix.
 * @return It returns the epipole of F.
 */
PIC_INLINE Eigen::Vector3d computeEpipole(Eigen::Matrix3d &F)
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
PIC_INLINE Eigen::Matrix34d getCameraMatrixFromHomography(Eigen::Matrix3d &H, Eigen::Matrix3d &K)
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
 * @brief getCameraMatrixIdentity
 * @param K
 * @return
 */
PIC_INLINE Eigen::Matrix34d getCameraMatrixIdentity(Eigen::Matrix3d &K)
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
PIC_INLINE Eigen::Matrix34d getCameraMatrix(Eigen::Matrix3d &K, Eigen::Matrix3d &R, Eigen::Vector3d &t)
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
PIC_INLINE Eigen::Vector2i cameraMatrixProject(Eigen::Matrix34d &M, Eigen::Vector4d &p)
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
PIC_INLINE Eigen::Vector2i cameraMatrixProject(Eigen::Matrix34d &M, Eigen::Vector3d &p)
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
PIC_INLINE Eigen::Vector2i cameraMatrixProjection(Eigen::Matrix34d &M, Eigen::Vector3d &p, double cx, double cy, double fx, double fy, double lambda)
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

/**
 * @brief getCameraCenter
 * @param P the camera matrix of a view
 * @return it returns the camera center of P
 */
Eigen::Vector3d getCameraCenter(Eigen::Matrix34d &P)
{
    Eigen::Matrix3d Q = P.block<3, 3>(0, 0);
    auto Q_inv = Q.inverse();
    return - Q_inv * P.col(3);
}

/**
 * @brief cameraRectify
 * @param K0 intrisic matrix of view0
 * @param R0 rotation matrix of view0
 * @param t0 translation vector of view0
 * @param K1 intrisic matrix of view1
 * @param R1 rotation matrix of view1
 * @param t1 translation vector of view1
 * @param P0 new camera matrix of view0
 * @param P1 new camera matrix of view1
 * @param T0 transformation matrix for view0
 * @param T1 transformation matrix for view1
 */
PIC_INLINE void cameraRectify(Eigen::Matrix3d &K0, Eigen::Matrix3d &R0, Eigen::Vector3d &t0,
                              Eigen::Matrix3d &K1, Eigen::Matrix3d &R1, Eigen::Vector3d &t1,
                              Eigen::Matrix34d &P0, Eigen::Matrix34d &P1,
                              Eigen::Matrix3d &T0, Eigen::Matrix3d &T1)
{
    auto P0t = getCameraMatrix(K0, R0, t0);
    auto P1t = getCameraMatrix(K1, R1, t1);

    auto K0_i = K0.inverse();
    auto K1_i = K1.inverse();

    auto R0_t = Eigen::Transpose< Eigen::Matrix3d >(R0);
    auto R1_t = Eigen::Transpose< Eigen::Matrix3d >(R1);

    //compute optical centers

    auto c0 = getCameraCenter(P0t);
    auto c1 = getCameraCenter(P0t);

    //compute new rotation matrix
    Eigen::Vector3d x_axis = c1 - c0;
    Eigen::Vector3d tmp = R1.row(2);
    Eigen::Vector3d y_axis = tmp.cross(x_axis);
    Eigen::Vector3d z_axis = x_axis.cross(y_axis);

    x_axis.normalize();
    y_axis.normalize();
    z_axis.normalize();

    Eigen::Matrix3d R;

    R(0, 0) = x_axis[0];    R(1, 0) = x_axis[1];    R(2, 0) = x_axis[2];
    R(1, 1) = y_axis[0];    R(1, 1) = y_axis[1];    R(2, 1) = y_axis[2];
    R(0, 2) = z_axis[0];    R(1, 2) = z_axis[1];    R(2, 2) = z_axis[2];

    //new camera matrices
    Eigen::Vector3d t0n = -R * c0;
    P0 = getCameraMatrix(K0, R, t0n);

    Eigen::Vector3d t1n = -R * c1;
    P1 = getCameraMatrix(K1, R, t1n);

    //transformations
    auto o0 = P0t.block<3, 3>(0, 0);
    auto old0 = o0.inverse();

    auto new0 = P0.block<3, 3>(0, 0);
    T0 = new0 * old0;


    auto o1 = P1t.block<3, 3>(0, 0);
    auto old1 = o1.inverse();

    auto new1 = P1.block<3, 3>(0, 0);
    T1 = new1 * old1;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_CAMERA_MARTIX_HPP
