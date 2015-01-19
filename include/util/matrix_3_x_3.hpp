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

#ifndef PIC_UTIL_MATRIX_3_X_3_HPP
#define PIC_UTIL_MATRIX_3_X_3_HPP

#include <math.h>
#include <algorithm>

namespace pic {

/**
 * @brief The Matrix3x3 class provides methods for managing a 3 by 3 matrix.
 */
class Matrix3x3
{
public:
    /*
     * 0 1 2
     * 3 4 5
     * 6 7 8
    */
    float data[9];

    /**
     * @brief Matrix3x3
     */
    Matrix3x3()
    {
        for(int i = 0; i < 9; i++) {
            data[i] = 0.0f;
        }
    }

    /**
     * @brief Matrix3x3
     * @param data
     */
    Matrix3x3(float *data)
    {
        Set(data);
    }

    /**
     * @brief Clone clones the matrix.
     * @return it returns the cloned matrix.
     */
    Matrix3x3 Clone()
    {
        Matrix3x3 ret(data);

        return ret;
    }

    /**
     * @brief Set sets the matrix up.
     * @param data input data, they are assumed to be 9 floats.
     * The matrix is stored by rows.
     */
    void Set(float *data)
    {
        if(data != NULL) {
            memcpy(this->data, data, 9 * sizeof(float));
        }
    }

    /**
     * @brief Identity sets the matrix as an identity matrix; diag(1, 1, 1);
     */
    void Identity()
    {
        data[0] = 1.0f;
        data[1] = 0.0f;
        data[2] = 0.0f;

        data[3] = 0.0f;
        data[4] = 1.0f;
        data[5] = 0.0f;

        data[6] = 0.0f;
        data[7] = 0.0f;
        data[8] = 1.0f;
    }

    /**
     * @brief Mul
     * @param mtx
     * @return
     */
    Matrix3x3 Mul(const Matrix3x3 &mtx)
    {
        Matrix3x3 ret;
        ret.data[0] = data[0] * mtx.data[0] +  data[1] * mtx.data[3] + data[2] * mtx.data[6];
        ret.data[1] = data[0] * mtx.data[1] +  data[1] * mtx.data[4] + data[2] * mtx.data[7];
        ret.data[2] = data[0] * mtx.data[2] +  data[1] * mtx.data[6] + data[2] * mtx.data[8];

        ret.data[3] = data[3] * mtx.data[0] +  data[4] * mtx.data[3] + data[5] * mtx.data[6];
        ret.data[4] = data[3] * mtx.data[1] +  data[4] * mtx.data[4] + data[5] * mtx.data[7];
        ret.data[5] = data[3] * mtx.data[2] +  data[4] * mtx.data[6] + data[5] * mtx.data[8];

        ret.data[6] = data[6] * mtx.data[0] +  data[7] * mtx.data[3] + data[8] * mtx.data[6];
        ret.data[7] = data[6] * mtx.data[1] +  data[7] * mtx.data[4] + data[8] * mtx.data[7];
        ret.data[8] = data[6] * mtx.data[2] +  data[7] * mtx.data[6] + data[8] * mtx.data[8];

        return ret;
    }

    /**
     * @brief Mul
     * @param vec
     * @param ret
     * @return
     */
    float *Mul(float *vec, float *ret)
    {
        if(vec == NULL) {
            return ret;
        }

        if(ret == NULL) {
            ret = new float[3];
        }

        ret[0] = data[0] * vec[0] + data[1] * vec[1] + data[2] * vec[2];
        ret[1] = data[3] * vec[0] + data[4] * vec[1] + data[5] * vec[2];
        ret[2] = data[6] * vec[0] + data[7] * vec[1] + data[8] * vec[2];

        return ret;
    }

    /**
     * @brief MulH
     * @param vec
     * @param ret
     * @return
     */
    float *MulH(float *vec, float *ret)
    {
        if(vec == NULL) {
            return ret;
        }

        if(ret == NULL) {
            ret = new float[3];
        }

        ret[0] = data[0] * vec[0] + data[1] * vec[1] + data[2] * vec[2];
        ret[1] = data[3] * vec[0] + data[4] * vec[1] + data[5] * vec[2];
        ret[2] = data[6] * vec[0] + data[7] * vec[1] + data[8] * vec[2];

        return ret;
    }

    /**
     * @brief Projection
     * @param vec
     * @param ret
     * @return
     */
    float *Projection(float *vec, float *ret) {
        if(vec == NULL) {
            return ret;
        }

        if(ret == NULL) {
            ret = new float[2];
        }

        ret[0]      = data[0] * vec[0] + data[1] * vec[1] + data[2];
        ret[1]      = data[3] * vec[0] + data[4] * vec[1] + data[5];
        float ret_2 = data[6] * vec[0] + data[7] * vec[1] + data[8];

        if(ret_2 > 0.0f) {
            ret[0] /= ret_2;
            ret[1] /= ret_2;
        }

        return ret;
    }

    /**
     * @brief CrossProduct computes the cross product matrix
     * @param t is a three value array.
     */
    void CrossProduct(float *t)
    {
        if(t == NULL) {
            return;
        }

        data[0] =  0.0f;
        data[1] = -t[2];
        data[2] =  t[1];

        data[3] =  t[2];
        data[4] =  0.0f;
        data[5] = -t[0];

        data[6] = -t[1];
        data[7] =  t[0];
        data[8] =  0.0f;
    }

    /**
     * @brief Add adds a value to the diagonal.
     * @param value is the value to be added.
     */
    void Add(float value)
    {
        data[0] += value;
        data[4] += value;
        data[8] += value;
    }

    /**
     * @brief Determinant computes the determinant of the matrix.
     * @return
     */
    float Determinant()
    {
        return	 data[0] * (data[4] * data[8] - data[5] * data[7]) -
                 data[1] * (data[8] * data[3] - data[5] * data[6]) +
                 data[2] * (data[3] * data[7] - data[4] * data[6]);
    }

    /**
     * @brief Inverse computes the inverse of the matrix.
     * @param inv
     * @return
     */
    Matrix3x3 *Inverse(Matrix3x3 *inv)
    {
        if(inv == NULL) {
            inv = new Matrix3x3();
        }

        float det = Determinant();

        if(fabsf(det) <= 1e-9f) {
            printf("Error: Negative determinant\n");
            return inv;
        }

        inv->data[0] =  (data[4] * data[8] - data[5] * data[7]) / det;
        inv->data[1] = -(data[1] * data[8] - data[2] * data[7]) / det;
        inv->data[2] =  (data[1] * data[5] - data[2] * data[4]) / det;

        inv->data[3] = -(data[3] * data[8] - data[5] * data[6]) / det;
        inv->data[4] =  (data[0] * data[8] - data[2] * data[6]) / det;
        inv->data[5] = -(data[0] * data[5] - data[2] * data[3]) / det;

        inv->data[6] =  (data[3] * data[7] - data[4] * data[6]) / det;
        inv->data[7] = -(data[0] * data[7] - data[1] * data[6]) / det;
        inv->data[8] =  (data[0] * data[4] - data[1] * data[3]) / det;

        return inv;
    }

    //
    /**
     * @brief SetTranslationMatrix sets the matrix as a translation matrix.
     * @param tx
     * @param ty
     */
    void SetTranslationMatrix(float tx, float ty) {
        data[0] = 1.0f;
        data[1] = 0.0f;
        data[2] = tx;

        data[3] = 0.0f;
        data[4] = 1.0f;
        data[5] = ty;

        data[6] = 0.0f;
        data[7] = 0.0f;
        data[8] = 1.0f;
    }

    /**
     * @brief SetRotationMatrix sets the matrix as a rotation matrix
     * @param ang
     */
    void SetRotationMatrix(float ang) {
        float cosAng = cosf(ang);
        float sinAng = sinf(ang);

        data[0] = cosAng;
        data[1] = -sinAng;
        data[2] = 0.0f;

        data[3] = sinAng;
        data[4] = cosAng;
        data[5] = 0.0f;

        data[6] = 0.0f;
        data[7] = 0.0f;
        data[8] = 1.0f;
    }

    /**
     * @brief SetShearMatrix sets the matrix as a shear matrix.
     * @param horizontal_shear
     * @param vertical_shear
     */
    void SetShearMatrix(float horizontal_shear, float vertical_shear)
    {
        Identity();

        data[1] = vertical_shear;
        data[3] = horizontal_shear;
    }

    /**
     * @brief Transpose computes the transposed matrix.
     */
    void Transpose()
    {
        std::swap(data[1], data[3]);
        std::swap(data[5], data[7]);
        std::swap(data[2], data[6]);
    }

    /**
     * @brief SetScaleMatrix set the matrix as a scaling matrix.
     * @param x_scale
     * @param y_scale
     */
    void SetScaleMatrix(float x_scale, float y_scale)
    {
        if(x_scale <= 0.0f) {
            x_scale = 1.0f;
        }

        if(y_scale <= 0.0f) {
            y_scale = 1.0f;
        }

        Identity();

        data[0] = x_scale;
        data[4] = y_scale;
    }

    /**
     * @brief print
     */
    void print(){
        printf("\n");
        printf("%f %f %f\n", data[0], data[1], data[2]);
        printf("%f %f %f\n", data[3], data[4], data[5]);
        printf("%f %f %f\n", data[6], data[7], data[8]);
    }
};

} // end namespace pic

#endif // PIC_UTIL_MATRIX_3_X_3_HPP
