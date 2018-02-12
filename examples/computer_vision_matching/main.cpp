/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img0_str, img1_str;

    if(argc == 3) {
        img0_str = argv[1];
        img1_str = argv[2];
    } else {
        img0_str = "../data/input/features/balcony_0.png";
        img1_str = "../data/input/features/balcony_1.png";
    }

    pic::Image img0, img1;
    ImageRead(img0_str, &img0, pic::LT_NOR);
    ImageRead(img1_str, &img1, pic::LT_NOR);

    printf("Ok\n");

    printf("Are they both valid? ");
    if(img0.isValid() && img1.isValid()) {
        printf("OK\n");

        //output corners
        std::vector< Eigen::Vector2f > corners_from_img0;
        std::vector< Eigen::Vector2f > corners_from_img1;

        //compute the luminance images
        pic::Image *L0 = pic::FilterLuminance::Execute(&img0, NULL, pic::LT_CIE_LUMINANCE);
        pic::Image *L1 = pic::FilterLuminance::Execute(&img1, NULL, pic::LT_CIE_LUMINANCE);

        //get corners
        printf("Extracting corners...\n");
        pic::HarrisCornerDetector hcd(2.5f, 5);
        hcd.execute(L0, &corners_from_img0);
        hcd.execute(L1, &corners_from_img1);

        //compute luminance images
        pic::Image *L0_flt = pic::FilterGaussian2D::Execute(L0, NULL, 2.5f);
        pic::Image *L1_flt = pic::FilterGaussian2D::Execute(L1, NULL, 2.5f);

        //compute ORB descriptors for each corner and image
        printf("Computing ORB descriptors...\n");

        pic::ORBDescriptor b_desc(31, 512);

        std::vector< unsigned int *> descs0;
        b_desc.getAll(L0_flt, corners_from_img0, descs0);

        std::vector< unsigned int *> descs1;
        b_desc.getAll(L1_flt, corners_from_img1, descs1);

        printf("Matching ORB descriptors...\n");
        int n = b_desc.getDescriptorSize();
        printf("Descriptor size: %d\n", n);
        pic::BinaryFeatureBruteForceMatcher bfm(&descs1, n);
        std::vector< Eigen::Vector3i > matches;
        bfm.getAllMatches(descs0, matches);
        printf("Number of Matches: %d\n", matches.size());

        printf("Matches:\n");
        std::vector< Eigen::Vector2f > m0, m1;
        pic::BinaryFeatureMatcher::filterMatches(corners_from_img0, corners_from_img1, matches, m0, m1);

        printf("Estimating a homography matrix H from the matches...");
        std::vector< unsigned int > inliers;
        Eigen::Matrix3d H = pic::estimateHomographyWithNonLinearRefinement(m0, m1, inliers, 10000, 2.5f, 1, 10000, 1e-5f);

        printf("\nHomography matrix:\n");
        pic::printfMat(H);

        printf("Applying H to the first image..");
        pic::Image *img0_H = pic::FilterWarp2D::Execute(&img0, NULL, pic::MatrixConvert(H), true, false);
        ImageWrite(img0_H, "../data/output/simple_matching_img_0_H_applied.png", pic::LT_NOR);
        printf("Ok.\n");

        printf("\nEstimating the fundamental matrix F from the matches...");
        Eigen::Matrix3d F = pic::estimateFundamentalRansac(m0, m1, inliers, 10000);
        printf("Ok.\n");

        printf("\nFoundamental matrix:\n");
        pic::printfMat(F);
    } else {
        printf("No there is at least an invalid file!\n");
    }

    return 0;
}
