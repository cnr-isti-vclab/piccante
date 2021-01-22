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
#include "../filtering/filter_downsampler_2d.hpp"
#include "../filtering/filter_white_balance.hpp"

#include "../computer_vision/iterative_closest_point_2D.hpp"
#include "../computer_vision/nelder_mead_opt_ICP_2D.hpp"

#include "../features_matching/orb_descriptor.hpp"

#include "../computer_vision/find_checker_board.hpp"

#include "../algorithms/binarization.hpp"
#include "../util/mask.hpp"
#include "../features_matching/canny_edge_detector.hpp"

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

    Image *work;

    if(bRead) {

        bool bScale = false;
        float scale = 1.0f;

        if(in.nPixels() > 1000000) {

            int maxLength = MAX(in.width, in.height);

            scale = 1000.0f / float(maxLength);

#ifdef PIC_DEBUG
            printf("Down scale factor: %f\n", scale);
#endif

            work = FilterDownSampler2D::execute(&in, NULL, scale);

            bScale = true;
        } else {
            work = &in;
        }

        std::vector< Eigen::Vector2f > corners;
        pic::findCheckerBoard(work, corners);

        //
        //scale
        //
        Eigen::Vector2f p0, p1;

#ifdef PIC_DEBUG
        float pixel_length = pic::estimateLengthOfCheckers(corners, p0, p1);
        printf("Pixel length: %f\n", pixel_length);
#endif

        ret.push_back(int(p0[0] / scale));
        ret.push_back(int(p0[1] / scale));
        ret.push_back(int(p1[0] / scale));
        ret.push_back(int(p1[1] / scale));

        //
        //white balance
        //
        Eigen::Vector2f pw = pic::estimateCoordinatesWhitePointFromCheckerBoard(work, corners, 4, 6);

        ret.push_back(int(pw[0] / scale));
        ret.push_back(int(pw[1] / scale));

        Image* img_wb;
        if(bScale) {
            int patchSize = 5;
            BBox patch(int(pw[0]) - patchSize, 
                       int(pw[0]) + patchSize,
                       int(pw[1]) - patchSize,
                       int(pw[1]) + patchSize);
            float *white_color = work->getMeanVal(&patch, NULL);
            img_wb = FilterWhiteBalance::execute(&in, white_color, NULL);
        } else {
            img_wb = FilterWhiteBalance::execute(&in, int(pw[0]), int(pw[1]), true, NULL);
        }

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
