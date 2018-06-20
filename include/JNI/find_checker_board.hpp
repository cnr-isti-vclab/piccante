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
 * @return
 */
PIC_INLINE std::vector<int> extractCheckerBoardJNI(std::string imageInPath, std::string imageOutPath)
{
    Image in;
    bool bRead = in.Read(imageInPath, LT_NOR_GAMMA);

    std::vector<int> ret;
    ret.clear();

    if(bRead) {

        std::vector< Eigen::Vector2f > corners;
        pic::findCheckerBoard(&in, corners);

        //
        //scale
        //
        Eigen::Vector2f p0, p1;
        pic::estimateLengthOfCheckers(corners, p0, p1);

        ret.push_back(int(p0[0]));
        ret.push_back(int(p0[1]));
        ret.push_back(int(p1[0]));
        ret.push_back(int(p1[1]));

        //
        //white balance
        //
        Eigen::Vector2f pw = pic::estimateCoordinatesWhitePointFromCheckerBoard(&in, corners, 4, 6);

        ret.push_back(int(pw[0]));
        ret.push_back(int(pw[1]));

        Image* img_wb =  applyWhiteBalance(&in, pw[0], pw[1], true);

        if(img_wb != NULL) {
            bool bWrite = img_wb->Write(imageOutPath.c_str(), LT_NOR_GAMMA, 0);

            if(!bWrite) {
                printf("extractCheckerBoardJNI: the image could not be written.\n");
            }

            delete img_wb;
        }
    }

    return ret;
}

#endif

} // end namespace pic

#endif // PIC_JNI_FIND_CHECKER_BOARD_HPP
