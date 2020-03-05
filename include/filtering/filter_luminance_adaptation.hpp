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

#ifndef PIC_FILTERING_FILTER_LUMINANCE_ADAPTATION_HPP
#define PIC_FILTERING_FILTER_LUMINANCE_ADAPTATION_HPP

#include <cmath>

#include "../util/math.hpp"
#include "../filtering/filter.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../algorithms/connected_components.hpp"

namespace pic {

/**
 * @brief The FilterLuminanceAdaptation class
 */
class FilterLuminanceAdaptation: public Filter
{
protected:
    float threshold;
    float bin_size_1;
    float bin_size_2;
    float delta_bin_size;
    int maxLayers;
    FilterLuminance flt;
    Image *lum;

public:

    /**
     * @brief FilterLuminanceAdaptation
     * @param maxLayers
     * @param threshold
     */
    FilterLuminanceAdaptation(int maxLayers = 32, float threshold = 0.05f) : Filter()
    {
        lum = NULL;
        update(maxLayers, threshold);
    }

    /**
     * @brief update
     * @param maxLayers
     * @param threshold
     */
    void update(int maxLayers = 32, float threshold = 0.05f)
    {
        this->threshold = threshold > 0.0f ? threshold : 0.05f;
        this->maxLayers = maxLayers > 0 ? maxLayers : 32;

        bin_size_1 = 0.5f;
        bin_size_2 = 2.0f;
        delta_bin_size = bin_size_2 - bin_size_1;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = 1;
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief Filter::Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(!checkInput(imgIn)) {
            return imgOut;
        }

        imgOut = setupAux(imgIn, imgOut);

        if(imgOut == NULL) {
            return imgOut;
        }

        lum = flt.Process(imgIn, lum);
        lum->applyFunction(log10fPlusEpsilon);

        float lum_min;
        lum->getMinVal(NULL, &lum_min);

        int n = imgIn[0]->width * imgIn[0]->height;
        int *category = new int[n];
        unsigned int *img_labels = NULL;

        imgOut->setZero();

        float maxLayer_m_1 = float(maxLayers - 1);

        for(int i = 0; i < maxLayers; i++) {
            float bin_size = bin_size_1 + (float(i) * delta_bin_size / maxLayer_m_1);

            #pragma omp parallel for
            for(int j = 0; j < n; j++) {
                category[j] = int(lround((lum->data[j] - lum_min) / bin_size));
                category[j]++;
            }

            ConnectedComponents<int> cc_int;

            std::vector<LabelOutput> labelsList;
            img_labels = cc_int.execute(category, imgIn[0]->width, imgIn[0]->height, img_labels, labelsList);

            ConnectedComponents<int>::mergeIsolatedAreasWithThreshold(img_labels, lum->width, lum->height, labelsList);

            ConnectedComponents<int>::reCount(img_labels, labelsList);

            for(unsigned int j_ui = 0; j_ui < labelsList.size(); j_ui++) {
                //mean luminance
                float La_j_ui = IndexedArrayf::mean(lum->data, labelsList[j_ui].coords);
                IndexedArrayf::add(imgOut->data, labelsList[j_ui].coords, La_j_ui);
            }
        }
        (*imgOut) /= float (maxLayers);

        imgOut->applyFunction(powf10fMinusEpsilon);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_LUMINANCE_ADAPTATION_HPP */

