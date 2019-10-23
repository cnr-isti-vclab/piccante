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
#else
    #include <Eigen/Dense>
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

    Eigen::Matrix3f matRot;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        for(int j = box->y0; j < box->y1; j++) {
            float theta = (C_PI   * float(j) ) / dst->heightf;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            Eigen::Vector3f d;

            for(int i = box->x0; i < box->x1; i++) {
                float phi   = (C_PI_2 * float(i)) / dst->widthf;

                d[0] = sinTheta * cosf(phi);
                d[1] = cosTheta;
                d[2] = sinTheta * sinf(phi);

                auto d_new = matRot * d;

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
     * @param phi
     * @param theta
     */
    FilterRotation(float phi, float theta) : Filter()
    {
        update(phi, theta);
    }

    /**
     * @brief update
     * @param type
     */
    void update(float phi, float theta)
    {
        this->phi = phi;
        this->theta = theta;
    }
};

#endif

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ROTATION_HPP */

