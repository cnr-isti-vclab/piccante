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

#ifndef PIC_COMPUTER_VISION_HOMOGRAPHY_HPP
#define PIC_COMPUTER_VISION_HOMOGRAPHY_HPP

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

#include "../computer_vision/nelder_mead_opt_homography.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief estimateHomography estimates an homography matrix H between image 1 to image 2
 * @param points0 is an array of points computed from image 1.
 * @param points1 is an array of points computed from image 2.
 * @return It returns the homography matrix H.
 */
PIC_INLINE Eigen::Matrix3d estimateHomography(std::vector< Eigen::Vector2f > &points0,
                                   std::vector< Eigen::Vector2f > &points1)
{
    Eigen::Matrix3d  H;

    if((points0.size() != points1.size()) || (points0.size() < 4)) {
        H.setZero();
        return H;
    }

    Eigen::Vector3f transform_0 = ComputeNormalizationTransform(points0);
    Eigen::Vector3f transform_1 = ComputeNormalizationTransform(points1);

    Eigen::Matrix3d mat_0 = getShiftScaleMatrix(transform_0);
    Eigen::Matrix3d mat_1 = getShiftScaleMatrix(transform_1);

    int n = int(points0.size());
    Eigen::MatrixXd A(n * 2, 9);

    //set up the linear system
    for(int i = 0; i < n; i++) {
        //transform coordinates for increasing stability of the system
        Eigen::Vector2f p0 = points0[i];
        Eigen::Vector2f p1 = points1[i];

        p0[0] = (p0[0] - transform_0[0]) / transform_0[2];
        p0[1] = (p0[1] - transform_0[1]) / transform_0[2];

        p1[0] = (p1[0] - transform_1[0]) / transform_1[2];
        p1[1] = (p1[1] - transform_1[1]) / transform_1[2];

        int j = i * 2;
        A(j, 0) = 0.0;
        A(j, 1) = 0.0;
        A(j, 2) = 0.0;
        A(j, 3) = p0[0];
        A(j, 4) = p0[1];
        A(j, 5) = 1.0;
        A(j, 6) = -p1[1] * p0[0];
        A(j, 7) = -p1[1] * p0[1];
        A(j, 8) = -p1[1];

        j++;

        A(j, 0) = p0[0];
        A(j, 1) = p0[1];
        A(j, 2) = 1.0;
        A(j, 3) = 0.0;
        A(j, 4) = 0.0;
        A(j, 5) = 0.0;
        A(j, 6) = -p1[0] * p0[0];
        A(j, 7) = -p1[0] * p0[1];
        A(j, 8) = -p1[0];
    }

    //solve the linear system
    Eigen::JacobiSVD< Eigen::MatrixXd > svd(A, Eigen::ComputeFullV);
    Eigen::MatrixXd V = svd.matrixV();

    n = int(V.cols()) - 1;

    //assign and transpose
    H(0, 0) = V(0, n);
    H(0, 1) = V(1, n);
    H(0, 2) = V(2, n);

    H(1, 0) = V(3, n);
    H(1, 1) = V(4, n);
    H(1, 2) = V(5, n);

    H(2, 0) = V(6, n);
    H(2, 1) = V(7, n);
    H(2, 2) = V(8, n);

    H = mat_1.inverse() * H * mat_0;
    return H / H(2, 2);
}

/**
 * @brief estimateHomographyRansac computes the homography such that: points1 = H * points0
 * @param points0
 * @param points1
 * @param inliers
 * @param maxIterations
 * @return
 */
PIC_INLINE Eigen::Matrix3d estimateHomographyRansac(std::vector< Eigen::Vector2f > &points0,
                                         std::vector< Eigen::Vector2f > &points1,
                                         std::vector< unsigned int > &inliers,
                                         unsigned int maxIterations = 100,
                                         double threshold = 4.0,
                                         unsigned int seed = 1)
{
    if(points0.size() < 5) {
        return estimateHomography(points0, points1);
    }

    Eigen::Matrix3d H;
    int nSubSet = 4;

    std::mt19937 m(seed);

    unsigned int n = int(points0.size());

    unsigned int *subSet = new unsigned int [nSubSet];

    inliers.clear();

    for(unsigned int i = 0; i < maxIterations; i++) {       

        getRandomPermutation(m, subSet, nSubSet, n);

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(int j = 0; j < nSubSet; j++) {
            sub_points0.push_back(points0[subSet[j]]);
            sub_points1.push_back(points1[subSet[j]]);
        }

        Eigen::Matrix3d tmpH = estimateHomography(sub_points0, sub_points1);

        //is it a good one?
        std::vector< unsigned int > tmp_inliers;

        for(unsigned int j = 0; j < n; j++) {
            Eigen::Vector3d point_hom = Eigen::Vector3d(points0[j][0], points0[j][1], 1.0);
            Eigen::Vector3d pp = tmpH * point_hom;
            pp /= pp[2];

            double dx = points1[j][0] - pp[0];
            double dy = points1[j][1] - pp[1];
            double squared_diff = (dx * dx) + (dy * dy);

            if(squared_diff < threshold) {
                tmp_inliers.push_back(j);
            }
        }

        //get the inliers
        if(tmp_inliers.size() > inliers.size()) {
            H = tmpH;
            inliers.clear();
            inliers.assign(tmp_inliers.begin(), tmp_inliers.end());
        }
    }

    //improve estimate with inliers only
    if(inliers.size() > 3) {
        #ifdef PIC_DEBUG
            printf("Better estimate using inliers only.\n");
        #endif

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(unsigned int i = 0; i < inliers.size(); i++) {
            sub_points0.push_back(points0[inliers[i]]);
            sub_points1.push_back(points1[inliers[i]]);
        }

        H = estimateHomography(sub_points0, sub_points1);
    }

    return H;
}
    
/**
* @brief estimateHomographyRansac computes the homography such that: points1 = H * points0
* @param points0
* @param points1
* @param inliers
* @param maxIterations
* @return
*/
PIC_INLINE Eigen::Matrix3d estimateHomographyWithNonLinearRefinement(
                                         std::vector< Eigen::Vector2f > &points0,
                                         std::vector< Eigen::Vector2f > &points1,
                                         std::vector< unsigned int > &inliers,
                                         unsigned int maxIterationsRansac = 10000,
                                         double thresholdRansac = 2.5,
                                         unsigned int seedRansac = 1,
                                         unsigned int maxIterationsNonLinear = 10000,
                                         float thresholdNonLinear = 1e-5f
                                                          ) {
    
    Eigen::Matrix3d H = estimateHomographyRansac(points0, points1, inliers,
                                                 maxIterationsRansac, thresholdRansac,
                                                 seedRansac);

    NelderMeadOptHomography nmoh(points0, points1, inliers);
    float *H_array = getLinearArrayFromMatrix(H);
    nmoh.run(H_array, 8, thresholdNonLinear, maxIterationsNonLinear, H_array);
    H = getMatrix3dFromLinearArray(H_array);
    return H;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_HOMOGRAPHY_HPP
