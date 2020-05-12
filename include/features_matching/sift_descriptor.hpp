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

#ifndef PIC_SIFT_DESCRIPTOR_HPP
#define PIC_SIFT_DESCRIPTOR_HPP

#include "../image.hpp"
#include "../util/array.hpp"

namespace pic {

class SIFTDescriptor
{
protected:
    float thr_weak, sigma, sigma_sq_2;

    float *reference_angles;
    float sector_angle, nBinf;

    float reference_angles_orientation[36];
    float sector_angle_orientation;

    int patchSize, half_patchSize, subPatchSize, subPatchSize_sq, nBin, tot;

public:

    SIFTDescriptor(float thr_weak = 0.01f, int patchSize = 16, int subPatchSize = 4, int nBin = 8)
    {
        reference_angles = NULL;
        update(thr_weak, patchSize, subPatchSize, nBin);
    }

    /**
     * @brief update
     * @param thr_weak
     * @param patchSize
     * @param subPatchSize
     * @param nBin
     */
    void update(float thr_weak = 0.05f, int patchSize = 16, int subPatchSize = 4, int nBin = 8)
    {
        thr_weak = MAX(thr_weak, 0.05f);
        nBin = MAX(nBin, 4);

        this->thr_weak = thr_weak;
        this->nBin = nBin;

        half_patchSize = patchSize >> 1;
        this->patchSize = (half_patchSize << 1) + 1;
        this->subPatchSize = subPatchSize;

        subPatchSize_sq = subPatchSize *  subPatchSize;
        tot = subPatchSize_sq * nBin;

        sigma = float(patchSize) * 1.5f;
        sigma_sq_2 = sigma * sigma * 2.0f;

        reference_angles = delete_vec_s(reference_angles);
        reference_angles = new float[nBin];
        nBinf = float(nBin);

        for(int i = 0; i < nBin; i++) {
            reference_angles[i] = (float(i) * C_PI_2) / nBinf;
        }
        sector_angle = C_PI_2 / float(nBin);

        for(int i = 0; i < 36; i++) {
            reference_angles_orientation[i] = (float(i) * C_PI_2) / 36.0f;
        }
        sector_angle_orientation = C_PI_2 / 36.0f;

    }

    /**
     * @brief getDescriptorSize returns the descriptor size.
     * @return the descriptor size.
     */
    int getDescriptorSize()
    {
        return tot;
    }

    /**
     * @brief computePatchOrientation
     * @param imgGrad
     * @param x0
     * @param y0
     * @return
     */
    float computePatchOrientation(Image *imgGrad, int x0, int y0)
    {
        float orientation[36];
        Arrayf::assign(0.0f, orientation, 36);

        int nBin = 36;
        float nBinf = float(nBin);

        for(int i = 0; i < patchSize; i ++) {
            int y_local = i - half_patchSize;
            int y = y_local + y0;

            for(int j = 0; j < patchSize; j ++) {
                int x_local = j  - half_patchSize;
                int x = x_local + x0;

                float *grad = (*imgGrad)(x, y);

                //compute current (x,y) angle
                float angle = atan2f(grad[1], grad[0]);
                angle = (angle >= 0.0f) ? angle : angle + C_PI_2;
                angle = CLAMPi(angle, 0.0f, C_PI_2);

                //place it in the bin
                float index_f = nBinf * (angle / C_PI_2);
                int index = int(floorf(index_f));
                int index_1 = (index + 1) % nBin;

                float dist = (angle - reference_angles_orientation[index]) / sector_angle_orientation;
                dist = CLAMPi(dist, 0.0f, 1.0f);

                int squared = x_local * x_local + y_local * y_local;

                float mag = grad[2] * expf(-float(squared) / sigma_sq_2);

                orientation[index]   += mag * (1.0f - dist);
                orientation[index_1] += mag * dist;
            }
        }

        int index;
        Arrayf::getMax(orientation, 36, index);

        return reference_angles_orientation[index];
    }

    /**
     * @brief get computes a descriptor at position (x0,y0) with size n for a given image gradient imgGrad.
     * @param img is the gradient of the input image
     * @param x0 is the x-coordinate where to compute the descriptor
     * @param y0 is the y-coordinate where to compute the descriptor
     * @param desc is the output descriptor, if it is NULL, memory will be allocated
     * @return it returns a pointer to the descriptor
     */
    float *get(Image *imgGrad, int x0, int y0, float *desc = NULL)
    {
        if(imgGrad == NULL) {
            return desc;
        }

        if(desc == NULL) {
            desc = new float[tot];
        }

        memset(desc, 0, sizeof(float) * tot);

        float kp_angle = computePatchOrientation(imgGrad, x0, y0);

        float cosAngle = cosf(-kp_angle);
        float sinAngle = sinf(-kp_angle);

        //compute the descriptor
        int counter = 0;

        for(int i = 0; i < patchSize; i += subPatchSize) {
            int tmp_y = y0 + i - half_patchSize;

            for(int j = 0; j < patchSize; j += subPatchSize) {
                int tmp_x = x0 + j - half_patchSize;

                for(int k = 0; k < subPatchSize; k++) {
                    int y = tmp_y + k;

                    for(int l = 0; l < subPatchSize; l++) {
                        int x = tmp_x + l;

                        float *grad = (*imgGrad)(x, y);

                        if(grad[2] <= thr_weak) {
                            continue;
                        }

                        //rotate gradients according to the patch's main orientation
                        float r_gx = grad[0] * cosAngle - grad[1] * sinAngle;
                        float r_gy = grad[0] * sinAngle + grad[1] * cosAngle;

                        float angle = atan2f(r_gy, r_gx);
                        angle = (angle >= 0.0f) ? angle : angle + C_PI_2;
                        angle = CLAMPi(angle, 0.0f, C_PI_2);

                        //find the bin
                        float index_f = nBinf * (angle / C_PI_2);
                        int index = int(floorf(index_f));
                        int index_1 = (index + 1) % nBin;

                        float dist = (angle - reference_angles[index]) / sector_angle;
                        dist = CLAMPi(dist, 0.0f, 1.0f);

                        int y_local = i + k - half_patchSize;
                        int x_local = j + l - half_patchSize;
                        int squared = x_local * x_local + y_local * y_local;

                        float mag = grad[2] * expf(-float(squared) / sigma_sq_2);

                        desc[counter + index]   += mag * (1.0f - dist);
                        desc[counter + index_1] += mag * dist;
                   }

                }

                counter += nBin;
            }
        }

        //normalize desc
        Array<float>::normalize(desc, tot);

        //remove strong edges; i.e., over 0.2
        for(int i = 0; i < tot; i++) {
            desc[i] = desc[i] > 0.2f ? desc[i] : 0.2f;
        }

        //re-normalize desc
        Array<float>::normalize(desc, tot);

        return desc;
    }

    /***match: matches two descriptors*/
    static float match(float *fv0, float *fv1, int nfv)
    {
        if(fv0 == NULL || fv1 == NULL || nfv < 1) {
            return -1.0f;
        }

        return Array<float>::distanceSq(fv0, fv1, nfv);
    }
};

} // end namespace NSF

#endif /* NSF_SIFT_DESCRIPTOR_HPP */

