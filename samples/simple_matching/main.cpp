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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an LDR images...");

    pic::Image *img0 = new pic::Image();
    img0->Read("../data/input/features/balcony_0.png", pic::LT_NOR);

    pic::Image *img1 = new pic::Image();
    img1->Read("../data/input/features/balcony_1.png", pic::LT_NOR);

    img0->Write("../data/output/simple_matching_img_0.png", pic::LT_NOR);
    img1->Write("../data/output/simple_matching_img_1.png", pic::LT_NOR);

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

        pic::ORBDescriptor b_desc(31, 512);

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

            unsigned int dist_1 = 0;
            unsigned int dist_2 = 0;

            int matched_j = -1;

            for(unsigned int j =0; j<descs1.size(); j++) {

                unsigned int dist = pic::BRIEFDescriptor::match(descs0.at(i), descs1.at(j), n);

                if(dist > 0) {
                    if(dist > dist_1) {
                        dist_2 = dist_1;
                        dist_1 = dist;
                        matched_j = j;
                    } else {
                        if(dist > dist_2) {
                            dist_2 = dist;
                        }
                    }
                }
            }

            if((dist_1 * 100 > dist_2 * 107) && matched_j != -1) {
                matches.push_back(Eigen::Vector3i(i, matched_j, dist_1));
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
        Eigen::Matrix3d H = pic::estimateHomographyRansac(m0, m1, inliers, 10000);
        printf("Ok.\n");

        pic::filterInliers(m0, inliers, m0f);
        pic::filterInliers(m1, inliers, m1f);

        //output to a file
        FILE *file = fopen("../data/output/simple_matching_matches.txt", "w");

        for(unsigned int i = 0; i < m0f.size(); i++) {
            fprintf(file, "%f %f %f %f\n", m0f[i][0], m0f[i][1], m1f[i][0], m1f[i][1]);
        }

        fclose(file);

        pic::Matrix3x3 mtxH;
        float *H_array = pic::getLinearArrayFromMatrix(H);
        pic::NelderMeadOptHomography nmoh(m0, m1, inliers);
        nmoh.run(H_array, 8, 1e-4f, 1000, mtxH.data);

        printf("\nHomography matrix:");
        mtxH.print();

        printf("Applying H to the first image..");
        pic::Image *img0_H = pic::FilterWarp2D::Execute(img0, NULL, mtxH, true, false);
        img0_H->Write("../data/output/simple_matching_img_0_H_applied.png", pic::LT_NOR);
        printf("Ok.\n");

        printf("\nEstimating the fundamental matrix F from the matches...");
        Eigen::Matrix3d F = pic::estimateFundamentalRansac(m0, m1, inliers, 10000);
        printf("Ok.\n");

        printf("\nFoundamental matrix: \n");
        pic::printfMat(F);
    } else {
        printf("No there is at least an invalid file!\n");
    }

    return 0;
}
