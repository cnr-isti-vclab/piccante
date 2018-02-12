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
    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/features/balcony_0.png";
    }

    printf("Reading an LDR file...");

    pic::Image img;

    ImageRead(img_str, &img, pic::LT_NOR);

    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("OK\n");

        bool bWritten = true;

        //FAST corners
        std::vector< Eigen::Vector2f > corners_fast;
        pic::FastCornerDetector fcd;
        fcd.update(1.0f, 5);
        fcd.execute(&img, &corners_fast);

        printf("\nFAST Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_fast.size(); i++) {
            printf("X: %3.2f Y: %3.2f\n", corners_fast[i][0], corners_fast[i][1]);
        }

        printf("\n");

        pic::Image *imgCorners_fast = fcd.getCornersImage(&corners_fast, NULL, img.width, img.height, true);

        bWritten = ImageWrite(imgCorners_fast, "../data/output/corner_fast_output.png", pic::LT_NOR);

        //Harris corners
        std::vector< Eigen::Vector2f > corners_harris;
        pic::HarrisCornerDetector hcd;
        hcd.update(1.0f, 5);
        hcd.execute(&img, &corners_harris);

        printf("\nHarris Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_harris.size(); i++) {
            printf("X: %3.2f Y: %3.2f\n", corners_harris[i][0], corners_harris[i][1]);
        }
        printf("\n");

        pic::Image *imgCorners_harris = hcd.getCornersImage(&corners_harris, NULL, img.width, img.height, true);
        bWritten = ImageWrite(imgCorners_harris, "../data/output/corner_harris_output.png", pic::LT_NOR);

        //SUSAN corners
        std::vector< Eigen::Vector2f > corners_susan;
        pic::SusanCornerDetector scd;
        scd.execute(&img, &corners_susan);

        printf("\nSUSAN Corner Detector Test:\n");
        for(unsigned int i = 0; i < corners_susan.size(); i++) {
            printf("X: %3.2f Y: %3.2f\n", corners_susan[i][0], corners_susan[i][1]);
        }
        printf("\n");

        pic::Image *imgCorners_susan = scd.getCornersImage(&corners_susan, NULL, img.width, img.height, true);
        bWritten = ImageWrite(imgCorners_susan, "../data/output/corner_susan_output.png", pic::LT_NOR);

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
