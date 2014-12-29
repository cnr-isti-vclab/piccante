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

#ifndef PIC_COMPUTER_VISION_FUNCTIONS
#define PIC_COMPUTER_VISION_FUNCTIONS

#include <vector>
#include <random>
#include <stdlib.h>

#include "util/math.hpp"

#include "features_matching/general_corner_detector.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#include "externals/Eigen/SVD"
#include "externals/Eigen/Geometry"

#include "util/eigen_util.hpp"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief estimateHomography estimates an homography matrix H between image 1 to image 2
 * @param points0 is an array of points computed from image 1.
 * @param points1 is an array of points computed from image 2.
 * @return It returns the homography matrix H.
 */
Eigen::Matrix3d estimateHomography(std::vector< Eigen::Vector2f > points0, std::vector< Eigen::Vector2f > points1)
{
    Eigen::Matrix3d  H;

    if((points0.size() != points1.size()) || (points0.size() < 4)) {
        H.setZero();
        return H;
    }

    Eigen::Vector3f transform_0 = ComputeNormalizationTransform(points0);
    Eigen::Vector3f transform_1 = ComputeNormalizationTransform(points1);

    Eigen::Matrix3d mat_0 = getShiftScaleMatrix(transform_0);
    Eigen::Matrix3d mat_1 = getShiftScaleMatrix(transform_1);

    unsigned int n = points0.size();
    Eigen::MatrixXd A(n * 2, 9);

    //setting up the linear system
    for(unsigned int i = 0; i < n; i++) {
        //transforming coordinates for increasing stability of the system
        Eigen::Vector2f p0 = points0[i];
        Eigen::Vector2f p1 = points1[i];

        p0[0] = (p0[0] - transform_0[0]) / transform_0[2];
        p0[1] = (p0[1] - transform_0[1]) / transform_0[2];

        p1[0] = (p1[0] - transform_1[0]) / transform_1[2];
        p1[1] = (p1[1] - transform_1[1]) / transform_1[2];

        int j = i * 2;
        A(j, 0) = 0.0;
        A(j, 1) = 0.0;
        A(j, 2) = 0.0;
        A(j, 3) = p0[0];
        A(j, 4) = p0[1];
        A(j, 5) = 1.0;
        A(j, 6) = -p1[1] * p0[0];
        A(j, 7) = -p1[1] * p0[1];
        A(j, 8) = -p1[1];

        j++;

        A(j, 0) = p0[0];
        A(j, 1) = p0[1];
        A(j, 2) = 1.0;
        A(j, 3) = 0.0;
        A(j, 4) = 0.0;
        A(j, 5) = 0.0;
        A(j, 6) = -p1[0] * p0[0];
        A(j, 7) = -p1[0] * p0[1];
        A(j, 8) = -p1[0];
    }

    //Solving the linear system
    Eigen::JacobiSVD< Eigen::MatrixXd > svd(A, Eigen::ComputeFullV);
    Eigen::MatrixXd V = svd.matrixV();

    n = V.cols() - 1;

    //assigning + transposing
    H(0, 0) = V(0, n);
    H(0, 1) = V(1, n);
    H(0, 2) = V(2, n);

    H(1, 0) = V(3, n);
    H(1, 1) = V(4, n);
    H(1, 2) = V(5, n);

    H(2, 0) = V(6, n);
    H(2, 1) = V(7, n);
    H(2, 2) = V(8, n);

    H = mat_1.inverse() * H * mat_0;
    return H / H(2, 2);
}

/**
 * @brief estimateHomographyRansac computes the homography such that: points1 = H * points0
 * @param points0
 * @param points1
 * @param inliers
 * @param maxIterations
 * @return
 */
Eigen::Matrix3d estimateHomographyRansac(std::vector< Eigen::Vector2f > points0, std::vector< Eigen::Vector2f > points1,
                                         std::vector< unsigned int > &inliers, unsigned int maxIterations = 100, double threshold = 4.0)
{
    if(points0.size() < 5) {
        return estimateHomography(points0, points1);
    }

    Eigen::Matrix3d H;
    int nSubSet = 4;

    std::mt19937 m(rand() % 10000);

    unsigned int n = points0.size();

    unsigned int *subSet = new unsigned int [nSubSet];

    inliers.clear();

    for(unsigned int i = 0; i < maxIterations; i++) {       

        getPermutation(m, subSet, nSubSet, n);

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(int j = 0; j < nSubSet; j++) {
            sub_points0.push_back(points0[subSet[j]]);
            sub_points1.push_back(points1[subSet[j]]);
        }

        Eigen::Matrix3d tmpH = estimateHomography(sub_points0, sub_points1);

        //is it a good one?
        std::vector< unsigned int > tmp_inliers;

        for(unsigned int j = 0; j < n; j++) {
            Eigen::Vector3d point_hom = Eigen::Vector3d(points0[j][0], points0[j][1], 1.0);
            Eigen::Vector3d pp = tmpH * point_hom;
            pp /= pp[2];

            double dx = points1[j][0] - pp[0];
            double dy = points1[j][1] - pp[1];
            double squared_diff = (dx * dx) + (dy * dy);

            if(squared_diff < threshold) {
                tmp_inliers.push_back(j);
            }
        }

        //getting the inliers
        if(tmp_inliers.size() > inliers.size()) {
            H = tmpH;
            inliers.clear();
            inliers.assign(tmp_inliers.begin(), tmp_inliers.end());
        }
    }

    //improving estimate with inliers only
    if(inliers.size() > 3) {

        #ifdef PIC_DEBUG
            printf("Better estimate using inliers only.\n");
        #endif

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(unsigned int i = 0; i < inliers.size(); i++) {
            sub_points0.push_back(points0[inliers[i]]);
            sub_points1.push_back(points1[inliers[i]]);
        }

        H = estimateHomography(sub_points0, sub_points1);
    }

    return H;
}

/**
 * @brief estimateFundamental estimates the foundamental matrix between image 1 to image 2
 * @param points0 is an array of points computed from image 1.
 * @param points1 is an array of points computed from image 2.
 * @return It returns the fundamental matrix, F_{1,2}.
 */
Eigen::Matrix3d estimateFundamental(std::vector< Eigen::Vector2f > points0, std::vector< Eigen::Vector2f > points1)
{
    Eigen::Matrix3d F;

    if((points0.size() != points1.size()) || (points0.size() < 8)) {
        F.setZero();
        return F;
    }

    //shifting and scaling points for numerical stability
    Eigen::Vector3f transform_0 = ComputeNormalizationTransform(points0);
    Eigen::Vector3f transform_1 = ComputeNormalizationTransform(points1);

    Eigen::Matrix3d mat_0 = getShiftScaleMatrix(transform_0);
    Eigen::Matrix3d mat_1 = getShiftScaleMatrix(transform_1);

    Eigen::MatrixXd A(points0.size(), 9);

    //setting up the linear system
    for(unsigned int i = 0; i < points0.size(); i++) {

        //transforming coordinates for increasing stability of the system
        Eigen::Vector2f p0 = points0[i];
        Eigen::Vector2f p1 = points1[i];

        p0[0] = (p0[0] - transform_0[0]) / transform_0[2];
        p0[1] = (p0[1] - transform_0[1]) / transform_0[2];

        p1[0] = (p1[0] - transform_1[0]) / transform_1[2];
        p1[1] = (p1[1] - transform_1[1]) / transform_1[2];

        A(i, 0) = p0[0] * p1[0];
        A(i, 1) = p0[0] * p1[1];
        A(i, 2) = p0[0];
        A(i, 3) = p0[1] * p1[0];
        A(i, 4) = p0[1] * p1[1];
        A(i, 5) = p0[1];
        A(i, 6) = p1[0];
        A(i, 7) = p1[1];
        A(i, 8) = 1.0;
    }

    //Solving the linear system
    Eigen::JacobiSVD< Eigen::MatrixXd > svd(A, Eigen::ComputeFullV);
    Eigen::MatrixXd V = svd.matrixV();

    int n = V.cols() - 1;

    F(0, 0) = V(0, n);
    F(1, 0) = V(1, n);
    F(2, 0) = V(2, n);

    F(0, 1) = V(3, n);
    F(1, 1) = V(4, n);
    F(2, 1) = V(5, n);

    F(0, 2) = V(6, n);
    F(1, 2) = V(7, n);
    F(2, 2) = V(8, n);

    //Computing final F matrix
    Eigen::Matrix3d mat_1_t = Eigen::Transpose< Eigen::Matrix3d >(mat_1);
    F = mat_1_t * F * mat_0;

    //enforcing singularity
    Eigen::JacobiSVD< Eigen::MatrixXd > svdF(F, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Matrix3d Uf = svdF.matrixU();
    Eigen::Matrix3d Vf = svdF.matrixV();
    Eigen::Vector3d Df = svdF.singularValues();
    Df[2] = 0.0;

    Eigen::Matrix3d F_new = Uf * DiagonalMatrix(Df) * Eigen::Transpose< Eigen::Matrix3d >(Vf);

    double norm = MAX(Df[0], Df[1]);
    return F_new / norm;
}

/**
 * @brief estimateFundamentalRansac
 * @param points0
 * @param points1
 * @param inliers
 * @param maxIterations
 * @return
 */
Eigen::Matrix3d estimateFundamentalRansac(std::vector< Eigen::Vector2f > points0, std::vector< Eigen::Vector2f > points1,
                                          std::vector< unsigned int > &inliers, unsigned int maxIterations = 100, double threshold = 0.01)
{
    if(points0.size() < 9) {
        return estimateFundamental(points0, points1);
    }

    Eigen::Matrix3d F;
    int nSubSet = 8;

    std::mt19937 m(rand() % 10000);

    unsigned int n = points0.size();

    unsigned int *subSet = new unsigned int [nSubSet];

    inliers.clear();

    for(unsigned int i = 0; i < maxIterations; i++) {
        getPermutation(m, subSet, nSubSet, n);

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(int j = 0; j < nSubSet; j++) {
            unsigned int k = subSet[j];
            sub_points0.push_back(points0[k]);
            sub_points1.push_back(points1[k]);
        }

        Eigen::Matrix3d tmpF = estimateFundamental(sub_points0, sub_points1);

        //is it a good one?
        std::vector< unsigned int > tmp_inliers;

        for(unsigned int j = 0; j < n; j++) {
            Eigen::Vector3d p0 = Eigen::Vector3d(points0[j][0], points0[j][1], 1.0);
            Eigen::Vector3d p1 = Eigen::Vector3d(points1[j][0], points1[j][1], 1.0);

            Eigen::Vector3d tmpF_p0 = tmpF * p0;
            double n0 = sqrt(tmpF_p0[0] * tmpF_p0[0] + tmpF_p0[1] * tmpF_p0[1]);
            if(n0 >  0.0) {
                tmpF_p0 /= n0;
            }

            double err = fabs(tmpF_p0.dot(p1));

            if(err < threshold){
                tmp_inliers.push_back(j);
            }
        }

        //getting the inliers
        if(tmp_inliers.size() > inliers.size()) {
            F = tmpF;
            inliers.clear();
            inliers.assign(tmp_inliers.begin(), tmp_inliers.end());
        }
    }

    //improving estimate with inliers only
    if(inliers.size() > 7) {

        #ifdef PIC_DEBUG
            printf("Better estimate using inliers only.\n");
        #endif

        std::vector< Eigen::Vector2f > sub_points0;
        std::vector< Eigen::Vector2f > sub_points1;

        for(unsigned int i = 0; i < inliers.size(); i++) {
            sub_points0.push_back(points0[inliers[i]]);
            sub_points1.push_back(points1[inliers[i]]);
        }

        F = estimateFundamental(sub_points0, sub_points1);
    }

    return F;
}

/**
 * @brief noramalizeFundamentalMatrix
 * @param F
 * @return
 */
Eigen::Matrix3d noramalizeFundamentalMatrix(Eigen::Matrix3d F)
{
    Eigen::JacobiSVD< Eigen::Matrix3d > svdF(F, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Matrix3d Uf = svdF.matrixU();
    Eigen::Matrix3d Vf = svdF.matrixV();
    Eigen::Vector3d Df = svdF.singularValues();
    Df[2] = 0.0;

    Eigen::Matrix3d F_new = Uf * DiagonalMatrix(Df) * Eigen::Transpose< Eigen::Matrix3d >(Vf);

    double norm = MAX(Df[0], Df[1]);
    return F_new / norm;
}

/**
 * @brief extractFundamentalMatrix
 * @param M0
 * @param M1
 * @param e0
 * @param e1
 * @return
 */
Eigen::Matrix3d extractFundamentalMatrix(Eigen::Matrix34d &M0, Eigen::Matrix34d &M1, Eigen::VectorXd &e0, Eigen::VectorXd &e1) {

    Eigen::Matrix3d M0_3 = getSquareMatrix(M0);
    Eigen::Matrix3d M1_3 = getSquareMatrix(M1);


    Eigen::Matrix3d M0_inv = M0_3.inverse();
    Eigen::Vector3d c0 = - M0_inv * getLastColumn(M0);
    e1 = M1 * addOne(c0);

    Eigen::Matrix3d M1_inv = M1_3.inverse();
    Eigen::Vector3d c1 = - M1_inv * getLastColumn(M1);
    e0 = M0 * addOne(c1);

    Eigen::Matrix3d F;

    F(0, 0) =  0.0;
    F(0, 1) = -e1(2);
    F(0, 2) =  e1(1);

    F(1, 0) =  e1(2);
    F(1, 1) =  0.0;
    F(1, 2) = -e1(0);

    F(2, 0) = -e1(1);
    F(2, 1) =  e1(0);
    F(2, 2) =  0.0;

    F = F * M1_3 * M0_inv;

    Eigen::JacobiSVD< Eigen::Matrix3d > svdF(F, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Vector3d Df = svdF.singularValues();

    double norm = MAX(Df[0], MAX(Df[1], Df[2]));
    return F / norm;
}


/**
 * @brief filterInliers
 * @param vec
 * @param inliers
 * @param vecOut
 */
template<class T>
void filterInliers(std::vector< T > &vec, std::vector< unsigned int > &inliers, std::vector< T > &vecOut)
{
    vecOut.clear();

    if(!inliers.empty()) {
        for(unsigned int i = 0; i < inliers.size(); i++) {
            vecOut.push_back(vec[inliers[i]]);
        }
    } else {
        vecOut.assign(vec.begin(), vec.end());
    }
}

/**
 * @brief computeEpipole computes the epipole of a fundamental matrix F.
 * @param F is a fundamental matrix.
 * @return It returns the epipole of F.
 */
Eigen::Vector3d computeEpipole(Eigen::Matrix3d &F)
{
    Eigen::JacobiSVD< Eigen::Matrix3d > svdF(F, Eigen::ComputeFullV);
    Eigen::Matrix3d V = svdF.matrixV();

    Eigen::Vector3d e;

    e[0] = V(0, 2);
    e[1] = V(1, 2);
    e[2] = V(2, 2);

    return e;
}

/**
 * @brief computeEssentialMatrix computes the essential matrix, E, from the fundamental
 * matrix, F12, and the two intrics matrices K1 and K2
 * @param F is the fundamental matrix which maps points from camera 1 into camera 2
 * @param K1 is the camera 1 intrics matrix
 * @param K2 is the camera 2 intrics matrix
 * @return
 */
Eigen::Matrix3d computeEssentialMatrix(Eigen::Matrix3d &F, Eigen::Matrix3d &K1, Eigen::Matrix3d &K2)
{
    Eigen::Matrix3d K2t = Eigen::Transpose<Eigen::Matrix3d>(K2);
    return K2t * F * K1;
}

/**
 * @brief computeEssentialMatrix computes the essential matrix, E, from the fundamental
 * matrix, F, and a single instrics camera, K.
 * @param F
 * @param K
 * @return
 */
Eigen::Matrix3d computeEssentialMatrix(Eigen::Matrix3d &F, Eigen::Matrix3d &K)
{
    return computeEssentialMatrix(F, K, K);
}

/**
 * @brief getFocalLengthFromFOVAngle
 * @param fovy is an angle in radians.
 * @return
 */
double getFocalLengthFromFOVAngle(double fovy)
{
    return 1.0 / tan(fovy / 2.0);
}

/**
 * @brief getFOVAngleFromFocalSensor
 * @param f is the focal length in mm
 * @param x is the sensor width in mm
 * @param y is the sensor height in mm
 * @return
 */
double getFOVAngleFromFocalSensor(double f, double x, double y)
{
    double d = sqrt(x * x + y * y);
    return 2.0 * atan(d /  (2.0 * f));
}

/**
 * @brief getFocalLengthPixels
 * @param focal_length_mm
 * @param sensor_size_mm
 * @param sensor_size_px
 * @return
 */
double getFocalLengthPixels(double focal_length_mm, double sensor_size_mm, double sensor_size_px)
{
    return (focal_length_mm * sensor_size_px) / sensor_size_mm;
}

/**
 * @brief getIntrinsicsMatrix
 * @param focal_length
 * @param m_x
 * @param m_y
 * @param opitical_center_x
 * @param opitical_center_y
 * @return
 */
Eigen::Matrix3d getIntrinsicsMatrix(double focal_length, double m_x, double m_y, double opitical_center_x, double opitical_center_y)
{
    Eigen::Matrix3d K;
    K.setZero();
    K(0, 0) = focal_length * m_x;
    K(1, 1) = focal_length * m_y;
    K(2, 2) = 1.0;

    K(0, 2) = opitical_center_x;
    K(1, 2) = opitical_center_y;

    return K;
}

/**
 * @brief getIntrinsicsMatrix
 * @param focal_length_x
 * @param focal_length_y
 * @param opitical_center_y
 * @return
 */
Eigen::Matrix3d getIntrinsicsMatrix(double focal_length_x, double focal_length_y, double opitical_center_x, double opitical_center_y)
{
    Eigen::Matrix3d K;
    K.setZero();
    K(0, 0) = focal_length_x;
    K(1, 1) = focal_length_y;
    K(2, 2) = 1.0;

    K(0, 2) = opitical_center_x;
    K(1, 2) = opitical_center_y;

    return K;
}

/**
 * @brief removeLensDistortion
 * @param point
 * @param K
 * @return
 */
Eigen::Vector2d removeLensDistortion(Eigen::Vector2d &p, double k[5])
{
    Eigen::Vector2d ret;

    double r_2 = p[0] * p[0] + p[1] * p[1];
    double r_4 = r_2 * r_2;

    double c = 1.0 + k[0] * r_2 + k[1] * r_4 + k[4] * r_4 *r_2;

    Eigen::Vector2d dx;
    dx[0] = 2 * k[2] * p[0] * p[1] + k[3] * (r_2 + 2.0 * p[0] * p[0]);
    dx[1] = k[2] * (r_2 + 2 * p[1] * p[1]) + 2.0 * k[3] * p[0] * p[1];

    ret = p * c + dx;

    return ret;
}

/**
 * @brief getCameraMatrixFromHomography
 * @param H is 3x3 homography matrix.
 * @param K
 * @return
 */
Eigen::Matrix34d getCameraMatrixFromHomography(Eigen::Matrix3d &H, Eigen::Matrix3d &K)
{
    Eigen::Matrix34d m;
    m.setZero();

    Eigen::Matrix3d K_inv = K.inverse();

    Eigen::Matrix3d H_p = K_inv * H;

    Eigen::Vector3d r_0(H_p(0, 0), H_p(1, 0), H_p(2, 0));
    Eigen::Vector3d r_1(H_p(0, 1), H_p(1, 1), H_p(2, 1));

    r_0.normalize();
    r_1.normalize();
    Eigen::Vector3d r_2 = r_0.cross(r_1);

    Eigen::Vector3d t(H_p(0, 2), H_p(1, 2), H_p(2, 2));

    m(0, 0) = r_0[0];
    m(1, 0) = r_0[1];
    m(2, 0) = r_0[2];

    m(0, 1) = r_1[0];
    m(1, 1) = r_1[1];
    m(2, 1) = r_1[2];

    m(0, 2) = r_2[0];
    m(1, 2) = r_2[1];
    m(2, 2) = r_2[2];

    m(0 , 3) = t[0];
    m(1 , 3) = t[1];
    m(2 , 3) = t[2];

    return K * m;
}

/**
 * @brief getCameraMatrixIdentity
 * @param K
 * @return
 */
Eigen::Matrix34d getCameraMatrixIdentity(Eigen::Matrix3d &K)
{
    Eigen::Matrix34d m;
    m.setZero();

    m(0, 0) = 1.0;
    m(1, 1) = 1.0;
    m(2, 2) = 1.0;

    return K * m;
}

/**
 * @brief getCameraMatrix
 * @param K
 * @param R
 * @param t
 * @return
 */
Eigen::Matrix34d getCameraMatrix(Eigen::Matrix3d &K, Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    Eigen::Matrix34d m;

    m(0, 0) = R(0, 0);
    m(1, 0) = R(1, 0);
    m(2, 0) = R(2, 0);

    m(0, 1) = R(0, 1);
    m(1, 1) = R(1, 1);
    m(2, 1) = R(2, 1);

    m(0, 2) = R(0, 2);
    m(1, 2) = R(1, 2);
    m(2, 2) = R(2, 2);

    m(0, 3) = t[0];
    m(1, 3) = t[1];
    m(2, 3) = t[2];

    return K * m;
}

/**
 * @brief cameraMatrixProject projects a point, p, using the camera
 * matrix, M.
 * @param M
 * @param p is a 3D point encoded in homogenous coordinate (4D vector)
 * @return
 */
Eigen::Vector2i cameraMatrixProject(Eigen::Matrix34d &M, Eigen::Vector4d &p)
{
    Eigen::Vector3d proj = M * p;
    proj[0] /= proj[2];
    proj[1] /= proj[2];

    return Eigen::Vector2i(int(proj[0]), int(proj[1]));
}

/**
 * @brief cameraMatrixProject projects a point, p, using the camera
 * matrix, M.
 * @param M
 * @param p is a 3D point (3D vector)
 * @return
 */
Eigen::Vector2i cameraMatrixProject(Eigen::Matrix34d &M, Eigen::Vector3d &p)
{
    Eigen::Vector4d p4d(p[0], p[1], p[2], 1.0);
    return cameraMatrixProject(M, p4d);
}

/**
 * @brief decomposeEssentialMatrix decomposes an essential matrix E.
 * @param E is the essential matrix. Input.
 * Note1: E = S * R
 * Note2: S = [t]_x
 * Note3: there are four possible cases:
 * 1:     [R1 |  t]
 * 2:     [R1 | -t]
 * 3:     [R2 |  t]
 * 4:     [R2 | -t]
 * @param R1 is one possible rotation matrix. Output.
 * @param R2 is one possible rotation matrix. Output.
 * @param t is the translation vector which is not normalized. Output.
 */
void decomposeEssentialMatrix(Eigen::Matrix3d &E, Eigen::Matrix3d &R1, Eigen::Matrix3d &R2, Eigen::Vector3d &t)
{
    //Solving the linear system
    Eigen::JacobiSVD< Eigen::MatrixXd > svd(E, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Matrix3d U = svd.matrixU();
    Eigen::Matrix3d V = svd.matrixV();

    //Z matrix
    Eigen::Matrix3d Z;
    Z.setZero();
    Z(0, 1) =  1.0;
    Z(1, 0) = -1.0;

    //W matrix
    Eigen::Matrix3d W;
    W.setZero();
    W(0, 1) = -1.0;
    W(1, 0) =  1.0;
    W(2, 2) =  1.0;

    //Rotation matrices R1 and R2
    Eigen::Matrix3d Vt = Eigen::Transpose<Eigen::Matrix3d>(V);
    Eigen::Matrix3d Wt = Eigen::Transpose<Eigen::Matrix3d>(W);

    Eigen::Matrix3d  UVt = U * Vt;
    double detUVt = UVt.determinant();

    R1 = detUVt * U * W  * Vt;
    R2 = detUVt * U * Wt * Vt;

    R1 = RotationMatrixRefinement(R1);
    R2 = RotationMatrixRefinement(R2);

    //Translation vector
    Eigen::Matrix3d Ut = Eigen::Transpose<Eigen::Matrix3d>(U);
    Eigen::Matrix3d S = U * Z * Ut;

    t[0] = S(2, 1);
    t[1] = S(0, 2);
    t[2] = S(1, 0);

    t.normalize();
}

/**
 * @brief triangulationLonguetHiggins computes triangulation using Longuet-Higgins equations.
 * @param point_0 is the point from the first view which matches point_1
 * @param point_1 is the point from the second view which matches point_0
 * @param R is the rotation matrix between the two views.
 * @param t is the translation matrix between the two views.
 * @return
 */
Eigen::Vector3d triangulationLonguetHiggins(Eigen::Vector3d &point_0, Eigen::Vector3d &point_1, Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    Eigen::Vector3d ret;

    Eigen::Vector3d r_0 = Eigen::Vector3d(R(0, 0), R(0, 1), R(0, 2));
    Eigen::Vector3d r_2 = Eigen::Vector3d(R(2, 0), R(2, 1), R(2, 2));


    Eigen::Vector3d tmp = r_0 - point_1[0] * r_2;

    ret[2] = tmp.dot(t) / tmp.dot(point_0);

    ret[0] = point_0[0] * ret[2];
    ret[1] = point_0[1] * ret[2];

    return ret;
}

/**
 * @brief triangulationHartleySturm
 * @param point_0
 * @param point_1
 * @param R
 * @param t
 * @return
 */
Eigen::Vector4d triangulationHartleySturm(Eigen::Vector3d &point_0, Eigen::Vector3d &point_1,
                                          Eigen::Matrix34d &M0, Eigen::Matrix34d &M1, int maxIter = 100)
{

    Eigen::Vector4d M0_row[3], M1_row[3];

    for(int i = 0; i < 3; i++) {
        M0_row[i] = Eigen::Vector4d(M0(i, 0), M0(i, 1), M0(i, 2), M0(i, 3));
        M1_row[i] = Eigen::Vector4d(M1(i, 0), M1(i, 1), M1(i, 2), M1(i, 3));
    }

    Eigen::Vector4d x;
    double weight0 = 1.0;
    double weight0_prev = 1.0;

    double weight1 = 1.0;
    double weight1_prev = 1.0;

    int j = 0;
    while(j < maxIter) {
        Eigen::Vector4d A0 = (M0_row[0] - point_0[0] * M0_row[2]) / weight0;
        Eigen::Vector4d A1 = (M0_row[1] - point_0[1] * M0_row[2]) / weight0;

        Eigen::Vector4d A2 = (M1_row[0] - point_1[0] * M1_row[2]) / weight1;
        Eigen::Vector4d A3 = (M1_row[1] - point_1[1] * M1_row[2]) / weight1;

        Eigen::MatrixXd A(4, 4);
        for(int i = 0; i < 4; i++) {
            A(0, i) = A0[i];
            A(1, i) = A1[i];
            A(2, i) = A2[i];
            A(3, i) = A3[i];
        }

        Eigen::JacobiSVD< Eigen::MatrixXd > svdA(A, Eigen::ComputeFullV);
        Eigen::MatrixXd V = svdA.matrixV();
        int n = V.cols() - 1;

        x[0] = V(0, n);
        x[1] = V(1, n);
        x[2] = V(2, n);
        x[3] = V(3, n);
        x /= x[3];

        weight0_prev = weight0;
        weight1_prev = weight1;

        weight0 = x.dot(M0_row[2]);
        weight1 = x.dot(M1_row[2]);

        double d0 = weight0_prev - weight0;
        double d1 = weight1_prev - weight1;
        double err = sqrt(d0 * d0 + d1 * d1);

        if(err < 1e-12){
            break;
        }

        j++;
    }

    #ifdef PIC_DEBUG
        printf("triangulationHartleySturm's Iterations: %d\n",j);
    #endif

    return x;
}

/**
 * @brief decomposeEssentialMatrixWithConfiguration decomposes an essential matrix E.
 * @param E is the essential matrix.
 * @param K0
 * @param K1
 * @param points0
 * @param points1
 * @param R
 * @param t
 * @return
 */
bool decomposeEssentialMatrixWithConfiguration(Eigen::Matrix3d &E, Eigen::Matrix3d &K0, Eigen::Matrix3d &K1,
                                               std::vector< Eigen::Vector2f > &points0, std::vector< Eigen::Vector2f > &points1,
                                               Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    if(points0.size() != points1.size()) {
        return false;
    }

    Eigen::Matrix3d R0, R1;
    Eigen::Vector3d T;
    decomposeEssentialMatrix(E, R0, R1, T);

    //for each configuration (R0, -T), (R0, T), (R1, -T), (R1, T)
    //the sign of reconstructed points is checked
    int type = -1;
    int counter = -1;

    for(unsigned int j = 0; j < 4; j++) {

        Eigen::Matrix3d tmp_R = (j < 2) ? R0 : R1;
        Eigen::Vector3d tmp_T;

        if((j % 2) == 0) {
            tmp_T = T;
        } else {
            tmp_T = -T;
        }

        Eigen::Matrix34d M0 = getCameraMatrixIdentity(K0);
        Eigen::Matrix34d M1 = getCameraMatrix(K1, tmp_R, tmp_T);

        int tmp_counter = 0;
        for(unsigned int i = 0; i < points0.size(); i++) {
            //homogeneous coordinates
            Eigen::Vector3d point_0 = Eigen::Vector3d(points0[i][0], points0[i][1], 1.0);
            Eigen::Vector3d point_1 = Eigen::Vector3d(points1[i][0], points1[i][1], 1.0);

            Eigen::Vector4d p0 = triangulationHartleySturm(point_0, point_1, M0, M1);
            Eigen::Vector3d p0_euc = Eigen::Vector3d(p0[0], p0[1], p0[2]);
            Eigen::Vector3d p1 = rigidTransform(p0_euc, tmp_R, tmp_T);

            if((p0[2] >= 0.0) && (p1[2] >= 0.0)) {
                tmp_counter++;
            }

        }

        printf("Front: %d %d\n",tmp_counter,j);

        if(tmp_counter > counter) {
            type = j;
            counter = tmp_counter;
        }
    }

    if(type > -1) {

        R = (type < 2) ? R0 : R1;

        if((type % 2) == 0) {
            t = T;
        } else {
            t = -T;
        }

        return true;

    } else {
        R.setZero();
        t.setZero();

        return false;
    }
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_FUNCTIONS
