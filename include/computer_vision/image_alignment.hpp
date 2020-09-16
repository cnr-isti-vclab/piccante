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

#ifndef PIC_COMPUTER_VISION_IMAGE_ALIGNMENT_HPP
#define PIC_COMPUTER_VISION_IMAGE_ALIGNMENT_HPP

#include <vector>

#include "../base.hpp"
#include "../image.hpp"
#include "../features_matching/orb_descriptor.hpp"
#include "../features_matching/harris_corner_detector.hpp"
#include "../features_matching/binary_feature_brute_force_matcher.hpp"
#include "../computer_vision/homography_matrix.hpp"
#include "../filtering/filter_gaussian_2d.hpp"
#include "../filtering/filter_warp_2d.hpp"

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
 * @brief getHomographyMatrixFromTwoImage
 * @param img0
 * @param img1
 * @return
 */
Eigen::Matrix3d getHomographyMatrixFromTwoImage(Image *img0, Image *img1)
{
    //output corners
    std::vector< Eigen::Vector2f > corners_from_img0;
    std::vector< Eigen::Vector2f > corners_from_img1;

    //get corners
    HarrisCornerDetector hcd(2.5f, 5);
    hcd.execute(img0, &corners_from_img0);
    hcd.execute(img1, &corners_from_img1);

    //compute ORB descriptors for each corner and image
    ORBDescriptor b_desc(31, 512);

    std::vector< pic::uint* > descs0;
    b_desc.getAll(img0, corners_from_img0, descs0);

    std::vector< pic::uint* > descs1;
    b_desc.getAll(img1, corners_from_img1, descs1);

    //match descriptors
    int n = b_desc.getDescriptorSize();
    BinaryFeatureBruteForceMatcher bfm(&descs1, n);

    std::vector< Eigen::Vector3i > matches;
    bfm.getAllMatches(descs0, matches);

    //filter matches
    std::vector< Eigen::Vector2f > m0, m1;
    FeatureMatcher<uint>::filterMatches(corners_from_img0, corners_from_img1, matches, m0, m1);

    //estimate a homography matrix H from the matches
    std::vector< uint > inliers;
    Eigen::Matrix3d H = estimateHomographyWithNonLinearRefinement(m0, m1, inliers, 10000, 2.5f, 1, 10000, 1e-5f);

    return H;
}

/**
 * @brief imageAlignmentWithORB
 * @param img0
 * @param img1
 * @param imgOut
 * @return
 */
Image* imageAlignmentWithORB(Image *img0, Image *img1, Image *imgOut)
{
    auto H = getHomographyMatrixFromTwoImage(img0, img1);

    FilterWarp2D flt;
    flt.update(pic::MatrixConvert(H), true, false);
    imgOut = flt.Process(Single(img0), imgOut);

    return imgOut;
}

#endif

}

#endif // PIC_COMPUTER_VISION_IMAGE_ALIGNMENT_HPP
