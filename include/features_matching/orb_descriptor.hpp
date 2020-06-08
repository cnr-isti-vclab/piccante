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

#ifndef PIC_FEATURES_MATCHING_ORB_DESCRIPTOR_HPP
#define PIC_FEATURES_MATCHING_ORB_DESCRIPTOR_HPP

#include <vector>

#include "../base.hpp"
#include "../image.hpp"
#include "../features_matching/brief_descriptor.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The ORBDescriptor class
 */
class ORBDescriptor: public BRIEFDescriptor
{
protected:
    std::vector< int* > x_theta, y_theta;

    /**
     * @brief rotate2D
     * @param x
     * @param y
     * @param x_out
     * @param y_out
     * @param angle
     */
    void rotate2D(int x, int y, int &x_out, int &y_out, float angle)
    {
        float cosAng = cosf(angle);
        float sinAng = sinf(angle);

        float x_f = float(x);
        float y_f = float(y);

        x_out = int(x_f * cosAng - y_f * sinAng);
        y_out = int(x_f * sinAng + y_f * cosAng);
    }

    /**
     * @brief rotateSamples rotates x and y samples
     */
    void rotateSamples()
    {
        uint n2 = n * 2;

        for(uint i = 0; i < 360; i += 12) {
            int * X_r = new int[n2];
            int * Y_r = new int[n2];

            float ang = (float(i) * C_PI_2) / 360.0f;

            for(uint j = 0; j < n2; j += 2) {
                rotate2D(x[j], x[j + 1], X_r[j], X_r[j + 1], ang);
                rotate2D(y[j], y[j + 1], Y_r[j], Y_r[j + 1], ang);
            }

            x_theta.push_back(X_r);
            y_theta.push_back(Y_r);
        }
    }

    int halfS;

public:

    /**
     * @brief ORBDescriptor
     * @param S
     * @param n
     */
    ORBDescriptor(int S = 31, int n = 256, unsigned int seed = 1)
    {
        m = new std::mt19937(seed);

        this->S = S;
        this->halfS = S >> 1;
        this->sigma_sq = float(S * S) / 25.0f;
        this->sigma_sq_2 = 2.0f * this->sigma_sq;

        generateSamples(n);
        rotateSamples();
    }

    ~ORBDescriptor()
    {
        release();
    }

    /**
     * @brief get computes a descriptor at position (x0,y0) with size n.
     * @param img
     * @param x0
     * @param y0
     * @param desc
     * @return
     */
    uint *get(Image *img, int x0, int y0, uint* desc = NULL)
    {
        if(img == NULL){
            return NULL;
        }

        if(!img->checkCoordinates(x0, y0)) {
            return NULL;
        }

        float grad[2];

        img->getMomentsVal(x0, y0, S, grad);

        float theta = atan2f(grad[1], grad[0]);

        if(theta < 0.0f) {
            theta = CLAMPi(C_PI_2 + theta, 0.0f, C_PI_2);
        }

        //float theta_nor = CLAMPi(theta / C_PI_2, 0.0f, 1.0f);

        uint theta_nor = CLAMPi(uint(theta * 255.0f / C_PI_2), 0, 255);

        uint n = x_theta.size() - 1;

        uint index = (n * theta_nor) >> 8;

        return getAux(img, x0, y0, x_theta[index], y_theta[index], desc);
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_ORB_DESCRIPTOR_HPP */

