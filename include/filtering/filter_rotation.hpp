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

#ifndef PIC_FILTERING_FILTER_ROTATION_HPP
#define PIC_FILTERING_FILTER_ROTATION_HPP

#include "../filtering/filter.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"

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
#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The FilterRotation class
 */
class FilterRotation: public Filter
{
protected:

    ImageSamplerBilinear isb;

    //rotation
    float theta, phi;

    //inverse rotation matrix of (theta, phi)
    Eigen::Matrix3f matRot_inv;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        float c1 = C_PI   / dst->heightf;
        float c2 = C_PI_2 / dst->widthf;

        for(int j = box->y0; j < box->y1; j++) {
            float theta = float(j) * c1;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            Eigen::Vector3f d;

            for(int i = box->x0; i < box->x1; i++) {
                float phi = float(i) * c2;

                d[0] = sinTheta * cosf(phi);
                d[1] = cosTheta;
                d[2] = sinTheta * sinf(phi);

                auto d_new = (matRot_inv * d).normalized();

                float xt = 1.0f - ((atan2f(d_new[2], -d_new[0]) * C_INV_PI) * 0.5f + 0.5f);
                float yt = (acosf(d_new[1]) * C_INV_PI);

                float *data_dst = (*dst)(i, j);
                isb.SampleImage(src[0], xt, yt, data_dst);
            }
        }
    }

public:


    /**
     * @brief FilterRotation
     * @param theta
     * @param phi
     */
    FilterRotation(float theta, float phi) : Filter()
    {
        update(theta, phi);
    }

    /**
     * @brief update
     * @param type
     */
    void update(float theta, float phi)
    {
        this->phi = phi;
        this->theta = theta;

        Eigen::Matrix3f mat;
        mat = Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitX()) *
              Eigen::AngleAxisf(phi,   Eigen::Vector3f::UnitY()) *
              Eigen::AngleAxisf(0.0f,   Eigen::Vector3f::UnitZ());

        matRot_inv = mat.transpose();
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param theta
     * @param phi
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float theta, float phi)
    {
        FilterRotation fltRot(theta, phi);
        return fltRot.Process(Single(imgIn), imgOut);
    }
};

#endif

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ROTATION_HPP */

