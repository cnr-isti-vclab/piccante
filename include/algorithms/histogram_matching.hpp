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

#ifndef PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP
#define PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP

#include "../base.hpp"

#include "../image.hpp"
#include "../image_vec.hpp"
#include "../histogram.hpp"

#include "../util/std_util.hpp"

namespace pic {

class HistogramMatching
{
protected:
    int nBin;

public:

    /**
     * @brief HistogramMatching
     */
    HistogramMatching()
    {
        update(256);
    }

    /**
     * @brief update
     * @param nBin
     */
    void update(int nBin)
    {
        if(nBin < 1) {
            nBin = 256;
        } else {
            this->nBin = nBin;
        }
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut = NULL)
    {
        Image *img_source = NULL; //imgIn[0]
        Image *img_target = NULL; //imgIn[1]

        int count = 0;
        if(ImageVecCheck(imgIn, 1)) {
            img_source = imgIn[0];
            count = 1;
        }

        if(ImageVecCheck(imgIn, 2)) {
            img_target = imgIn[1];

            if(imgIn[0]->channels != imgIn[1]->channels) {
                return imgOut;
            }
            count = 2;
        }

        if(count == 0) {
            return imgOut;
        }

        count--;

        if(imgOut == NULL) {
            imgOut = imgIn[count]->clone();
        } else {
            if(!imgOut->isSimilarType(imgIn[count])) {
                imgOut = imgIn[count]->allocateSimilarOne();
            }
        }

        int channels = img_source->channels;

        Histogram *h_source = new Histogram[channels];
        Histogram *h_target = new Histogram[channels];

        std::vector<int *> lut;

        for(int i = 0; i < channels; i++) {
            h_source[i].calculate(img_source, VS_LIN, nBin, NULL, i);

            if(img_target != NULL) {
                h_target[i].calculate(img_target, VS_LIN, nBin, NULL, i);
            } else {
                uint value = (img_source->width * img_source->height) / nBin;
                h_target[i].uniform(h_source[i].getfMin(),
                                    h_source[i].getfMax(),
                                    MAX(value, 1), VS_LIN, nBin);
            }

            h_source[i].cumulativef(true);
            h_target[i].cumulativef(true);

            float *c_source = h_source[i].getCumulativef();
            float *c_target = h_target[i].getCumulativef();

            int *tmp_lut = new int[nBin];

            for(int j = 0; j < nBin; j++) {
                float x = c_source[j];
                float *ptr = std::upper_bound(c_target, c_target + nBin, x);
                tmp_lut[j] = MAX((int)(ptr - c_target), 0);
            }

            lut.push_back(tmp_lut);
        }

        for(int i = 0; i < imgOut->size(); i += channels) {

            for(int j = 0; j < channels; j++) {
                int ind_source = h_source[j].project(img_source->data[i + j]);

                int ind_target = lut[j][ind_source];

                imgOut->data[i + j] = h_target[j].unproject(ind_target);
            }
        }

        delete[] h_source;
        delete[] h_target;

        stdVectorArrayClear(lut);

        return imgOut;
    }

    /**
     * @brief execute
     * @param img_source
     * @param img_target
     * @param imgOut
     * @return
     */
    static Image* execute(Image *img_source, Image *img_target, Image *imgOut = NULL)
    {
        HistogramMatching hm;
        imgOut = hm.Process(Double(img_source, img_target), imgOut);
        return imgOut;
    }

    /**
     * @brief executeEqualization
     * @param img
     * @param imgOut
     * @return
     */
    static Image* executeEqualization(Image *img, Image *imgOut = NULL)
    {
        HistogramMatching hm;
        imgOut = hm.Process(Double(img, NULL), imgOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_HISTOGRAM_MATCHING_HPP */

