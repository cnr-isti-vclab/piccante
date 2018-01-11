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
    std::string img_str0, img_str1;

    if(argc == 3) {
        img_str0 = argv[1];
        img_str1 = argv[2];
    } else {
        img_str0 = "../data/input/features/checker_board_photo.png";
        img_str1 = "../data/input/features/checker_board_pattern.png";
    }

    pic::Image img, img_pattern;
    ImageRead(img_str0, &img);
    ImageRead(img_str1, &img_pattern);

    printf("Ok\n");

    printf("Is the image valid? ");
    if(img.isValid() && img_pattern.isValid()) {
        printf("OK\n");

        //compute the luminance images
        pic::Image *L = pic::FilterLuminance::Execute(&img, NULL, pic::LT_CIE_LUMINANCE);

        //get corners
        printf("Extracting corners...\n");
        pic::HarrisCornerDetector hcd(2.5f, 5);
        std::vector< Eigen::Vector3f > corners_from_img;
        hcd.execute(L, &corners_from_img);

        pic::GeneralCornerDetector::sortCorners(&corners_from_img);

        float *col_mu = img.getMeanVal(NULL, NULL);
        float *scaling = pic::FilterWhiteBalance::getScalingFactors(col_mu, img.channels);
        pic::FilterWhiteBalance fwb(scaling, img.channels);

        pic::Image *img_wb = fwb.Process(Single(&img), NULL);

        ImageWrite(img_wb, "../data/output/img_wb.png");

        std::vector< Eigen::Vector2f > cfi_out;

        pic::GeneralCornerDetector::removeClosestCorners(&corners_from_img, &cfi_out, 16.0f, 60);

        printf("%s\n", pic::GeneralCornerDetector::exportToString(&cfi_out).c_str());

    } else {
        printf("No there is at least an invalid file!\n");
    }

    return 0;
}
