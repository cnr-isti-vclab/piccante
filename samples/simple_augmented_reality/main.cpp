/*

PICCANTE
The hottest HDR imaging library!
http://piccantelib.net

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#define EIGEN_DONT_VECTORIZE

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading LDR images...");

    //computing K matrix from manufacturer data
    double fx = pic::getFocalLengthPixels(3.3, 3.8, 2592.0);
    double fy = pic::getFocalLengthPixels(3.3, 2.9, 1936.0);
    Eigen::Matrix3d K = pic::getIntrinsicsMatrix(fx, fy, 2592.0 / 2.0, 1936.0 / 2.0);

    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an LDR images...");

    pic::Image *img0 = new pic::Image();
    img0->Read("../data/input/features/balcony_0.png", pic::LT_NOR);

    pic::Image *img1 = new pic::Image();
    img1->Read("../data/input/augmented_reality/desk.png", pic::LT_NOR);

    printf("Ok\n");

    printf("Are they both valid? ");
    if(img0->isValid() && img1->isValid()) {
        printf("OK\n");

        //output corners
        std::vector< Eigen::Vector3f > corners_from_img0;
        std::vector< Eigen::Vector3f > corners_from_img1;

        //computing the luminance images
        pic::Image *L0 = pic::FilterLuminance::Execute(img0, NULL, pic::LT_CIE_LUMINANCE);
        pic::Image *L1 = pic::FilterLuminance::Execute(img1, NULL, pic::LT_CIE_LUMINANCE);

        //getting corners
        printf("Extracting corners...\n");
        pic::HarrisCornerDetector hcd(2.5f, 5);
        hcd.Compute(L0, &corners_from_img0);
        hcd.Compute(L1, &corners_from_img1);

        //computing ORB descriptors for each corner and image
        //Computing luminance images
        pic::Image *L0_flt = pic::FilterGaussian2D::Execute(L0, NULL, 2.5f);
        pic::Image *L1_flt = pic::FilterGaussian2D::Execute(L1, NULL, 2.5f);

        printf("Computing ORB descriptors...\n");

        pic::PoissonDescriptor b_desc(16);
//        pic::ORBDescriptor b_desc(31, 512);

        std::vector< unsigned int *> descs0;
        for(unsigned int i=0; i<corners_from_img0.size(); i++) {
            int x = corners_from_img0[i][0];
            int y = corners_from_img0[i][1];

            descs0.push_back(b_desc.get(L0_flt, x, y));
        }

        std::vector< unsigned int *> descs1;
        for(unsigned int i=0; i<corners_from_img1.size(); i++) {
            int x = corners_from_img1[i][0];
            int y = corners_from_img1[i][1];

            descs1.push_back(b_desc.get(L1_flt, x, y));
        }

        printf("Matching ORB descriptors...\n");
        std::vector< Eigen::Vector3i > matches;

        int n = b_desc.getDescriptorSize();

        printf("Descriptor size: %d\n", n);

        for(unsigned int i = 0; i < descs0.size(); i++) {

            unsigned int dist = 0;

            int matched_j = -1;

            for(unsigned int j =0; j<descs1.size(); j++) {

                unsigned int tmp_dist = pic::BRIEFDescriptor::match(descs0.at(i), descs1.at(j), n);

                if(tmp_dist > dist) {
                    dist = tmp_dist;
                    matched_j = j;
                }
            }

            if(matched_j != -1) {
                matches.push_back(Eigen::Vector3i(i, matched_j, dist));
            }
        }

        printf("Mathces:\n");
        std::vector< Eigen::Vector2f > m0, m0f, m1, m1f;

        for(unsigned int i=0; i<matches.size(); i++) {
            int I0 = matches[i][0];
            int I1 = matches[i][1];

            Eigen::Vector2f x, y;

            x[0] = corners_from_img0[I0][0];
            x[1] = corners_from_img0[I0][1];

            y[0] = corners_from_img1[I1][0];
            y[1] = corners_from_img1[I1][1];

            m0.push_back(x);
            m1.push_back(y);

            printf("I1: %d (%d %d) -- I2: %d (%d %d) -- Score: %d\n", I0, int(x[0]), int(x[1]), I1, int(y[0]), int(y[1]), matches[i][2]);
        }

        printf("Estimating a homography matrix H from the matches...");
        std::vector< unsigned int > inliers;
        Eigen::Matrix3d H = pic::estimateHomographyRansac(m0, m1, inliers, 10000, 2.5f);
        printf("Ok.\n");

        pic::filterInliers(m0, inliers, m0f);
        pic::filterInliers(m1, inliers, m1f);

        float *H_array = pic::getLinearArrayFromMatrix(H);
        pic::NelderMeadOptHomography nmoh(m0, m1, inliers);
        nmoh.run(H_array, 8, 1e-5f, 10000, H_array);

        H = pic::getMatrix3dFromLinearArray(H_array);

        Eigen::Matrix34d cam = pic::getCameraMatrixFromHomography(H, K);

        (*img1) *= 0.25f;

        //Augmentation: drawing a 3D grid
        int res = 10;
        for(int i=1;i<(res + 1);i++) {
            float u = float(i) / 50.0f;

            for(int j=1;j<(res + 1);j++) {
                float v = float(j) / 50.0f;

                Eigen::Vector4d point(u, v, 0.0f, 1.0f);
                Eigen::Vector2i coord = pic::cameraMatrixProject(cam, point);

                float *tmp = (*img1)(coord[0], coord[1]);
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
        DrawLine(img1, pic::Vec<2, int>(coord0[0], coord0[1]), pic::Vec<2, int>(coord1[0], coord1[1]), color);
        DrawLine(img1, pic::Vec<2, int>(coord0[0], coord0[1]), pic::Vec<2, int>(coord2[0], coord2[1]), color);
        DrawLine(img1, pic::Vec<2, int>(coord0[0], coord0[1]), pic::Vec<2, int>(coord3[0], coord3[1]), color);


        img1->Write("../data/output/simple_augmented_reality.png", pic::LT_NOR);

    } else {
        printf("No, there is at least an invalid file!\n");
    }

    return 0;
}
