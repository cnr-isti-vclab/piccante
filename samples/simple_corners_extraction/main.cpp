/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

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

    pic::ImageRAW img;
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
            printf("X: %f Y: %f\n", corners[i][0], corners[i][1]);
        }

        printf("\n");

        pic::ImageRAW *imgCorners = fcd.getCornersImage(&corners);

        bWritten = imgCorners->Write("../data/output/fast_corner_image.png");


        //Harris corners
        std::vector< Eigen::Vector3f > corners_harris;
        pic::HarrisCornerDetector hcd;
        hcd.Update(1.0f, 5);
        hcd.Compute(&img, &corners_harris);

        printf("\nHarris Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_harris.size(); i++) {
            printf("X: %f Y: %f\n", corners_harris[i][0], corners_harris[i][1]);
        }
        printf("\n");

        pic::ImageRAW *imgCorners_harris = fcd.getCornersImage(&corners_harris);
        bWritten = imgCorners_harris->Write("../data/output/harris_corner_image.png");


        //SUSAN corners
        std::vector< Eigen::Vector3f > corners_susan;
        pic::SusanCornerDetector scd;
        scd.Compute(&img, &corners_susan);

        printf("\nSUSAN Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_susan.size(); i++) {
            printf("X: %f Y: %f\n", corners_susan[i][0], corners_susan[i][1]);
        }
        printf("\n");

        pic::ImageRAW *imgCorners_susan = scd.getCornersImage(&corners_susan);

        bWritten = imgCorners_susan->Write("../data/output/susan_corner_image.png");

        printf("\n");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }
    } else {
        printf("No it is not a valid file!\n");
    }

    return 0;
}
