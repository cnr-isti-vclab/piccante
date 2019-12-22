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
    float angleX, angleY, angleZ;

    //the rotation matrix of (theta, phi)
    Eigen::Matrix3f mtxRot, mtxRot_inv;

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

                auto Rd = (mtxRot_inv * d).normalized();


          /*    printf("\nwrong: %f correct: %f Rd: %f\n",
                       sinTheta * cosf(phi + this->phi),
                       sinTheta * cosf(phi - this->phi),
                       Rd[0]
                       );*/

                float xt = 1.0f - ((atan2f(Rd[2], -Rd[0]) * C_INV_PI) * 0.5f + 0.5f);
                float yt = (acosf(Rd[1]) * C_INV_PI);

                float *data_dst = (*dst)(i, j);
                isb.SampleImage(src[0], xt, yt, data_dst);
            }
        }
    }

    /**
     * @brief fromAnglesToVector
     * @param theta
     * @param phi
     * @return
     */
    Eigen::Vector3f fromAnglesToVector(float theta, float phi)
    {
        Eigen::Vector3f ret;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        ret[0] = sinTheta * cosf(phi);
        ret[1] = cosTheta;
        ret[2] = sinTheta * sinf(phi);

        return ret;
    }

public:

    /**
     * @brief FilterRotation
     */
    FilterRotation() : Filter()
    {
        update(0.0f, 0.0f, 0.0f);
    }

    /**
     * @brief FilterRotation
     * @param angleX
     * @param angleY
     * @param angleZ
     */
    FilterRotation(float angleX, float angleY, float angleZ) : Filter()
    {
        update(angleX, angleY, angleZ);
    }

    /**
     * @brief FilterRotation
     * @param mtx
     */
    FilterRotation(Eigen::Matrix3f mtx) : Filter()
    {
        update(mtx);
    }

    /**
     * @brief update
     * @param angleX
     * @param angleY
     * @param angleZ
     */
    void update(float angleX, float angleY, float angleZ)
    {
        this->angleX = angleX;
        this->angleY = angleY;
        this->angleZ = angleZ;

        Eigen::Matrix3f mtx;
        mtx = Eigen::AngleAxisf(angleZ, Eigen::Vector3f::UnitZ()) *
              Eigen::AngleAxisf(angleY, Eigen::Vector3f::UnitY()) *
              Eigen::AngleAxisf(angleX, Eigen::Vector3f::UnitX());

        update(mtx);
    }

    /**
     * @param theta
     * @brief update
     * @param phi
     */
    void update(Eigen::Matrix3f mtx)
    {
        this->mtxRot = mtx;
        this->mtxRot_inv = Eigen::Transpose< Eigen::Matrix3f >(mtx);
    }

    /**
     * @brief getMtxRot
     * @return
     */
    Eigen::Matrix3f getMtxRot()
    {
        return mtxRot;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param theta
     * @param phi
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float angleX, float angleY, float angleZ)
    {
        FilterRotation fltRot(angleX, angleY, angleZ);
        return fltRot.Process(Single(imgIn), imgOut);
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param mtx
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, Eigen::Matrix3f &mtx)
    {
        FilterRotation fltRot(mtx);
        return fltRot.Process(Single(imgIn), imgOut);
    }
};

#endif

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ROTATION_HPP */

