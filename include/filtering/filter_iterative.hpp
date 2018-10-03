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

#ifndef PIC_FILTERING_FILTER_ITERATIVE_HPP
#define PIC_FILTERING_FILTER_ITERATIVE_HPP

#include "../filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterIterative class
 */
class FilterIterative: public Filter
{
protected:
    Image *imgTmp[2];

    bool parallel;
    int iterations;

    /**
     * @brief Destroy
     */
    void Destroy();

public:

    /**
     * @brief FilterIterative
     */
    FilterIterative();

    /**
     * @brief FilterIterative
     * @param flt
     * @param iterations
     */
    FilterIterative(Filter *flt, int iterations);

    ~FilterIterative();

    /**
     * @brief SetupAuxN
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual Image *SetupAuxN(ImageVec imgIn, Image *imgOut);

    /**
     * @brief update
     * @param flt
     * @param iterations
     */
    void update(Filter *flt, int iterations);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut);

    /**
     * @brief ProcessP
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *ProcessP(ImageVec imgIn, Image *imgOut);
};

PIC_INLINE FilterIterative::FilterIterative()
{
    parallel = false;
    iterations = 0;

    for(int i = 0; i < 2; i++) {
        imgTmp[i] = NULL;
    }
}

PIC_INLINE FilterIterative::FilterIterative(Filter *flt, int iterations)
{
    for(int i = 0; i < 2; i++) {
        imgTmp[i] = NULL;
    }

    update(flt, iterations);
}

PIC_INLINE FilterIterative::~FilterIterative()
{
    Destroy();
}

PIC_INLINE void FilterIterative::Destroy()
{
    if((iterations % 2) == 0) {
        if(imgTmp[0] != NULL) {
            delete imgTmp[0];
        }
    } else {
        if(imgTmp[1] != NULL) {
            delete imgTmp[1];
        }
    }

    for(int i = 0; i < 2; i++) {
        imgTmp[i] = NULL;
    }
}

PIC_INLINE void FilterIterative::update(Filter *flt, int iterations)
{
    if(iterations > 0) {
        this->iterations = iterations;
    }

    if(flt == NULL) {
        return;
    }

    if(filters.size() > 0) {
        filters.clear();
    }

    filters.push_back(flt);
}

PIC_INLINE Image *FilterIterative::SetupAuxN(ImageVec imgIn, Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->allocateSimilarOne();
    }

    if((iterations % 2) == 0) {
        imgTmp[1] = imgOut;

        if(imgTmp[0] == NULL) {
            imgTmp[0] = imgOut->allocateSimilarOne();
        }
    } else {
        imgTmp[0] = imgOut;

        if(imgTmp[1] == NULL) {
            imgTmp[1] = imgOut->allocateSimilarOne();
        }
    }

    return imgOut;
}

PIC_INLINE Image *FilterIterative::Process(ImageVec imgIn, Image *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL) {
        return imgOut;
    }

    //allocate output
    imgOut = SetupAuxN(imgIn, imgOut);

    if(parallel) {
        filters[0]->ProcessP(imgIn, imgTmp[0]);
    } else {
        filters[0]->Process(imgIn, imgTmp[0]);
    }

    for(int i = 1; i < iterations; i++) {
        imgIn[0] = imgTmp[(i + 1) % 2];

        if(parallel) {
            filters[0]->ProcessP(imgIn, imgTmp[i % 2]);
        } else {
            filters[0]->Process(imgIn, imgTmp[i % 2]);
        }
    }

    parallel = false;
    return imgOut;
}

PIC_INLINE Image *FilterIterative::ProcessP(ImageVec imgIn, Image *imgOut)
{
    parallel = true;
    return Process(imgIn, imgOut);
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ITERATIVE_HPP */

