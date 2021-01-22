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

#ifndef PIC_COMPUTER_VISION_ITERATIVE_CLOSEST_POINT_2D_HPP
#define PIC_COMPUTER_VISION_ITERATIVE_CLOSEST_POINT_2D_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../util/math.hpp"

#include "../features_matching/brief_descriptor.hpp"

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
#ifndef PIC_DISABLE_EIGEN

/**
 * @brief getMean
 * @param p
 * @return
 */
PIC_INLINE Eigen::Vector2f getMeanVector2f(std::vector< Eigen::Vector2f > &p)
{
    auto c = p[0];
    for(unsigned int i = 1; i < p.size(); i++) {
        c += p[i];
    }
    c /= float(p.size());

    return c;
}

/**
 * @brief getMedianVector2f
 * @param p
 * @return
 */
PIC_INLINE Eigen::Vector2f getMedianVector2f(std::vector< Eigen::Vector2f > &p)
{
    auto n = p.size();
    float *x = new float[n];
    float *y = new float[n];

    for(unsigned int i = 0; i < n; i++) {
        x[i] = p[i][0];
        y[i] = p[i][1];
    }

    std::sort(x, x + n);
    std::sort(y, y + n);

    Eigen::Vector2f med;

    med[0] = x[n >> 1];
    med[1] = y[n >> 1];

#ifdef PIC_DEBUG
    printf("%f %f\n", med[0], med[1]);
#endif

    delete[] x;
    delete[] y;
    return med;
}

class ICP2DTransform
{
public:
    Eigen::Matrix2f R;
    Eigen::Vector2f t;
    float scale;

    ICP2DTransform()
    {
        R.setIdentity();

        t.setZero();

        scale = 1.0f;
    }

    ICP2DTransform(float tx, float ty, float angle, float scale)
    {
        this->scale = scale;
        t[0] = tx;
        t[1] = ty;

        float cos_a = cosf(angle);
        float sin_a = sinf(angle);

        R(0, 0) =  cos_a;
        R(0, 1) = -sin_a;
        R(1, 0) =  sin_a;
        R(1, 1) =  cos_a;
    }

    void print()
    {
        printf("R:\n %f %f\n %f %f\n", R(0,0), R(0,1), R(1,0), R(1,1));

        printf("T: %f %f\n", t[0], t[1]);

        printf("S: %f\n\n", scale);
    }

    void apply(std::vector< Eigen::Vector2f > &points) {
        //apply transform
        for(unsigned int i  = 0; i < points.size(); i++) {
            Eigen::Vector2f tmp = points[i];
            points[i] = ((R * tmp) + t) * scale;
        }
    }

    void apply(std::vector< Eigen::Vector2f > &points,
               std::vector< Eigen::Vector2f > &out) {
        //apply transform
        for(unsigned int i  = 0; i < points.size(); i++) {
            Eigen::Vector2f tmp = ((R * points[i]) + t) * scale;
            out.push_back(tmp);
        }
    }

    //
    //
    //

    void applyC(std::vector< Eigen::Vector2f > &points) {

        //compute centroid to points
        Eigen::Vector2f c = getMeanVector2f(points);
        auto shift = c + t;

        //apply transform
        for(unsigned int i  = 0; i < points.size(); i++) {
            Eigen::Vector2f tmp = points[i] - c;
            points[i] = (R * tmp) * scale + shift;
        }
    }

    void applyC(std::vector< Eigen::Vector2f > &points,
               std::vector< Eigen::Vector2f > &out) {

        //compute centroid to points
        Eigen::Vector2f c = getMeanVector2f(points);
        auto shift = c + t;

        //apply transform
        for(unsigned int i  = 0; i < points.size(); i++) {
            Eigen::Vector2f tmp = points[i] - c;
            Eigen::Vector2f tmp2 = (R * tmp) * scale + shift;
            out.push_back(tmp2);
        }
    }
};

/**
 * @brief estimateRotatioMatrixAndTranslation
 * @param p0
 * @param p1
 * @param p0_descs
 * @param p1_descs
 * @param ind
 * @return
 */
PIC_INLINE ICP2DTransform estimateRotatioMatrixAndTranslation(std::vector< Eigen::Vector2f > &p0,
                                                   std::vector< Eigen::Vector2f > &p1,
                                                   std::vector< unsigned int *> &p0_descs,
                                                   std::vector< unsigned int *> &p1_descs,
                                                   int size_descs,
                                                   int *ind = NULL)
{
    ICP2DTransform ret;

    if(p0.size() < 2 || p1.size() < 2) {
        return ret;
    }

    bool bFlag = false;
    if(ind == NULL) {
        ind = new int[p1.size()];
        bFlag = true;
    }

    //compute c0
    Eigen::Vector2f c1 = getMeanVector2f(p1);

    //compute c1
    Eigen::Vector2f c0;
    c0.setZero();
    int n = 0;

#ifdef PIC_DEBUG
    printf("Size: %d\n", size_descs);
#endif

    for(uint i = 0; i < p1.size(); i++) {
        auto p_i = p1[i];

        float d_min = FLT_MAX;
        int index = -1;
        for(uint j = 0; j < p0.size(); j++) {
            auto delta_ij = p_i - p0[j];
            float d_tmp = delta_ij.norm();

            int value = BRIEFDescriptor::match(p0_descs[j], p1_descs[i], size_descs);
            d_tmp += float(size_descs * 32) - float(value);

            if(d_tmp < d_min) {
                d_min = d_tmp;
                index = j;
            }

        }

        if(index > -1) {
            ind[i] = index;
            c0 += p0[index];
            n++;
        }
    }
    c0 /= float(n);


    //compute R
    Eigen::Matrix2f H;
    H.setZero();

    for(unsigned int i = 0; i < p1.size(); i++) {
        int j = ind[i];

        auto t0 = p0[j] - c0;
        auto t1 = p1[i] - c1;

        Eigen::RowVector2f t1r = t1;
        Eigen::Matrix2f tmp = t0 * t1r;

/*      tmp(0, 0) = t0(0) * t1(0);
        tmp(0, 1) = t0(0) * t1(1);
        tmp(1, 0) = t0(1) * t1(0);
        tmp(1, 1) = t0(1) * t1(1);*/
        H += tmp;
    }

    //SVD decomposition
    Eigen::JacobiSVD< Eigen::Matrix2f > svd(H, Eigen::ComputeFullV | Eigen::ComputeFullU);
    Eigen::Matrix2f U = svd.matrixU();
    Eigen::Matrix2f V = svd.matrixV();

    Eigen::Matrix2f U_t = U.transpose();
    Eigen::Matrix2f R = V * U_t;

    if(R.determinant() < 0.0f) {
        for(auto i = 0; i < V.rows(); i++) {
            V(i, 1) = -V(i, 1);
        }

        R = V * U_t;
    }

    ret.R = R;
    ret.t = c0 - (ret.R * c1);

    if(bFlag) {
        delete[] ind;
    }

    return ret;
}

/**
 * @brief getErrorPointsList
 * @param p0
 * @param p1
 * @return
 */
PIC_INLINE float getErrorPointsList(std::vector< Eigen::Vector2f > &p0,
                         std::vector< Eigen::Vector2f > &p1)
{
    float err = 0.0f;
    for(unsigned int i = 0; i < p0.size(); i++) {
        auto p_i = p0[i];

        float tmp_err = FLT_MAX;
        for(unsigned int j = 0; j < p1.size(); j++) {
            auto delta_ij = p_i - p1[j];
            float dist = delta_ij.norm();

            if(dist < tmp_err) {
                tmp_err = dist;
            }
        }

        err += tmp_err;
    }

    return err / float(p0.size());
}

/**
 * @brief iterativeClosestPoints2D
 * @param points_pattern
 * @param points
 * @param points_pattern_descs
 * @param points_descs
 * @param thresholdErr
 * @param maxIterations
 */
PIC_INLINE void iterativeClosestPoints2D(std::vector<Eigen::Vector2f> &points_pattern,
                              std::vector<Eigen::Vector2f> &points,
                              std::vector< unsigned int *> &points_pattern_descs,
                              std::vector< unsigned int *> &points_descs,
                              int size_descs,
                              int maxIterations = 1000)
{
    ICP2DTransform t_init;
    t_init.t = getMedianVector2f(points) - getMeanVector2f(points_pattern);
    t_init.apply(points_pattern);

    float err = getErrorPointsList(points_pattern, points);;
    float prev_err = 1e32f;
    int iter = 0;
    while(iter < maxIterations) {
        prev_err = err;
        ICP2DTransform t = estimateRotatioMatrixAndTranslation(points, points_pattern,
                                                               points_descs, points_pattern_descs,
                                                               size_descs);

#ifdef PIC_DEBUG
        t.print();
#endif

//        std::vector< Eigen::Vector2f > points_pattern_tmp;
        t.apply(points_pattern);

        err = getErrorPointsList(points_pattern, points);

        /*
        if(err < prev_err) {
            points_pattern.clear();
            std::copy(points_pattern_tmp.begin(), points_pattern_tmp.end(),
                      std::back_inserter(points_pattern));
        } else {
            iter = maxIterations;
        }
        */

        #ifdef PIC_DEBUG
            printf("Error: %f %f\n", err, prev_err);
        #endif

        iter++;
    }
}

#endif
} // end namespace pic

#endif // PIC_COMPUTER_VISION_ITERATIVE_CLOSEST_POINT_2D_HPP
