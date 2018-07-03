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

#ifndef PIC_COMPUTER_VISION_INTRISICS_MATRIX_HPP
#define PIC_COMPUTER_VISION_INTRISICS_MATRIX_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../util/math.hpp"

#ifndef PIC_DISABLE_EIGEN

    #ifndef PIC_EIGEN_NOT_BUNDLED
        #include "../externals/Eigen/Dense"
        #include "../externals/Eigen/Geometry"
    #else
        #include <Eigen/Dense>
        #include <Eigen/Geometry>
    #endif

#endif

namespace pic {

/**
 * @brief getFocalLengthFromFOVAngle
 * @param fovy is an angle in radians.
 * @return
 */
PIC_INLINE double getFocalLengthFromFOVAngle(double fovy)
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
PIC_INLINE double getFOVAngleFromFocalSensor(double f, double x, double y)
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
PIC_INLINE double getFocalLengthPixels(double focal_length_mm, double sensor_size_mm, double sensor_size_px)
{
    return (focal_length_mm * sensor_size_px) / sensor_size_mm;
}

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief getIntrinsicsMatrix
 * @param focal_length
 * @param m_x
 * @param m_y
 * @param opitical_center_x
 * @param opitical_center_y
 * @return
 */
PIC_INLINE Eigen::Matrix3d getIntrinsicsMatrix(double focal_length, double m_x, double m_y, double opitical_center_x, double opitical_center_y)
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
PIC_INLINE Eigen::Matrix3d getIntrinsicsMatrix(double focal_length_x, double focal_length_y, double opitical_center_x, double opitical_center_y)
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
PIC_INLINE Eigen::Vector2d removeLensDistortion(Eigen::Vector2d &p, double k[5])
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

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_INTRISICS_MATRIX_HPP
