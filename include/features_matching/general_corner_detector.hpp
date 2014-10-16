/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_FEATURES_MATCHING_GENERAL_CORNER_DETECTOR_HPP
#define PIC_FEATURES_MATCHING_GENERAL_CORNER_DETECTOR_HPP

#include "util/vec.hpp"

#include "image.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class GeneralCornerDetector
{
protected:
    bool     bLum;
    Image *lum;

public:
    GeneralCornerDetector()
    {
        lum = NULL;
        bLum = false;
    }

    ~GeneralCornerDetector()
    {

    }

    /**
     * @brief Compute
     * @param img
     * @param corners
     */
    virtual void Compute(Image *img, std::vector< Eigen::Vector3f > *corners)
    {
    }

    /**
     * @brief getCornersImage
     * @param corners
     * @param imgOut
     * @param bColor
     * @return
     */
    Image *getCornersImage(std::vector< Eigen::Vector3f > *corners,
                              Image *imgOut = NULL, bool bColor = true)
    {
        if(corners == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            if(lum == NULL) {
                return imgOut;
            }

            imgOut = lum->AllocateSimilarOne();

        } else {
            if(!imgOut->SimilarType(lum)) {
                return imgOut;
            }
        }

        imgOut->SetZero();

        for(unsigned int i = 0; i < corners->size(); i++) {
            int x = int((*corners)[i][0]);
            int y = int((*corners)[i][1]);
            if(bColor) {
                (*imgOut)(x, y)[0] = 1.0f;
            } else {
                (*imgOut)(x, y)[0] = (*corners)[i][2];
            }
        }

        return imgOut;
    }


    /**
     * @brief NormalizePoints normalizes points coordinates to have
     * points' center in (0,0) and maximum length at sqrt(2).
     * @param points
     * @return It returns the scaling factor and the original center.
     */
    static Eigen::Vector3f NormalizePoints(std::vector< Eigen::Vector2f > &points)
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

        for(unsigned int i = 0; i < points.size(); i++) {
            points[i][0] -= ret[0];
            points[i][0] /= ret[2];

            points[i][1] -= ret[1];
            points[i][1] /= ret[2];
        }

        return ret;
    }

    /**
     * @brief Test
     * @param gcd
     */
    static void Test(GeneralCornerDetector *gcd)
    {
        if(gcd == NULL){
            return;
        }

        Image full_image(1, 512, 512, 3);
        full_image.SetZero();

        Image quad(1, 128, 128, 3);
        quad.Assign(1.0f);

        full_image.CopySubImage(&quad, 192, 192);

        std::vector< Eigen::Vector3f > corners;
        gcd->Compute(&full_image, &corners);

        printf("\n Corner Detector Test:\n");

        for(unsigned int i = 0; i < corners.size(); i++) {
            printf("X: %f Y: %f\n", corners[i][0], corners[i][1]);
        }

        printf("\n");

        Image *img_corners = gcd->getCornersImage(&corners);
        img_corners->Write("general_corner_test_image.hdr");
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_GENERAL_CORNER_DETECTOR_HPP */

