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

#ifndef PIC_FEATURES_MATCHING_GENERAL_CORNER_DETECTOR_HPP
#define PIC_FEATURES_MATCHING_GENERAL_CORNER_DETECTOR_HPP

#include "image.hpp"

#ifndef PIC_DISABLE_EIGEN
#include "externals/Eigen/Dense"
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The GeneralCornerDetector class
 */
class GeneralCornerDetector
{
protected:
    bool     bLum;
    Image *lum;

public:
    /**
     * @brief GeneralCornerDetector
     */
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
        quad = 1.0f;

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

