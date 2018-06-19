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

#ifndef PIC_JNI_FIND_CHECKER_BOARD_HPP
#define PIC_JNI_FIND_CHECKER_BOARD_HPP

#include "../filtering/filter_luminance.hpp"

#include "../computer_vision/iterative_closest_point_2D.hpp"
#include "../computer_vision/nelder_mead_opt_ICP_2D.hpp"

#include "../features_matching/orb_descriptor.hpp"

#include "../algorithms/white_balance.hpp"

#include "../computer_vision/find_checker_board.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief extractCheckerBoardJNI
 * @param imageInPath is the path for the input image
 * @param imageOutPath is the path for the output image
 * @param x0 is the x coordinate for the first point for computing the scale
 * @param y0 is the y coordinate for the first point for computing the scale
 * @param x1 is the x coordinate for the second point for computing the scale
 * @param y1 is the y coordinate for the second point for computing the scale
 * @param length_pixels is the length in pixels between P0(x0, y0) and P1(x1, y1)
 * @param xw is the x coordinate for the white balance
 * @param yw is the y coordinate for the white balance
 * @return a boolean value: true --> success; false --> failure
 */
PIC_INLINE bool extractCheckerBoardJNI(std::string imageInPath, std::string imageOutPath,
                                       int &x0, int &y0, int &x1, int &y1,
                                       float &length_pixels,
                                       int &xw, int &yw
                                       )
{
    Image in;
    bool bRead = in.Read(imageInPath, LT_NOR_GAMMA);

    if(bRead) {

        std::vector< Eigen::Vector2f > corners;
        pic::findCheckerBoard(&in, corners);

        //
        //scale
        //
        Eigen::Vector2f p0, p1;
        float length = pic::estimateLengthOfCheckers(corners, p0, p1);

        x0 = p0[0];
        y0 = p0[1];

        x1 = p1[0];
        y1 = p1[1];

        //
        //white balance
        //
        Eigen::Vector2f pw = pic::estimateCoordinatesWhitePointFromCheckerBoard(&in, corners, 4, 6);

        xw = pw[0];
        yw = pw[1];

        Image* img_wb =  applyWhiteBalance(&in, xw, yw, true);

        if(img_wb != NULL) {
            bool bWrite = img_wb->Write(imageOutPath.c_str(), LT_NOR_GAMMA, 0);

            if(!bWrite) {
                printf("extractCheckerBoardJNI: the image could not be written.\n");
            }

            delete img_wb;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

#endif

} // end namespace pic

#endif // PIC_JNI_FIND_CHECKER_BOARD_HPP
