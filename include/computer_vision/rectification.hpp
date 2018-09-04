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
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../image.hpp"

#include "../image_vec.hpp"

#include "../filtering/filter_warp_2d.hpp"

#include "../computer_vision/camera_matrix.hpp"

#include "../util/eigen_util.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief computeImageRectificationWarp
 * @param img0
 * @param img1
 * @param T0
 * @param T1
 * @param out
 * @return
 */
PIC_INLINE ImageVec *computeImageRectificationWarp(Image *img0,
                                                   Image *img1,
                                                   Eigen::Matrix3d &T0,
                                                   Eigen::Matrix3d &T1,
                                                   ImageVec *out)
{
    if(img0 == NULL || img1 == NULL) {
        return out;
    }

    if(out == NULL) {
        out = new ImageVec();
    }

    FilterWarp2D warp0(MatrixConvert(T0));
    FilterWarp2D warp1(MatrixConvert(T1));

    int bmin0[2], bmin1[2], bmax0[2], bmax1[2];
    warp0.computeBoundingBox(img0->widthf, img0->heightf, bmin0, bmax0);
    warp1.computeBoundingBox(img1->widthf, img1->heightf, bmin1, bmax1);

    bmin0[1] = MIN(bmin0[1], bmin1[1]);
    bmax0[1] = MAX(bmax0[1], bmax1[1]);

    bmin1[1] = bmin0[1];
    bmax1[1] = bmax0[1];

    warp0.SetBoundingBox(bmin0, bmax0);
    warp1.SetBoundingBox(bmin1, bmax1);

    Image *img0_r = NULL;
    Image *img1_r = NULL;

    bool bTest = out->size() == 2;
    if(bTest) {
        img0_r = out->at(0);
        img1_r = out->at(1);
    }

    img0_r = warp0.ProcessP(Single(img0), img0_r);
    img1_r = warp1.ProcessP(Single(img1), img1_r);

    if(!bTest) {
        out->push_back(img0_r);
        out->push_back(img1_r);
    }

    return out;
}

/**
 * @brief computeImageRectification
 * @param img0
 * @param img1
 * @param M0
 * @param M1
 * @return
 */
PIC_INLINE ImageVec *computeImageRectification(Image *img0,
                                               Image *img1,
                                               Eigen::Matrix34d &M0,
                                               Eigen::Matrix34d &M1,
                                               ImageVec *out = NULL)
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

    pic::cameraRectify(M0, M1, M0_r, M1_r, T0, T1);

    out = computeImageRectificationWarp(img0, img1, T0, T1, out);

    return out;
}

/**
 * @brief computeImageRectification
 * @param img0
 * @param img1
 * @param K0
 * @param R0
 * @param t0
 * @param K1
 * @param R1
 * @param t1
 * @param out
 * @return
 */
PIC_INLINE ImageVec *computeImageRectification(Image *img0,
                                               Image *img1,
                                               Eigen::Matrix3d &K0,
                                               Eigen::Matrix3d &R0,
                                               Eigen::Vector3d &t0,
                                               Eigen::Matrix3d &K1,
                                               Eigen::Matrix3d &R1,
                                               Eigen::Vector3d &t1,
                                               ImageVec *out = NULL)
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

    pic::cameraRectify(K0, R0, t0, K1, R1, t1, M0_r, M1_r, T0, T1);

    out = computeImageRectificationWarp(img0, img1, T0, T1, out);

    return out;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_RECTIFICATION_HPP
