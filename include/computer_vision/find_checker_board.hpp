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

#include "computer_vision/iterative_closest_point_2D.hpp"
#include "computer_vision/nelder_mead_opt_ICP_2D.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#include "externals/Eigen/SVD"
#include "externals/Eigen/Geometry"
#include "util/eigen_util.hpp"
#endif

namespace pic {

/**
 * @brief getMinDistance
 * @param points
 * @return
 */
float getMinDistance(std::vector< Eigen::Vector2f > &points)
{
    float ret = FLT_MAX;
    for(auto i = 0; i < points.size(); i++) {

        auto p_i = points[i];

        for(auto j = 0; j < points.size(); j++) {
            if(j == i) {
                continue;
            }

            auto delta_ij = p_i - points[j];
            float dist = delta_ij.norm();

            if(dist < ret) {
                ret = dist;
            }
        }
    }

    return ret;
}

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
        auto p_i = points[i];

        float closest = FLT_MAX;

        for(int j = 0; j < n; j++) {
            if(j == i) {
                continue;
            }

            auto delta_ij = p_i - points[j];

            float dist = delta_ij.norm();

            if(dist < closest) {
                closest = dist;
            }
        }

        if(closest < FLT_MAX) {
            m_d.push_back(closest);
        }
    }

    if(!m_d.empty()) {
        std::sort(m_d.begin(), m_d.end());

        ret = m_d[m_d.size() / 2];
    }

    return ret;
}

/**
 * @brief getCheckerBoardModel
 * @param chekers_x
 * @param checkers_y
 * @param checkers_size
 * @param out
 */
void getCheckerBoardModel(int chekers_x, int checkers_y, int checkers_size, std::vector< Eigen::Vector2f > &out)
{
    float size_f = float(checkers_size);
    for(int i = 0; i < checkers_y; i++) {
        for(int j = 0; j < chekers_x; j++) {
            Eigen::Vector2f point;
            point[0] = float(j) * size_f;
            point[1] = float(i) * size_f;

            out.push_back(point);
        }
    }
}

/**
 * @brief findCheckerBoard
 * @param img
 */
void findCheckerBoard(Image *img)
{
    //compute the luminance images
    Image *L = FilterLuminance::Execute(img, NULL, LT_CIE_LUMINANCE);

    //get corners
    printf("Extracting corners...\n");
    HarrisCornerDetector hcd(2.5f, 5);
    std::vector< Eigen::Vector2f > corners_from_img;
    hcd.execute(L, &corners_from_img);

    float *col_mu = img->getMeanVal(NULL, NULL);
    float *scaling = FilterWhiteBalance::getScalingFactors(col_mu, img->channels);
    FilterWhiteBalance fwb(scaling, img->channels, true);

    Image *img_wb = fwb.Process(Single(img), NULL);

    float red[] = {1.0f, 0.0f, 0.0f};
    float green[] = {0.0f, 1.0f, 0.0f};
    float blue[] = {0.0f, 0.0f, 1.0f};
    float yellow[] = {1.0f, 1.0f, 0.0f};

    (*img_wb) *= 0.125f;

    std::vector< Eigen::Vector2f > cfi_out;
    GeneralCornerDetector::removeClosestCorners(&corners_from_img, &cfi_out, 16.0f, 100);

    //compute checkerboard size
    float checker_size = estimateCheckerBoardSize(corners_from_img);
    float threshold_checker = (checker_size * 0.75f);

    std::vector< Eigen::Vector2f > cfi_valid;
    auto n =  cfi_out.size();
    for(auto i = 0; i < n; i++) {
        auto p_i = cfi_out[i];

        bool bFlag = true;
        for(auto j = 0; j < n; j++) {
            if(j == i) {
                continue;
            }
            auto delta_ij = p_i - cfi_out[j];
            float dist = delta_ij.norm();
            if(dist < threshold_checker) {
                bFlag = false;
                break;
            }
        }

        if(bFlag) {
            cfi_valid.push_back(p_i);
        }
    }

    checker_size = estimateCheckerBoardSize(cfi_valid);

    drawPoints(img_wb, cfi_valid, green);

    //pattern image
    std::vector< Eigen::Vector2f > corners_model;

    int checkers_size = 32;
    getCheckerBoardModel(4, 6, checkers_size, corners_model);

    float min_dist = getMinDistance(corners_model);
    float scaling_factor = checker_size / min_dist;

    ICP2DTransform t_init;
    t_init.scale = scaling_factor;
    t_init.applyC(corners_model);

    iterativeClosestPoints2D(corners_model, cfi_valid, 1e-3f, 1000);

    drawPoints(img_wb, corners_model, red);

    NelderMeadOptICP2D opt(corners_model, cfi_valid);

    float prev_err = FLT_MAX;
    float *x;
    int nSample = 36;
    for(float i = 0; i < nSample; i++) {
        float angle = float(i) * C_PI_2 / float(nSample);
        float start[] = {0.0f, 0.0f, angle};
        float *tmp = opt.run(start, 3, 1e-10f, 1000);

        if(opt.output_error < prev_err) {
            x = tmp;
            prev_err = opt.output_error;
        } else {
            delete[] tmp;
        }
    }
    for(int i=0; i<4;i++) {
        printf("%f\n", x[i]);
    }

    float start[] = {x[0], x[1], x[2], 1.0f};

    float *tmp = opt.run(start, 4, 1e-10f, 1000);
    ICP2DTransform t2(tmp[0], tmp[1], tmp[2], tmp[3]);
    for(int i=0; i<4;i++) {
        printf("%f\n", tmp[i]);
    }
    t2.applyC(corners_model);
    drawPoints(img_wb, corners_model, yellow);

    img_wb->Write("../data/output/img_wb.bmp");
}

} // end namespace pic

#endif // PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP
