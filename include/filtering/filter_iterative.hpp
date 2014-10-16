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

#ifndef PIC_FILTERING_FILTER_ITERATIVE_HPP
#define PIC_FILTERING_FILTER_ITERATIVE_HPP

#include "filtering/filter.hpp"

namespace pic {

class FilterIterative: public Filter
{
protected:
    Image	*imgTmp[2];

    bool		parallel;
    int			iterations;

public:

    //Basic constructor
    FilterIterative();
    FilterIterative(Filter *flt, int iterations);

    //Basic
    ~FilterIterative();
    void Destroy();

    //Setup NPasses
    virtual Image *SetupAuxN(ImageVec imgIn, Image *imgOut);

    void Update(Filter *flt, int iterations);

    //Process
    Image *Process(ImageVec imgIn, Image *imgOut);
    //Process in parallel
    Image *ProcessP(ImageVec imgIn, Image *imgOut);
};

//Basic constructor
FilterIterative::FilterIterative()
{
    parallel = false;
    iterations = 0;

    for(int i = 0; i < 2; i++) {
        imgTmp[i] = NULL;
    }
}

FilterIterative::FilterIterative(Filter *flt, int iterations)
{
    for(int i = 0; i < 2; i++) {
        imgTmp[i] = NULL;
    }

    Update(flt, iterations);
}

//Basic
FilterIterative::~FilterIterative()
{
    Destroy();
}

void FilterIterative::Destroy()
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

void FilterIterative::Update(Filter *flt, int iterations)
{
    if(iterations > 1) {
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

//Setup NPasses
Image *FilterIterative::SetupAuxN(ImageVec imgIn, Image *imgOut)
{
    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOne();
    }

    if((iterations % 2) == 0) {
        imgTmp[1] = imgOut;

        if(imgTmp[0] == NULL) {
            imgTmp[0] = imgOut->AllocateSimilarOne();
        }
    } else {
        imgTmp[0] = imgOut;

        if(imgTmp[1] == NULL) {
            imgTmp[1] = imgOut->AllocateSimilarOne();
        }
    }

    return imgOut;
};

//Processing
Image *FilterIterative::Process(ImageVec imgIn, Image *imgOut)
{
    if(imgIn.size() < 1 || imgIn[0] == NULL) {
        return imgOut;
    }

    //Allocate output
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

//Processing in parallel
Image *FilterIterative::ProcessP(ImageVec imgIn, Image *imgOut)
{
    parallel = true;
    return Process(imgIn, imgOut);
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ITERATIVE_HPP */

