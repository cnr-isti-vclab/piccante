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

#ifndef PIC_COLORS_MATRIX_FORM_PRIMARIES_HPP
#define PIC_COLORS_MATRIX_FORM_PRIMARIES_HPP

#include "../base.hpp"

#ifndef PIC_DISABLE_EIGEN
    #ifndef PIC_EIGEN_NOT_BUNDLED
        #include "../externals/Eigen/Dense"
        #include "../externals/Eigen/QR"
    #else
        #include <Eigen/Dense>
        #include <Eigen/QR>
    #endif
#endif

namespace pic {

/**
 * @brief createMatrixFromPrimaries computes a matrix for converting XYZ values into the
 * defined color space (i.e., by defining the three primaries: red, green, and blue).
 * @param red_XYZ is the XYZ values of the red primary
 * @param green_XYZ is the XYZ values of the green primary
 * @param blue_XYZ is the XYZ values of the blue primary
 * @param white_point_XYZ is the XYZ values of the white point primary
 * @return It returns a 3x3 matrix for converting XYZ values into the defined color space
 */
float *createMatrixFromPrimaries(float *red_XYZ,
                                 float *green_XYZ,
                                 float *blue_XYZ,
                                 float *white_point_XYZ,
                                 float *ret = NULL
                                 )
{
    if(red_XYZ == NULL || green_XYZ == NULL || blue_XYZ == NULL) {
        return ret;
    }

    if(ret == NULL) {
        ret = new float[9];
    }

#ifndef PIC_DISABLE_EIGEN

    int w = 0;
    if(white_point_XYZ != NULL) {
        w = 3;
    }

    //set up a liner system A x = b
    int nRow = 9 + w;
    Eigen::MatrixXf A(nRow, 9);
    Eigen::VectorXf b(nRow);

    //A matrix
    A.setZero();

    //red
    for(int j = 0; j < 3; j++) {
        for(int i = 0 ; i < 3; i++) {
            A(j, j * 3 + i) = red_XYZ[i];
        }
    }

    //green`
    for(int j = 0; j < 3; j++) {
        for(int i = 0 ; i < 3; i++) {
            A(j + 3, j * 3 + i) = green_XYZ[i];
        }
    }

    //blue`
    for(int j = 0; j < 3; j++) {
        for(int i = 0 ; i < 3; i++) {
            A(j + 6, j * 3 + i) = blue_XYZ[i];
        }
    }

    //white
    if(w == 3) {
        for(int j = 0; j < 3; j++) {
            for(int i = 0 ; i < 3; i++) {
                A(j + 9, j * 3 + i) = white_point_XYZ[i];
            }
        }
    }

    //b vector
    b(0) = 1.0f;
    b(1) = 0.0f;
    b(2) = 0.0f;

    b(3) = 0.0f;
    b(4) = 1.0f;
    b(5) = 0.0f;

    b(6) = 0.0f;
    b(7) = 0.0f;
    b(8) = 1.0f;

    if(w == 3) {
        b(9) = 1.0f;
        b(10) = 1.0f;
        b(11) = 1.0f;
    }

    //solve Ax=b
    Eigen::VectorXf x = A.colPivHouseholderQr().solve(b);

    for(int i = 0; i < 9; i++) {
        ret[i] = x(i);
    }
#endif
    return ret;
}

} // end namespace pic

#endif /* PIC_COLORS_MATRIX_FORM_PRIMARIES_HPP */

