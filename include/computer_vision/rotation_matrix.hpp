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

#ifndef PIC_COMPUTER_VISION_ROTATION_MATRIX_HPP
#define PIC_COMPUTER_VISION_ROTATION_MATRIX_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../image.hpp"

#include "../util/math.hpp"

#include "../util/eigen_util.hpp"

#include "../computer_vision/nelder_mead_opt_triangulation.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief computeSkewMatrix computes a skew 3x3 matrix from a 3D vector v.
 * @param v is 3D vector.
 * @return It returns a skew 3x3 matrix.
 */
PIC_INLINE Eigen::Vector3d computeSkewMatrix(Eigen::Vecotr3d &v)
{
    Eigen::Matrix3d m;
    
    m(0, 0) =   0.0;
    m(1, 0) = -v[2];
    m(2, 0) =  v[1];

    m(0, 1) =  v[2];
    m(1, 1) =   0.0;
    m(2, 1) = -v[0];

    m(0, 2) = -v[1];
    m(1, 2) =  v[0];
    m(2, 2) =   0.0;

    return m;
}

/**
 * @brief getMatrixFromAxisRotation computes Rodriguez 3x3 rotation matrix from an axis and an angle.
 * @param axis is is 3D vector.
 * @param theta is the angle of rotation around axis.
 * @return It returns a 3x3 rotation matrix.
 */
PIC_INLINE Eigen::Matrix3d getMatrixFromAxisRotation(Eigen::Vecotr3d &axis, double theta)
{
    Eigen::Matrix3d R;
    
    Eigen::Matrix3d I;
    I.setIdentity();
    
    double sinTheta = sin(theta);
    double cosTheta = cos(theta);
    
    auto N = computeSkewMatrix(axis);

    auto R = I + sinTheta * N + (1.0 - cosTheta) * (N * N);
    return R;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_ROTATION_MATRIX_HPP
