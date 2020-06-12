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

    /**
     * @brief computeHistograms
     * @param img_s
     * @param img_t
     * @param hist_s
     * @param hist_t
     * @param lut
     */
    void computeHistograms(Image *img_s, Image *img_t,
                           Histogram *hist_s, Histogram *hist_t,
                           std::vector<int *> &lut)
    {
        int channels = img_s->channels;

        for(int i = 0; i < channels; i++) {
            hist_s[i].calculate(img_s, VS_LIN, nBin, NULL, i);

            if(img_t != NULL) {
                hist_t[i].calculate(img_t, VS_LIN, nBin, NULL, i);
            } else {
                uint value = (img_s->width * img_s->height) / nBin;
                hist_t[i].uniform(hist_s[i].getfMin(),
                                  hist_s[i].getfMax(),
                                  MAX(value, 1), VS_LIN, nBin);
            }

            hist_s[i].cumulativef(true);
            hist_t[i].cumulativef(true);

            float *c_s = hist_s[i].getCumulativef();
            float *c_t = hist_t[i].getCumulativef();

            int *tmp_lut = new int[nBin];

            for(int j = 0; j < nBin; j++) {
                float x = c_s[j];
                float *ptr = std::upper_bound(c_t, c_t + nBin, x);
                tmp_lut[j] = MAX((int)(ptr - c_t), 0);
            }

            lut.push_back(tmp_lut);
        }
    }

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
        this->nBin = nBin > 1 ? nBin : 256;
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

        computeHistograms(img_source, img_target, h_source, h_target, lut);


        for(int i = 0; i < imgOut->size(); i += channels) {

            for(int j = 0; j < channels; j++) {
                int k = i + j;

                int ind_source = h_source[j].project(img_source->data[k]);

                int ind_target = lut[j][ind_source];

                imgOut->data[k] = h_target[j].unproject(ind_target);
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

