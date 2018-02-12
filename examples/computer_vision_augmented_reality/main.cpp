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

#define EIGEN_DONT_VECTORIZE

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

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
        img1_str = "../data/input/augmented_reality/desk.png";
    }

    //computing K matrix from manufacturer data
    double fx = pic::getFocalLengthPixels(3.3, 3.8, 2592.0);
    double fy = pic::getFocalLengthPixels(3.3, 2.9, 1936.0);
    Eigen::Matrix3d K = pic::getIntrinsicsMatrix(fx, fy, 2592.0 / 2.0, 1936.0 / 2.0);

    printf("Reading LDR images...");
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

        //compute ORB descriptors for each corner and image
        //compute luminance images
        pic::Image *L0_flt = pic::FilterGaussian2D::Execute(L0, NULL, 2.5f);
        pic::Image *L1_flt = pic::FilterGaussian2D::Execute(L1, NULL, 2.5f);

        printf("Computing ORB descriptors...\n");

        //pic::PoissonDescriptor b_desc(16);
        pic::ORBDescriptor b_desc(31, 512);

        std::vector< unsigned int *> descs0;
        b_desc.getAll(L0_flt, corners_from_img0 , descs0);

        std::vector< unsigned int *> descs1;
        b_desc.getAll(L1_flt, corners_from_img1 , descs1);

        printf("Matching ORB descriptors...\n");
        int n = b_desc.getDescriptorSize();

        pic::BinaryFeatureBruteForceMatcher bfm(&descs1, n);

        printf("Descriptor size: %d\n", n);

        printf("Matching...");
        std::vector< Eigen::Vector3i > matches;
        bfm.getAllMatches(descs0, matches);
        printf("Ok\n");

        //filter
        std::vector< Eigen::Vector2f > m0, m1;
        pic::BinaryFeatureMatcher::filterMatches(corners_from_img0, corners_from_img1, matches, m0, m1);

        printf("Estimating a homography matrix H from the matches...");

        std::vector< unsigned int > inliers;
        Eigen::Matrix3d H = pic::estimateHomographyWithNonLinearRefinement(m0, m1, inliers, 10000, 2.5f, 1, 10000, 1e-5f);

        Eigen::Matrix34d cam = pic::getCameraMatrixFromHomography(H, K);

        img1 *= 0.25f;

        //Augmentation: drawing a 3D grid
        int res = 10;
        for(int i=1;i<(res + 1);i++) {
            float u = float(i) / 50.0f;

            for(int j=1;j<(res + 1);j++) {
                float v = float(j) / 50.0f;

                Eigen::Vector4d point(u, v, 0.0f, 1.0f);
                Eigen::Vector2i coord = pic::cameraMatrixProject(cam, point);

                float *tmp = img1(coord[0], coord[1]);
                tmp[0] = 1.0f;
                tmp[1] = 0.25f;
                tmp[2] = 0.25f;
            }

        }

        //Augmentation: drawing 3D axis
        Eigen::Vector4d p0(0.0, 0.0, 0.0, 1.0);
        Eigen::Vector2i coord0 = pic::cameraMatrixProject(cam, p0);

        Eigen::Vector4d p1(0.2, 0.0, 0.0, 1.0);
        Eigen::Vector2i coord1 = pic::cameraMatrixProject(cam, p1);

        Eigen::Vector4d p2(0.0, 0.2, 0.0, 1.0);
        Eigen::Vector2i coord2 = pic::cameraMatrixProject(cam, p2);

        Eigen::Vector4d p3(0.0, 0.0, 0.2, 1.0);
        Eigen::Vector2i coord3 = pic::cameraMatrixProject(cam, p3);

        float color[]={0.25f, 1.0f, 0.25f};
        pic::drawLine(&img1, pic::Vec<2, int>(coord0[0], coord0[1]), pic::Vec<2, int>(coord1[0], coord1[1]), color);
        pic::drawLine(&img1, pic::Vec<2, int>(coord0[0], coord0[1]), pic::Vec<2, int>(coord2[0], coord2[1]), color);
        pic::drawLine(&img1, pic::Vec<2, int>(coord0[0], coord0[1]), pic::Vec<2, int>(coord3[0], coord3[1]), color);

        ImageWrite(&img1, "../data/output/simple_augmented_reality.png", pic::LT_NOR);

    } else {
        printf("No, there is at least an invalid file!\n");
    }

    return 0;
}
