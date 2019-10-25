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

    //the rotation matrix of (theta, phi)
    Eigen::Matrix3f mtxRot;

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

                auto Rd = (mtxRot * d).normalized();

                float xt = 1.0f - ((atan2f(Rd[2], -Rd[0]) * C_INV_PI) * 0.5f + 0.5f);
                float yt = (acosf(Rd[1]) * C_INV_PI);

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
     * @brief FilterRotation
     * @param theta
     * @param phi
     */
    FilterRotation(Eigen::Matrix3f mtxRot) : Filter()
    {
        update(mtxRot);
    }

    /**
     * @brief update
     * @param theta
     * @param phi
     */
    void update(float theta, float phi)
    {
        this->phi = phi;
        this->theta = theta;

        Eigen::Matrix3f mtx;
        mtx = Eigen::AngleAxisf(phi,   Eigen::Vector3f::UnitY());
        std::cout<< mtx;
                //Eigen::AngleAxisf(theta, Eigen::Vector3f::UnitX());// *
             // Eigen::AngleAxisf(phi,   Eigen::Vector3f::UnitY()) *
             // Eigen::AngleAxisf(0.0f,  Eigen::Vector3f::UnitZ());

        update(mtx);
    }

    /**
     * @brief update
     * @param theta
     * @param phi
     */
    void update(Eigen::Matrix3f mtx)
    {
        this->mtxRot = Eigen::Transpose< Eigen::Matrix3f >(mtx);
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

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param mtxRot
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, Eigen::Matrix3f &mtxRot)
    {
        FilterRotation fltRot(mtxRot);
        return fltRot.Process(Single(imgIn), imgOut);
    }
};

#endif

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ROTATION_HPP */

