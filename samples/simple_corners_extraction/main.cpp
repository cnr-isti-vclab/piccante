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

#include <QCoreApplication>

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an LDR file...");

    pic::Image img;
    img.Read("../data/input/features/balcony_1.png", pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        bool bWritten;

        //FAST corners
        std::vector< Eigen::Vector3f > corners;
        pic::FastCornerDetector fcd;
        fcd.Update(1.0f, 5);
        fcd.Compute(&img, &corners);

        printf("\nFAST Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners.size(); i++) {
            printf("X: %3.2f Y: %3.2f\n", corners[i][0], corners[i][1]);
        }

        printf("\n");

        pic::Image *imgCorners = fcd.getCornersImage(&corners);

        bWritten = imgCorners->Write("../data/output/corner_fast_output.png");


        //Harris corners
        std::vector< Eigen::Vector3f > corners_harris;
        pic::HarrisCornerDetector hcd;
        hcd.Update(1.0f, 5);
        hcd.Compute(&img, &corners_harris);

        printf("\nHarris Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_harris.size(); i++) {
            printf("X: %3.2f Y: %3.2f\n", corners_harris[i][0], corners_harris[i][1]);
        }
        printf("\n");

        pic::Image *imgCorners_harris = fcd.getCornersImage(&corners_harris);
        bWritten = imgCorners_harris->Write("../data/output/corner_harris_output.png");


        //SUSAN corners
        std::vector< Eigen::Vector3f > corners_susan;
        pic::SusanCornerDetector scd;
        scd.Compute(&img, &corners_susan);

        printf("\nSUSAN Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_susan.size(); i++) {
            printf("X: %3.2f Y: %3.2f\n", corners_susan[i][0], corners_susan[i][1]);
        }
        printf("\n");

        pic::Image *imgCorners_susan = scd.getCornersImage(&corners_susan);

        bWritten = imgCorners_susan->Write("../data/output/corner_susan_output.png");

        printf("\n");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }
    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
