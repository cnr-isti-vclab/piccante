/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#include "util/matrix_3_x_3.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/eigen/LU"
#endif

#ifndef PIC_EIGEN_UTIL
#define PIC_EIGEN_UTIL

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief DiagonalMatrix creates a diagonal matrix.
 * @param D a vector of size 3.
 * @return It returns a diagonal matrix.
 */
Eigen::Matrix3d DiagonalMatrix(Eigen::Vector3d D)
{
    Eigen::Matrix3d ret;

    ret.setZero();
    ret(0, 0) = D[0];
    ret(1, 1) = D[1];
    ret(2, 2) = D[2];

    return ret;
}

/**
 * @brief RemoveLastColumn
 * @param mat
 * @return
 */
Eigen::MatrixXd RemoveLastColumn(Eigen::MatrixXd &mat)
{
    Eigen::MatrixXd ret = Eigen::MatrixXd(mat.rows(), mat.cols() - 1);

    for(int i = 0; i < mat.rows(); i++) {
        for(int j = 0; j < (mat.cols() - 1); j++) {
            ret(i, j) = mat(i, j);
        }
    }
    return ret;
}

/**
 * @brief getColumn
 * @param mat
 * @param col
 * @return
 */
Eigen::VectorXd getColumn(Eigen::MatrixXd &mat, unsigned int col)
{
    Eigen::VectorXd ret = Eigen::VectorXd(mat.rows());

    for(int i = 0; i < mat.rows(); i++) {
        ret[i] = mat(i, col);
    }
    return ret;
}

/**
 * @brief addOne
 * @param mat
 * @param col
 * @return
 */
Eigen::VectorXd addOne(Eigen::VectorXd x)
{
    unsigned int n = MAX(x.rows(), x.cols());
    Eigen::VectorXd ret = Eigen::VectorXd(n + 1);

    for(unsigned int i = 0; i < n; i++) {
        ret[i] = x[i];
    }

    ret[n] = 1.0f;

    return ret;
}

/**
 * @brief printf
 * @param mat
 */
void printfMat(Eigen::MatrixXd mat)
{
    for(int i = 0; i < mat.rows(); i++){
        for(int j = 0; j < mat.cols(); j++){
            printf("%3.3f ", mat(i, j));
        }
        printf("\n");
    }
}

/**
 * @brief printf
 * @param mat
 */
void printfMat(Eigen::Matrix3f mat)
{
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf("%f ", mat(i, j));
        }
        printf("\n");
    }
}

/**
 * @brief fprintf
 * @param mat
 */
void fprintfMat(Eigen::MatrixXd mat, std::string name)
{
    FILE *file = fopen(name.c_str(), "w");
    for(int i = 0; i < mat.rows(); i++){
        for(int j = 0; j < mat.cols(); j++){
            fprintf(file, "%f ", mat(i, j));
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

/**
 * @brief printf
 * @param mat
 */
void printfMat(Eigen::Matrix3d mat)
{
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf("%f ", mat(i, j));
        }
        printf("\n");
    }
}

/**
 * @brief getShiftScaleMatrix computes a shifting and scaling matrix
 * @param info is an array with the center (0 and 1) a scaling factor (3)
 * @return It returns a scaling and shifting matrix.
 */
Eigen::Matrix3d getShiftScaleMatrix(Eigen::Vector3f info)
{
    Eigen::Matrix3d ret;

    double cX = info[0];
    double cY = info[1];
    double s  = 1.0 / info[2];

    ret(0,0) = s;   ret(0,1) = 0.0; ret(0,2) = -cX / info[2];
    ret(1,0) = 0.0; ret(1,1) = s;   ret(1,2) = -cY / info[2];
    ret(2,0) = 0.0; ret(2,1) = 0.0; ret(2,2) = 1.0;

    return ret;
}

/**
 * @brief CrossProduct computes a cross product matrix from a vector.
 * @param t a translation vector
 * @return It returns a cross product matrix.
 */
Eigen::Matrix3d CrossProduct(Eigen::Vector3d t)
{
    Eigen::Matrix3d ret;
    ret(0, 0) =  0.0;  ret(0, 1) = -t[2]; ret(0, 2) =  t[1];
    ret(1, 0) =  t[2]; ret(1, 1) =  0.0;  ret(1, 2) = -t[0];
    ret(2, 0) = -t[1]; ret(2, 1) =  t[0]; ret(2, 2) =  0.0;
    return ret;
}

/**
 * @brief rigidTransform computes a rigidi transformation in 3D.
 * @param point is the point to be transformed.
 * @param R is a rotation matrix 3x3.
 * @param t is a translation vector.
 * @return
 */
Eigen::Vector3d rigidTransform(Eigen::Vector3d &point, Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    return R * point + t;
}

/**
 * @brief RotationMatrixRefinement
 * @param R
 * @return
 */
Eigen::Matrix3d RotationMatrixRefinement(Eigen::Matrix3d &R)
{
    Eigen::Quaternion<double> reg(R);

    return reg.toRotationMatrix();
}

/**
 * @brief MatrixConvert converts a matrix from a Eigen::Matrix3f representation
 * into a Matrix3x3 representation.
 * @param mat is an Eigen 3x3 matrix.
 * @return It returns a Matrix3x3 with values from mat.
 */
Matrix3x3 MatrixConvert(Eigen::Matrix3f &mat)
{
    Matrix3x3 mtx;
    mtx.data[0] = mat(0, 0);
    mtx.data[1] = mat(0, 1);
    mtx.data[2] = mat(0, 2);

    mtx.data[3] = mat(1, 0);
    mtx.data[4] = mat(1, 1);
    mtx.data[5] = mat(1, 2);

    mtx.data[6] = mat(2, 0);
    mtx.data[7] = mat(2, 1);
    mtx.data[8] = mat(2, 2);

    return mtx;
}


/**
 * @brief MatrixConvert converts a matrix from a Eigen::Matrix3f representation
 * into a Matrix3x3 representation.
 * @param mat is an Eigen 3x3 matrix.
 * @return It returns a Matrix3x3 with values from mat.
 */
Matrix3x3 MatrixConvert(Eigen::Matrix3d &mat)
{
    Matrix3x3 mtx;
    mtx.data[0] = mat(0, 0);
    mtx.data[1] = mat(0, 1);
    mtx.data[2] = mat(0, 2);

    mtx.data[3] = mat(1, 0);
    mtx.data[4] = mat(1, 1);
    mtx.data[5] = mat(1, 2);

    mtx.data[6] = mat(2, 0);
    mtx.data[7] = mat(2, 1);
    mtx.data[8] = mat(2, 2);

    return mtx;
}

/**
 * @brief getLinearArray
 * @param mat
 * @return
 */
float *getLinearArrayFromMatrix(Eigen::Matrix3d &mat)
{
    int n = mat.cols() * mat.rows();

    float *ret = new float[n];
    int c = 0;
    for(int i = 0; i < mat.rows(); i++) {
        for(int j = 0; j < mat.cols(); j++) {
            ret[c] = mat(i, j);
            c++;
        }
    }

    return ret;
}

/**
 * @brief getLinearArray
 * @param mat
 * @return
 */
float *getLinearArrayFromMatrix(Eigen::Matrix3f &mat)
{
    int n = mat.cols() * mat.rows();

    float *ret = new float[n];
    int c = 0;
    for(int i = 0; i < mat.rows(); i++) {
        for(int j = 0; j < mat.cols(); j++) {
            ret[c] = mat(i, j);
            c++;
        }
    }

    return ret;
}

/**
 * @brief getMatrixFromLinearArray
 * @param array
 * @param rows
 * @param cols
 * @return
 */
Eigen::MatrixXf getMatrixfFromLinearArray(float *array, int rows, int cols)
{
    Eigen::MatrixXf ret = Eigen::MatrixXf(rows, cols);

    int c = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            ret(i, j) = array[c];
            c++;
        }
    }
    return ret;
}

/**
 * @brief getMatrixFromLinearArray
 * @param array
 * @param rows
 * @param cols
 * @return
 */
Eigen::MatrixXd getMatrixdFromLinearArray(float *array, int rows, int cols)
{
    Eigen::MatrixXd ret = Eigen::MatrixXd(rows, cols);

    int c = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            ret(i, j) = array[c];
            c++;
        }
    }
    return ret;
}

/**
 * @brief MatrixConvert
 * @param mat
 * @return
 */
Eigen::Matrix3f MatrixConvert(Matrix3x3 &mat)
{
    Eigen::Matrix3f mtx;
    mtx(0, 0) = mat.data[0];
    mtx(0, 1) = mat.data[1];
    mtx(0, 2) = mat.data[2];

    mtx(1, 0) = mat.data[3];
    mtx(1, 1) = mat.data[4];
    mtx(1, 2) = mat.data[5];

    mtx(2, 0) = mat.data[6];
    mtx(2, 1) = mat.data[7];
    mtx(2, 2) = mat.data[8];

    return mtx;
}



#endif

}

#endif // PIC_EIGEN_UTIL
