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

#include <vector>

#include "../base.hpp"

#include "../util/matrix_3_x_3.hpp"
#include "../util/vec.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/LU"
    #include "../externals/Eigen/Geometry"
#else
    #include <Eigen/LU>
    #include <Eigen/Geometry>
#endif

#endif

#ifndef PIC_EIGEN_UTIL
#define PIC_EIGEN_UTIL

#ifndef PIC_DISABLE_EIGEN
namespace Eigen {
    typedef Matrix<float,  3, 4> Matrix34f;
    typedef Matrix<double, 3, 4> Matrix34d;
}
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief readMatrix34dFromFile
 * @param nameFile
 * @return
 */
PIC_INLINE Eigen::Matrix34d readMatrix34dFromFile(std::string nameFile)
{
    Eigen::Matrix34d mat;

    FILE *file = fopen(nameFile.c_str(), "r");

    if(file == NULL) {
        return mat;
    }

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 4; j++) {
            float val;
            fscanf(file, "%f", &val);
            mat(i, j) = double(val);
        }
    }

    fclose(file);

    return mat;
}

/**
 * @brief writeMatrix34dToFile
 * @param nameFile
 * @param mat
 * @return
 */
PIC_INLINE bool writeMatrix34dToFile(std::string nameFile, Eigen::Matrix34d &mat)
{
    FILE *file = fopen(nameFile.c_str(), "w");

    if(file == NULL) {
        return false;
    }

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 4; j++) {
            fprintf(file, "%f ", mat(i, j));
        }

        if(i < 2) {
            fprintf(file, "\n");
        }
    }

    fclose(file);

    return true;
}

/**
 * @brief DiagonalMatrix creates a diagonal matrix.
 * @param D a vector of size 3.
 * @return It returns a diagonal matrix.
 */
PIC_INLINE Eigen::Matrix3d DiagonalMatrix(Eigen::Vector3d D)
{
    Eigen::Matrix3d ret;

    ret.setZero();
    ret(0, 0) = D[0];
    ret(1, 1) = D[1];
    ret(2, 2) = D[2];

    return ret;
}

/**
 * @brief getDiagonalFromMatrix
 * @param mat
 * @return
 */
PIC_INLINE Eigen::Vector3d getDiagonalFromMatrix(Eigen::Matrix3d &mat)
{
    Eigen::Vector3d D;

    D[0] = mat(0, 0);
    D[1] = mat(1, 1);
    D[2] = mat(2, 2);

    return D;
}

/**
 * @brief getSquareMatrix
 * @param mat
 * @return
 */
PIC_INLINE Eigen::Matrix3d getSquareMatrix(Eigen::Matrix34d &mat)
{
    Eigen::Matrix3d ret;
    ret(0, 0) = mat(0, 0);
    ret(0, 1) = mat(0, 1);
    ret(0, 2) = mat(0, 2);

    ret(1, 0) = mat(1, 0);
    ret(1, 1) = mat(1, 1);
    ret(1, 2) = mat(1, 2);

    ret(2, 0) = mat(2, 0);
    ret(2, 1) = mat(2, 1);
    ret(2, 2) = mat(2, 2);

    return ret;
}

/**
 * @brief getLastColumn
 * @param mat
 * @return
 */
PIC_INLINE Eigen::Vector3d getLastColumn(Eigen::Matrix34d &mat)
{
    Eigen::Vector3d ret;

    ret[0] = mat(0, 3);
    ret[1] = mat(1, 3);
    ret[2] = mat(2, 3);

    return ret;
}

/**
 * @brief addOne
 * @param x
 * @return
 */
PIC_INLINE Eigen::Vector3f addOne(Eigen::Vector2f &x)
{
    return Eigen::Vector3f(x[0], x[1], 1.0f);
}

/**
 * @brief addOne
 * @param x
 * @return
 */
PIC_INLINE Eigen::Vector3d addOne(Eigen::Vector2d &x)
{
    return Eigen::Vector3d(x[0], x[1], 1.0);
}

/**
 * @brief addOne
 * @param x
 * @return
 */
PIC_INLINE Eigen::Vector4d addOne(Eigen::Vector3d &x)
{
    return Eigen::Vector4d(x[0], x[1], x[2], 1.0);
}

/**
 * @brief printfVet3d
 * @param x
 */
PIC_INLINE void printfVet3d(Eigen::Vector3d &x)
{
    printf("%f %f %f\n", x[0], x[1], x[2]);
}


/**
 * @brief printf
 * @param mat
 */
PIC_INLINE void printfMat(Eigen::MatrixXd mat)
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
PIC_INLINE void printfMat(Eigen::Matrix3f &mat)
{
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf("%f ", mat(i, j));
        }
        printf("\n");
    }
}
    
/**
  * @brief printf
  * @param mat
  */
PIC_INLINE void printfMat34d(Eigen::Matrix34d &mat)
{
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 4; j++){
            printf("%.9f ", mat(i, j));
        }
        printf("\n");
    }
}

/**
 * @brief fprintf
 * @param mat
 */
PIC_INLINE void fprintfMat(Eigen::MatrixXd &mat, std::string name)
{
    FILE *file = fopen(name.c_str(), "w");
    for(int i = 0; i < mat.rows(); i++){
        for(int j = 0; j < mat.cols(); j++){
            fprintf(file, "%.9f ", mat(i, j));
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

/**
 * @brief printf
 * @param mat
 */
PIC_INLINE void printfMat(Eigen::Matrix3d &mat)
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
PIC_INLINE Eigen::Matrix3d getShiftScaleMatrix(Eigen::Vector3f &info)
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
PIC_INLINE Eigen::Matrix3d CrossProduct(Eigen::Vector3d &t)
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
PIC_INLINE Eigen::Vector3d rigidTransform(Eigen::Vector3d &point, Eigen::Matrix3d &R, Eigen::Vector3d &t)
{
    return R * point + t;
}

/**
 * @brief RotationMatrixRefinement
 * @param R
 * @return
 */
PIC_INLINE Eigen::Matrix3d RotationMatrixRefinement(Eigen::Matrix3d &R)
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
PIC_INLINE Matrix3x3 MatrixConvert(Eigen::Matrix3f &mat)
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
PIC_INLINE Matrix3x3 MatrixConvert(Eigen::Matrix3d &mat)
{
    Matrix3x3 mtx;
    mtx.data[0] = float(mat(0, 0));
    mtx.data[1] = float(mat(0, 1));
    mtx.data[2] = float(mat(0, 2));

    mtx.data[3] = float(mat(1, 0));
    mtx.data[4] = float(mat(1, 1));
    mtx.data[5] = float(mat(1, 2));

    mtx.data[6] = float(mat(2, 0));
    mtx.data[7] = float(mat(2, 1));
    mtx.data[8] = float(mat(2, 2));

    return mtx;
}

/**
 * @brief getLinearArray
 * @param mat
 * @return
 */
PIC_INLINE float *getLinearArrayFromMatrix(Eigen::Matrix3d &mat)
{
    int n = int(mat.cols() * mat.rows());

    float *ret = new float[n];
    int c = 0;
    for(int i = 0; i < mat.rows(); i++) {
        for(int j = 0; j < mat.cols(); j++) {
            ret[c] = float(mat(i, j));
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
PIC_INLINE float *getLinearArrayFromMatrix(Eigen::Matrix3f &mat)
{
    int n = int(mat.cols() * mat.rows());

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
PIC_INLINE Eigen::MatrixXf getMatrixfFromLinearArray(float *array, int rows, int cols)
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
PIC_INLINE Eigen::MatrixXd getMatrixdFromLinearArray(float *array, int rows, int cols)
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
 * @brief getMatrix3dFromLinearArray
 * @param array
 * @return
 */
PIC_INLINE Eigen::Matrix3d getMatrix3dFromLinearArray(float *array)
{
    Eigen::Matrix3d ret;

    int c = 0;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
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
PIC_INLINE Eigen::Matrix3f MatrixConvert(Matrix3x3 &mat)
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

/**
 * @brief ComputeNormalizationTransform
 * @param points
 * @return
 */
PIC_INLINE Eigen::Vector3f ComputeNormalizationTransform(std::vector< Eigen::Vector2f > &points)
{
    Eigen::Vector3f ret;

    if(points.size() < 2) {
        return ret;
    }

    ret[0] = 0.0f;
    ret[1] = 0.0f;

    for(unsigned int i = 0; i < points.size(); i++) {
        ret[0] += points[i][0];
        ret[1] += points[i][1];
    }

    float n = float(points.size());
    ret[0] /= n;
    ret[1] /= n;

    ret[2] = 0.0;
    for(unsigned int i = 0; i < points.size(); i++) {

        float dx = points[i][0] - ret[0];
        float dy = points[i][1] - ret[1];

        ret[2] += sqrtf(dx * dx + dy * dy);
    }

    ret[2] = ret[2] / n / sqrtf(2.0f);

    return ret;
}

/**
 * @brief convertFromEigenToVec
 * @param x
 */
PIC_INLINE Vec2i convertFromEigenToVec(Eigen::Vector2i &x)
{
    return Vec2i(x[0], x[1]);
}

#endif

}

#endif // PIC_EIGEN_UTIL
