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

#ifndef PIC_COMPUTER_VISION_RECTIFICATION_HPP
#define PIC_COMPUTER_VISION_RECTIFICATION_HPP

#include <vector>
#include <cmath>
#include <stdlib.h>

#include "../base.hpp"

#include "../image.hpp"

#include "../image_vec.hpp"

#include "../filtering/filter_warp_2d.hpp"
#include "../filtering/filter_rotation.hpp"

#include "../computer_vision/camera_matrix.hpp"

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
 * @brief computeImageRectificationWarp
 * @param img0 is the first image to rectify
 * @param img1 is the second image to rectify
 * @param T0 is the homography for img0
 * @param T1 is the homography for img0
 * @param out is the output as an ImageVec with two images; i.e., rectified versions of img0 and img1
 * @return
 */
PIC_INLINE ImageVec *computeImageRectificationWarp(Image *img0,
                                                   Image *img1,
                                                   Eigen::Matrix3d &T0,
                                                   Eigen::Matrix3d &T1,
                                                   ImageVec *out,
                                                   bool bPartial = true)
{
    if(img0 == NULL || img1 == NULL) {
        return out;
    }

    if(out == NULL) {
        out = new ImageVec();
    }

    auto H0 = MatrixConvert(T0);
    auto H1 = MatrixConvert(T1);
    FilterWarp2D warp0(H0);
    FilterWarp2D warp1(H1);

    int bmin0[2], bmin1[2], bmax0[2], bmax1[2];

    FilterWarp2D::computeBoundingBox(H0, warp0.getBCentroid(), img0->widthf, img0->heightf, bmin0, bmax0);
    FilterWarp2D::computeBoundingBox(H1, warp1.getBCentroid(), img1->widthf, img1->heightf, bmin1, bmax1);

    if(bPartial) {
        bmin0[1] = MIN(bmin0[1], bmin1[1]);
        bmax0[1] = MAX(bmax0[1], bmax1[1]);

        bmin1[1] = bmin0[1];
        bmax1[1] = bmax0[1];
    } else {
        for(int i = 0; i < 2; i++) {
            bmin0[i] = MIN(bmin0[i], bmin1[i]);
            bmin1[i] = bmin0[i];

            bmax0[i] = MAX(bmax0[i], bmax1[i]);
            bmax1[i] = bmax0[i];
        }
    }

    warp0.setBoundingBox(bmin0, bmax0);
    warp1.setBoundingBox(bmin1, bmax1);

    Image *img0_r = NULL;
    Image *img1_r = NULL;

    bool bTest = out->size() == 2;
    if(bTest) {
        img0_r = out->at(0);
        img1_r = out->at(1);
    }

    img0_r = warp0.Process(Single(img0), img0_r);
    img1_r = warp1.Process(Single(img1), img1_r);

    if(!bTest) {
        out->push_back(img0_r);
        out->push_back(img1_r);
    }

    return out;
}

/**
 * @brief computeImageRectification this function rectifies two images
 * @param img0 is the first image to rectify
 * @param img1 is the second image to rectify
 * @param M0 is the camera matrix (3x4) of img0
 * @param M1 is the camera matrix (3x4) of img1
 * @param out is the output as an ImageVec with two images; i.e., rectified versions of img0 and img1
 * @return is the output as an ImageVec with two images; i.e., rectified versions of img0 and img1

 */
PIC_INLINE ImageVec *computeImageRectification(Image *img0,
                                               Image *img1,
                                               Eigen::Matrix34d &M0,
                                               Eigen::Matrix34d &M1,
                                               ImageVec *out = NULL,
                                               bool bPartial = true)
{
    //NOTE: we should check that img0 and img1 are valid...
    if(img0 == NULL || img1 == NULL) {
        return out;
    }

    if(out == NULL) {
        out = new ImageVec();
    }

    Eigen::Matrix34d M0_r, M1_r;
    Eigen::Matrix3d T0, T1;

    cameraRectify(M0, M1, M0_r, M1_r, T0, T1);

    //check if the trasform is correct!
    Eigen::Vector3d corners[4], corners_T0[4];
    corners[0] = Eigen::Vector3d(0.0, 0.0, 1.0);
    corners[1] = Eigen::Vector3d(img0->widthf, 0.0, 1.0);
    corners[2] = Eigen::Vector3d(img0->widthf, img0->heightf, 1.0);
    corners[3] = Eigen::Vector3d(img0->heightf, 0.0, 1.0);

    for(int i = 0; i < 4; i ++) {
        corners_T0[i] = T0 * corners[i];
        corners_T0[i] /= corners_T0[i][2];
    }

    auto d_c   = corners[2] - corners[0];
    auto d_c_T0 = corners_T0[2] - corners_T0[0];

    bool b_x = std::signbit(d_c[0]) == std::signbit(d_c_T0[0]);
    bool b_y = std::signbit(d_c[1]) == std::signbit(d_c_T0[1]);

    double f_x = b_x ? 1.0 : -1.0;
    double f_y = b_y ? 1.0 : -1.0;


    auto H = DiagonalMatrix(Eigen::Vector3d(f_x, f_y, 1));
    T0 = H * T0;
    T1 = H * T1;

    out = computeImageRectificationWarp(img0, img1, T0, T1, out, bPartial);

    return out;
}

/**
 * @brief computeImageRectification this function rectifies two images
 * @param img0 is the first image to rectify
 * @param img1 is the second image to rectify
 * @param K0 is the intrisic matrix of img0
 * @param R0 is the rotation matrix of img0
 * @param t0 is the translation vector of img0
 * @param K1 is the intrisic matrix of im1
 * @param R1 is the rotation matrix of img1
 * @param t1 is the translation vector of img1
 * @param out is the output as an ImageVec with two images; i.e., rectified versions of img0 and img1
 * @return is the output as an ImageVec with two images; i.e., rectified versions of img0 and img1
 */
PIC_INLINE ImageVec *computeImageRectification(Image *img0,
                                               Image *img1,
                                               Eigen::Matrix3d &K0,
                                               Eigen::Matrix3d &R0,
                                               Eigen::Vector3d &t0,
                                               Eigen::Matrix3d &K1,
                                               Eigen::Matrix3d &R1,
                                               Eigen::Vector3d &t1,
                                               ImageVec *out = NULL,
                                               bool bPartial = true)
{
    //NOTE: we should check that img0 and img1 are valid...
    if(img0 == NULL || img1 == NULL) {
        return out;
    }

    if(out == NULL) {
        out = new ImageVec();
    }

    Eigen::Matrix34d M0_r, M1_r;
    Eigen::Matrix3d T0, T1;

    cameraRectify(K0, R0, t0, K1, R1, t1, M0_r, M1_r, T0, T1);

    out = computeImageRectificationWarp(img0, img1, T0, T1, out, bPartial);

    return out;
}

/**
 * @brief alignPanoramicLL
 * @param R0
 * @param t0
 * @param R1
 * @param t1
 * @param R01
 * @param t01
 */
PIC_INLINE void alignPanoramicLL(Eigen::Matrix3d &R0, Eigen::Vector3d &t0,
                                 Eigen::Matrix3d &R1, Eigen::Vector3d &t1,
                                 Eigen::Matrix3d &R01, Eigen::Vector3d &t01)
{
    t01 = t1 - t0;
    Eigen::Matrix3d R0_t = Eigen::Transpose< Eigen::Matrix3d >(R0);

    //R0 --> I
    //t0 --> 0

    R01 = R0_t * R1;
    t01 = R0_t * t01;
}

/**
 * @brief computeImageRectificationPanoramicLL
 * @param img0
 * @param img1
 * @param R01
 * @param t01
 * @param out
 * @return
 */
PIC_INLINE ImageVec *computeImageRectificationPanoramicLL(
                                               Image *img0,
                                               Image *img1,
                                               Eigen::Matrix3d &R01,
                                               Eigen::Vector3d &t01,
                                               ImageVec *out = NULL)
{
    //NOTE: we should check that img0 and img1 are valid...
    if(img0 == NULL || img1 == NULL) {
        return out;
    }

    if(out == NULL) {
        out = new ImageVec();
    }

    //rotation 1
    Eigen::Matrix3d R01_t = Eigen::Transpose< Eigen::Matrix3d >(R01);

    //rotation 2
    Eigen::Vector3d X(0.0, 1.0, 0.0);
    Eigen::Vector3d n;
    n = t01.cross(X);
    n.normalize();

    double alpha = std::acos(t01.dot(X));
    Eigen::Matrix3d rot, rotation1;

    rot = Eigen::AngleAxisd(alpha, n);
    rotation1 = rot * R01_t;

    Eigen::Matrix3f rotation0f, rotation1f;
    rotation0f = rot.cast<float>();
    rotation1f = rotation1.cast<float>();

    out->push_back(FilterRotation::execute(img0, NULL, rotation0f));
    out->push_back(FilterRotation::execute(img1, NULL, rotation1f));
    return out;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_RECTIFICATION_HPP
