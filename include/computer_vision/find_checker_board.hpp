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

#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_bilateral_2ds.hpp"

#include "../computer_vision/iterative_closest_point_2D.hpp"
#include "../computer_vision/nelder_mead_opt_ICP_2D.hpp"

#include "../features_matching/harris_corner_detector.hpp"
#include "../features_matching/orb_descriptor.hpp"

#include "../util/rasterizer.hpp"
#include "../util/eigen_util.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
    #include "../externals/Eigen/SVD"
    #include "../externals/Eigen/Geometry"
#else
    #include <Eigen/Dense>
    #include <Eigen/SVD>
    #include <Eigen/Geometry>
#endif

#endif

namespace pic {

/**
 * @brief getMinDistance
 * @param points
 * @return
 */
#ifndef PIC_DISABLE_EIGEN

PIC_INLINE float getMinDistance(std::vector< Eigen::Vector2f > &points)
{
    float ret = FLT_MAX;
    for(unsigned int i = 0; i < points.size(); i++) {

        auto p_i = points[i];

        for(unsigned int j = 0; j < points.size(); j++) {
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
PIC_INLINE float estimateCheckerBoardSize(std::vector< Eigen::Vector2f > &points)
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
 * @brief estimateCheckerBoardSizeCross
 * @param points
 * @return
 */
PIC_INLINE float estimateCheckerBoardSizeCross(std::vector< Eigen::Vector2f > &points)
{
    if(points.size() < 2) {
        return -1.0f;
    }

    float ret = 0.0f;

    int n = int(points.size());

    std::vector<float> m_d;
    for(int i = 0; i < n; i++) {
        auto p_i = points[i];

        float c[] = {FLT_MAX, FLT_MAX, FLT_MAX};
        int ci[] = {-1, -1, -1};

        for(int j = 0; j < n; j++) {
            if(j == i) {
                continue;
            }

            auto delta_ij = p_i - points[j];

            float dist = delta_ij.norm();

            if(dist < c[0]) {
                c[0] = dist;
                ci[0] = j;
            } else {
                if(dist < c[1]) {
                    c[1] = dist;
                    ci[1] = j;
                } else {
                    if(dist < c[2]) {
                        c[2] = dist;
                        ci[2] = j;
                    }
                }
            }
        }

        Eigen::Vector2f v0 = (points[ci[0]] - p_i) / c[0];
        Eigen::Vector2f v1 = (points[ci[1]] - p_i) / c[1];
        Eigen::Vector2f v2 = (points[ci[2]] - p_i) / c[2];

        float v01 = fabsf(v0.dot(v1));
        float v02 = fabsf(v0.dot(v2));
        float v12 = fabsf(v1.dot(v2));

        if((v01 < 0.1f) || (v02 < 0.1f) || (v12 < 0.1f))
        {
            m_d.push_back(c[0]);
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
 * @return
 */
PIC_INLINE Image *getCheckerBoardModel(int checkers_x, int checkers_y, int checkers_size, std::vector< Eigen::Vector2f > &out)
{
    Image *ret = new Image(1, (checkers_x + 1) * checkers_size, (checkers_y + 1) * checkers_size, 1);
    *ret = 1.0f;

    for(int i = 1; i <= checkers_y; i++) {
        Eigen::Vector2f point;

        int y = i * checkers_size;
        point[1] = float(y);

        for(int j = 1; j <= checkers_x; j++) {

            int x = j * checkers_size;
            point[0] = float(x);

            bool bDraw = false;
            if(j < checkers_x) {
                if(((j % 2) == 0) && ((i % 2) == 0)) {
                    bDraw = true;
                }
            }

            if(i < checkers_y) {
                if(((j % 2) == 1) && ((i % 2) == 1)) {
                    bDraw = true;
                }
            }

            if(bDraw) {
                for(int yy = y; yy < (y + checkers_size); yy++) {
                    for(int xx = x; xx < (x + checkers_size); xx++) {
                        float *pixel_value = (*ret)(xx, yy);
                        pixel_value[0] = 0.0f;
                    }
                }
            }

            out.push_back(point);
        }
    }

    return ret;
}

/**
 * @brief findCheckerBoard
 * @param img
 * @param corners_model
 * @param checkerBoardSizeX
 * @param checkerBoardSizeY
 */
PIC_INLINE void findCheckerBoard(Image *img, std::vector< Eigen::Vector2f > &corners_model, int checkerBoardSizeX = 4, int checkerBoardSizeY = 7)
{
     corners_model.clear();

    //get corners
#ifdef PIC_DEBUG
    printf("Extracting corners...\n");
#endif

    //compute the luminance images
    HarrisCornerDetector hcd(2.5f, 5);
    std::vector< Eigen::Vector2f > corners_from_img;
    hcd.execute(img, &corners_from_img);

    #ifdef PIC_DEBUG
        //automatic white balance
        float *col_mu = img->getMeanVal(NULL, NULL);
        float *scaling = FilterWhiteBalance::getScalingFactors(col_mu, img->channels);
        FilterWhiteBalance fwb(scaling, img->channels, true);

        Image *img_wb = fwb.Process(Single(img), NULL);

        float red[] = {1.0f, 0.0f, 0.0f};
        float green[] = {0.0f, 1.0f, 0.0f};
        float blue[] = {1.0f, 0.0f, 1.0f};
        float yellow[] = {1.0f, 1.0f, 0.0f};

        (*img_wb) *= 0.125f;
    #endif

    std::vector< Eigen::Vector2f > cfi_out;
    GeneralCornerDetector::removeClosestCorners(&corners_from_img, &cfi_out, 16.0f, 64);

    //compute checkerboard size
    float checker_size = estimateCheckerBoardSize(corners_from_img);

    #ifdef PIC_DEBUG
        //drawPoints(img_wb, cfi_out, blue);
    #endif

    //
    // remove very closed points
    //

    std::vector< Eigen::Vector2f > cfi_valid2;
    auto n =  cfi_out.size();
    for(unsigned int i = 0; i < n; i++) {
        auto p_i = cfi_out[i];

        bool bFlag = true;

        for(unsigned int j = 0; j < n; j++) {
            if(j != i) {
                auto delta_ij = p_i - cfi_out[j];
                float dist = delta_ij.norm();

                if(dist < (checker_size)) {
                    bFlag = false;
                    break;
                }
            }
        }

        if(bFlag) {
            cfi_valid2.push_back(p_i);
        }
    }

    //
    // remove very far away points
    //

    std::vector< Eigen::Vector2f > cfi_valid;
    n =  cfi_valid2.size();
    for(unsigned int i = 0; i < n; i++) {
        auto p_i = cfi_valid2[i];

        float dist = 1e32f;
        for(unsigned int j = 0; j < n; j++) {
            if(j != i) {
                auto delta_ij = p_i - cfi_valid2[j];
                float t_dist = delta_ij.norm();

                if(t_dist < dist) {
                    dist = t_dist;
                }
            }
        }

        if(dist < (checker_size * 3)) {
            cfi_valid.push_back(p_i);
        }
    }

    #ifdef PIC_DEBUG
        printf("Checker size: %f\n", checker_size);
        drawPoints(img_wb, cfi_valid, green);
    #endif

    checker_size = estimateCheckerBoardSizeCross(cfi_valid);

#ifdef PIC_DEBUG
    printf("Re-fit Checker size: %f\n", checker_size);
#endif
    //pattern image

    int checkers_size = 32;
    Image *img_pattern = getCheckerBoardModel(checkerBoardSizeX, checkerBoardSizeY, checkers_size, corners_model);
//    corners_model.erase(corners_model.begin() + 3);
//    corners_model.erase(corners_model.begin());

    ORBDescriptor b_desc(checkers_size, 256);

    std::vector< unsigned int *> descs_model, descs_cfi_valid;
    b_desc.getAll(img_pattern, corners_model, descs_model);
    b_desc.getAll(img, cfi_valid, descs_cfi_valid);

    //scale the model using the checker size
    float min_dist = getMinDistance(corners_model);
    float scaling_factor = checker_size / min_dist;

    ICP2DTransform t_init;
    t_init.scale = scaling_factor;
    t_init.applyC(corners_model);

    //run 2D ICP
    iterativeClosestPoints2D(corners_model, cfi_valid, descs_model, descs_cfi_valid, b_desc.getDescriptorSize(), 3000);

#ifdef PIC_DEBUG
    drawPoints(img_wb, corners_model, red);
#endif

    //At this point, the rotation may be wrong so
    //this brute-force trick does the job.
    NelderMeadOptICP2D opt(corners_model, cfi_valid);

    float prev_err = FLT_MAX;
    float *x = new float[3];
    int nSample = 72;

    float *tmp = new float[4];
    for(float i = 0; i < nSample; i++) {
        float angle = float(i) * C_PI_2 / float(nSample);
        float start[] = {0.0f, 0.0f, angle};
        opt.run(start, 3, 1e-9f, 100, tmp);

        if(opt.output_error < prev_err) {
            memcpy(x, tmp, sizeof(float) * 3);
            prev_err = opt.output_error;
        }
    }

    #ifdef PIC_DEBUG
        for(int i = 0; i < 4; i++) {
            printf("%f\n", x[i]);
        }
    #endif

    float start[] = {x[0], x[1], x[2], 1.0f};
    opt.run(start, 4, 1e-12f, 100, tmp);
    ICP2DTransform t2(tmp[0], tmp[1], tmp[2], tmp[3]);

    #ifdef PIC_DEBUG
        for(int i = 0; i < 4; i++) {
            printf("%f\n", tmp[i]);
        }
    #endif

    t2.applyC(corners_model);

    #ifdef PIC_DEBUG
        drawPoints(img_wb, corners_model, yellow);
        img_wb->Write("../data/output/img_wb.bmp");

        if(img_wb != NULL) {
            delete img_wb;
        }
    #endif
}

/**
 * @brief estimateLengthInPixelOfCheckers
 * @param corners_model
 * @param p0
 * @param p1
 * @return
 */
PIC_INLINE float estimateLengthOfCheckers(std::vector< Eigen::Vector2f > &corners_model, Eigen::Vector2f &p0, Eigen::Vector2f &p1)
{
    if(corners_model.size() < 8) {
        return -1.0f;
    }

    int selected = 5;
    auto p_0 = corners_model[selected];

    int closest = -1;
    float ret = FLT_MAX;
    for(uint j = 0; j < corners_model.size(); j++) {
        if(j != selected) {
            auto delta_ij = p_0 - corners_model[j];
            float dist = delta_ij.norm();

            if(dist < ret) {
                ret = dist;
                closest = j;
            }
        }
    }

    p0 = p_0;
    p1 = corners_model[closest];

    return ret;
}

/**
 * @brief estimateCoordinatesWhitePointFromCheckerBoard
 * @param img
 * @param corners_model
 * @param checkerBoardSizeX
 * @param checkerBoardSizeY
 * @return
 */
PIC_INLINE Eigen::Vector2f estimateCoordinatesWhitePointFromCheckerBoard(Image *img, std::vector< Eigen::Vector2f > &corners_model, int checkerBoardSizeX = 4, int checkerBoardSizeY = 6)
{
    Eigen::Vector2f ret(-1.0f, -1.0f);

    if(img == NULL || corners_model.empty()) {
        return ret;
    }

    float maxVal = 0.0f;

    for(int i = 0; i < (checkerBoardSizeY -1) ; i++) {
        for(int j = 0; j < (checkerBoardSizeX - 1); j++) {

            int ind0 = (i * checkerBoardSizeX) + j;
            int ind1 = (i + 1) * checkerBoardSizeX + j + 1;

            auto p0 = corners_model[ind0];
            auto p1 = corners_model[ind1];

            auto pMid = (p0 + p1) / 2.0f;

            int x = int(pMid[0]);
            int y = int(pMid[1]);
            float *color = (*img)(x, y);

            float meanColor = 0.0f;
            for(auto c = 0; c < img->channels; c++) {
                meanColor += color[c];
            }

            if(meanColor > maxVal) {
                maxVal = meanColor;
                ret = pMid;
            }
        }
    }

    return ret;
}

/**
 * @brief estimateWhitePointFromCheckerBoard
 * @param img
 * @param corners_model
 * @param checkerBoardSizeX
 * @param checkerBoardSizeY
 * @return
 */
PIC_INLINE float *estimateWhitePointFromCheckerBoard(Image *img, std::vector< Eigen::Vector2f > &corners_model, int checkerBoardSizeX = 4, int checkerBoardSizeY = 6)
{
    Eigen::Vector2f point = estimateCoordinatesWhitePointFromCheckerBoard(img, corners_model, checkerBoardSizeX, checkerBoardSizeY);

    if(point[0] >= 0.0f && point[1] >= 0.0f) {

        float *ret = new float[img->channels];
        float *color = (*img)(int(point[0]), int(point[1]));
        memcpy(ret, color, img->channels * sizeof(float));

        return ret;
    } else {
        return NULL;
    }
}

#endif

} // end namespace pic

#endif // PIC_COMPUTER_VISION_FIND_CHECKER_BOARD_HPP
