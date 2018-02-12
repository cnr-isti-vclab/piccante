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

#ifndef PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP
#define PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP

#include "filtering/filter_luminance.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#include "externals/Eigen/SVD"
#include "externals/Eigen/Geometry"
#include "util/eigen_util.hpp"
#endif

namespace pic {

/**
 * @brief estimateCheckerBoardSize
 * @param points
 * @return
 */
float estimateCheckerBoardSize(std::vector< Eigen::Vector2f > &points)
{
    if(points.size() < 2) {
        return -1.0f;
    }

    float ret = 0.0f;

    int n = int(points.size());

    std::vector<float> m_d;
    for(int i = 0; i < n; i++) {
        Eigen::Vector2f p_i = points.at(i);

        float closest = FLT_MAX;

        for(int j = 0; j < n; j++) {
            if(j == i) {
                continue;
            }

            Eigen::Vector2f delta_ij = p_i - points.at(j);

            float dist = delta_ij.norm();

            if(closest < dist) {
                closest = dist;
            }
        }

        if(closest < FLT_MAX) {
            m_d.push_back(closest);
        }
    }

    std::sort(m_d.begin(), m_d.end());

    ret = m_d[m_d.size() / 2];

    return ret;
}

void findCheckerBoard(Image *img)
{
    pic::Image *img_L = FilterLuminance::Execute(img, NULL, pic::LT_CIE_LUMINANCE);

    std::vector< Eigen::Vector2f > corners, corners_new;
    pic::FastCornerDetector fcd;
    fcd.update(1.0f, 5);
    fcd.execute(img, &corners);


}

} // end namespace pic

#endif // PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP
