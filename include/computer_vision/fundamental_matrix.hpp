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

#ifndef PIC_COMPUTER_VISION_FUNDAMENTAL_HPP
#define PIC_COMPUTER_VISION_FUNDAMENTAL_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../image.hpp"

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gaussian_2d.hpp"

#include "../util/math.hpp"
#include "../util/eigen_util.hpp"

#include "../features_matching/orb_descriptor.hpp"
#include "../features_matching/feature_matcher.hpp"
#include "../features_matching/binary_feature_lsh_matcher.hpp"
#include "../computer_vision/nelder_mead_opt_fundamental.hpp"

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
 * @brief estimateFundamental estimates the foundamental matrix between image 1 to image 2
 * @param points0 is an array of points computed from image 1.
 * @param points1 is an array of points computed from image 2.
 * @return It returns the fundamental matrix, F_{1,2}.
 */
PIC_INLINE Eigen::Matrix3d estimateFundamental(std::vector< Eigen::Vector2f > &points0,
                                    std::vector< Eigen::Vector2f > &points1)
{
    Eigen::Matrix3d F;

    if((points0.size() != points1.size()) || (points0.size() < 8)) {
        F.setZero();
        return F;
    }

    //shift and scale points for numerical stability
    Eigen::Vector3f transform_0 = ComputeNormalizationTransform(points0);
    Eigen::Vector3f transform_1 = ComputeNormalizationTransform(points1);

    Eigen::Matrix3d mat_0 = getShiftScaleMatrix(transform_0);
    Eigen::Matrix3d mat_1 = getShiftScaleMatrix(transform_1);

    Eigen::MatrixXd A(points0.size(), 9);

    //set up the linear system
    for(unsigned int i = 0; i < points0.size(); i++) {

        //transform coordinates for increasing stability of the system
        Eigen::Vector2f p0 = points0[i];
        Eigen::Vector2f p1 = points1[i];

        p0[0] = (p0[0] - transform_0[0]) / transform_0[2];
        p0[1] = (p0[1] - transform_0[1]) / transform_0[2];

        p1[0] = (p1[0] - transform_1[0]) / transform_1[2];
        p1[1] = (p1[1] - transform_1[1]) / transform_1[2];

        A(i, 0) = p0[0] * p1[0];
        A(i, 1) = p0[0] * p1[1];
        A(i, 2) = p0[0];
        A(i, 3) = p0[1] * p1[0];
        A(i, 4) = p0[1] * p1[1];
        A(i, 5) = p0[1];
        A(i, 6) = p1[0];
        A(i, 7) = p1[1];
        A(i, 8) = 1.0;
    }

    //solve the linear system
    Eigen::JacobiSVD< Eigen::MatrixXd > svd(A, Eigen::ComputeFullV);
    Eigen::MatrixXd V = svd.matrixV();

    int n = int(V.cols()) - 1;

    F(0, 0) = V(0, n);
    F(1, 0) = V(1, n);
    F(2, 0) = V(2, n);

    F(0, 1) = V(3, n);
    F(1, 1) = V(4, n);
    F(2, 1) = V(5, n);

    F(0, 2) = V(6, n);
    F(1, 2) = V(7, n);
    F(2, 2) = V(8, n);

    //compute the final F matrix
    Eigen::Matrix3d mat_1_t = Eigen::Transpose< Eigen::Matrix3d >(mat_1);
    F = mat_1_t * F * mat_0;

    //enforce singularity
    Eigen::JacobiSVD< Eigen::MatrixXd > svdF(F, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Matrix3d Uf = svdF.matrixU();
    Eigen::Matrix3d Vf = svdF.matrixV();
    Eigen::Vector3d Df = svdF.singularValues();
    Df[2] = 0.0;

    Eigen::Matrix3d F_new = Uf * DiagonalMatrix(Df) * Eigen::Transpose< Eigen::Matrix3d >(Vf);

    double norm = MAX(Df[0], Df[1]);
    return F_new / norm;
}

/**
 * @brief estimateFundamentalRansac
 * @param points0
 * @param points1
 * @param inliers
 * @param maxIterations
 * @return
 */
PIC_INLINE Eigen::Matrix3d estimateFundamentalRansac(std::vector< Eigen::Vector2f > &points0,
                                          std::vector< Eigen::Vector2f > &points1,
                                          std::vector< unsigned int > &inliers,
                                          unsigned int maxIterations = 100,
                                          double threshold = 0.01,
                                          unsigned int seed = 1)
{
    if(points0.size() < 9) {
        return estimateFundamental(points0, points1);
    }

    Eigen::Matrix3d F;
    int nSubSet = 8;

    std::mt19937 m(seed);

    unsigned int n = int(points0.size());

    unsigned int *subSet = new unsigned int [nSubSet];

    inliers.clear();

    for(unsigned int i = 0; i < maxIterations; i++) {
        getRandomPermutation(m, subSet, nSubSet, n);

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(int j = 0; j < nSubSet; j++) {
            unsigned int k = subSet[j];
            sub_points0.push_back(points0[k]);
            sub_points1.push_back(points1[k]);
        }

        Eigen::Matrix3d tmpF = estimateFundamental(sub_points0, sub_points1);

        //is it a good one?
        std::vector< unsigned int > tmp_inliers;

        for(unsigned int j = 0; j < n; j++) {
            Eigen::Vector3d p0 = Eigen::Vector3d(points0[j][0], points0[j][1], 1.0);
            Eigen::Vector3d p1 = Eigen::Vector3d(points1[j][0], points1[j][1], 1.0);

            Eigen::Vector3d tmpF_p0 = tmpF * p0;
            double n0 = sqrt(tmpF_p0[0] * tmpF_p0[0] + tmpF_p0[1] * tmpF_p0[1]);
            if(n0 >  0.0) {
                tmpF_p0 /= n0;
            }

            double err = fabs(tmpF_p0.dot(p1));

            if(err < threshold){
                tmp_inliers.push_back(j);
            }
        }

        //get the inliers
        if(tmp_inliers.size() > inliers.size()) {
            F = tmpF;
            inliers.clear();
            inliers.assign(tmp_inliers.begin(), tmp_inliers.end());
        }
    }

    //improve estimate with inliers only
    if(inliers.size() > 7) {

        #ifdef PIC_DEBUG
            printf("Better estimate using inliers only.\n");
        #endif

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(unsigned int i = 0; i < inliers.size(); i++) {
            sub_points0.push_back(points0[inliers[i]]);
            sub_points1.push_back(points1[inliers[i]]);
        }

        F = estimateFundamental(sub_points0, sub_points1);
    }

    return F;
}
    
/**
 * @brief estimateFundamentalWithNonLinearRefinement
 * @param F
 * @return
 */
PIC_INLINE Eigen::Matrix3d estimateFundamentalWithNonLinearRefinement(std::vector< Eigen::Vector2f > &points0,
                                                           std::vector< Eigen::Vector2f > &points1,
                                                           std::vector< unsigned int >    &inliers,
                                                           unsigned int maxIterationsRansac = 100,
                                                           double thresholdRansac = 0.01,
                                                           unsigned int seed = 1,
                                                           unsigned int maxIterationsNonLinear = 10000,
                                                           float thresholdNonLinear = 1e-4f
                                                           )
{
    Eigen::Matrix3d F = estimateFundamentalRansac(points0, points1, inliers, maxIterationsRansac, thresholdRansac, seed);

    //non-linear refinement using Nelder-Mead
    NelderMeadOptFundamental nmf(points0, points1, inliers);
        
    float F_data_opt[9];
    nmf.run(getLinearArrayFromMatrix(F), 9, thresholdNonLinear, maxIterationsNonLinear, &F_data_opt[0]);
    F = getMatrixdFromLinearArray(F_data_opt, 3, 3);

    return F;
}

/**
 * @brief noramalizeFundamentalMatrix
 * @param F
 * @return
 */
PIC_INLINE Eigen::Matrix3d noramalizeFundamentalMatrix(Eigen::Matrix3d F)
{
    Eigen::JacobiSVD< Eigen::Matrix3d > svdF(F, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Matrix3d Uf = svdF.matrixU();
    Eigen::Matrix3d Vf = svdF.matrixV();
    Eigen::Vector3d Df = svdF.singularValues();
    Df[2] = 0.0;

    Eigen::Matrix3d F_new = Uf * DiagonalMatrix(Df) * Eigen::Transpose< Eigen::Matrix3d >(Vf);

    double norm = MAX(Df[0], Df[1]);
    return F_new / norm;
}

/**
 * @brief extractFundamentalMatrix
 * @param M0
 * @param M1
 * @param e0
 * @param e1
 * @return
 */
PIC_INLINE Eigen::Matrix3d extractFundamentalMatrix(Eigen::Matrix34d &M0, Eigen::Matrix34d &M1, Eigen::VectorXd &e0, Eigen::VectorXd &e1) {

    Eigen::Matrix3d M0_3 = getSquareMatrix(M0);
    Eigen::Matrix3d M1_3 = getSquareMatrix(M1);


    Eigen::Matrix3d M0_inv = M0_3.inverse();
    Eigen::Vector3d c0 = - M0_inv * getLastColumn(M0);
    e1 = M1 * addOne(c0);

    Eigen::Matrix3d M1_inv = M1_3.inverse();
    Eigen::Vector3d c1 = - M1_inv * getLastColumn(M1);
    e0 = M0 * addOne(c1);

    Eigen::Matrix3d F;

    F(0, 0) =  0.0;
    F(0, 1) = -e1(2);
    F(0, 2) =  e1(1);

    F(1, 0) =  e1(2);
    F(1, 1) =  0.0;
    F(1, 2) = -e1(0);

    F(2, 0) = -e1(1);
    F(2, 1) =  e1(0);
    F(2, 2) =  0.0;

    F = F * M1_3 * M0_inv;

    Eigen::JacobiSVD< Eigen::Matrix3d > svdF(F, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Vector3d Df = svdF.singularValues();

    double norm = MAX(Df[0], MAX(Df[1], Df[2]));
    return F / norm;
}

/**
 * @brief estimateFundamentalFromImages
 * @param img0
 * @param img1
 * @return
 */
PIC_INLINE  Eigen::Matrix3d estimateFundamentalFromImages(Image *img0,
                                                          Image *img1,
                                                          std::vector< Eigen::Vector2f > &m0,
                                                          std::vector< Eigen::Vector2f > &m1,
                                                          std::vector< unsigned int > &inliers)
{
    Eigen::Matrix3d F;
    if(img0 == NULL || img1 == NULL) {
        return F;
    }

    m0.clear();
    m1.clear();
    inliers.clear();

    //corners
    std::vector< Eigen::Vector2f > corners_from_img0;
    std::vector< Eigen::Vector2f > corners_from_img1;

    //compute the luminance images
    Image *L0 = FilterLuminance::execute(img0, NULL, LT_CIE_LUMINANCE);
    Image *L1 = FilterLuminance::execute(img1, NULL, LT_CIE_LUMINANCE);

    //extract corners
    HarrisCornerDetector hcd(2.5f, 5);
    hcd.execute(L0, &corners_from_img0);
    hcd.execute(L1, &corners_from_img1);

    //compute ORB descriptors for each corner and image

    //apply a gaussian filter to luminance images
    Image *L0_flt = FilterGaussian2D::execute(L0, NULL, 2.5f);
    Image *L1_flt = FilterGaussian2D::execute(L1, NULL, 2.5f);

    //compute ORB descriptor
    ORBDescriptor b_desc(31, 512);

    std::vector< unsigned int *> descs0;
    b_desc.getAll(L0_flt, corners_from_img0, descs0);

    std::vector< unsigned int *> descs1;
    b_desc.getAll(L1_flt, corners_from_img1, descs1);

    //match ORB descriptors
    std::vector< Eigen::Vector3i > matches;
    int n = b_desc.getDescriptorSize();

    //BinaryFeatureBruteForceMatcher bffm_bin(&descs1, n);
    BinaryFeatureLSHMatcher bffm_bin(&descs1, n, 64);
    bffm_bin.getAllMatches(descs0, matches);

    //get matches
    FeatureMatcher<unsigned int>::filterMatches(corners_from_img0, corners_from_img1, matches, m0, m1);

    //estimate the fundamental matrix
    F = estimateFundamentalWithNonLinearRefinement(m0, m1, inliers, 1000, 0.5, 1, 1000, 1e-4f);

    delete L0;
    delete L1;
    delete L0_flt;
    delete L1_flt;

    return F;
}
    
#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_FUNDAMENTAL_HPP
