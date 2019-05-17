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
                                 float *white_point_XYZ = NULL)
{
    float *ret = new float[9];

    return ret;
}

/*

A_R = [R(1) R(2) R(3) 0 0 0 0 0 0; ...
       0 0 0 R(1) R(2) R(3) 0 0 0; ...
       0 0 0 0 0 0 R(1) R(2) R(3)];
b_R = [1; 0; 0];

A_G = [G(1) G(2) G(3) 0 0 0 0 0 0; ...
       0 0 0 G(1) G(2) G(3) 0 0 0; ...
       0 0 0 0 0 0 G(1) G(2) G(3)];
b_G = [0; 1; 0];

A_B = [B(1) B(2) B(3) 0 0 0 0 0 0; ...
       0 0 0 B(1) B(2) B(3) 0 0 0; ...
       0 0 0 0 0 0 B(1) B(2) B(3)];
b_B = [0; 0; 1];

A_Wp = [Wp(1) Wp(2) Wp(3) 0 0 0 0 0 0; ...
       0 0 0 Wp(1) Wp(2) Wp(3) 0 0 0; ...
       0 0 0 0 0 0 Wp(1) Wp(2) Wp(3)];

b_Wp = [1; 1; 1];

A = [A_R; A_G; A_B; A_Wp];
b = [b_R; b_G; b_B;  b_Wp];

mtx = A \ b;

mtx = reshape(mtx, 3, 3)';
        */

} // end namespace pic

#endif /* PIC_COLORS_MATRIX_FORM_PRIMARIES_HPP */

