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

#ifndef PIC_FILTERING_FILTER_MAX_POOLING_HPP
#define PIC_FILTERING_FILTER_MAX_POOLING_HPP

#include "../util/std_util.hpp"
#include "../filtering/filter_npasses.hpp"

namespace pic {

/**
 * @brief The FilterMaxPooling class
 */
class FilterMaxPooling: public FilterNPasses
{
protected:
    
    int kernel_size_x, kernel_size_y;
    
    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int channels = dst->channels;
        
        for(int k = box->z0; k < box->z1; k++) {
            for(int j = box->y0; j < box->y1; j ++) {
                int j2 = j * this->kernel_size_y;
                
                for(int i = box->x0; i < box->x1; i++) {
                    int i2 = i * this->kernel_size_x;

                    float *dst_data = (*dst)(i, j, k);

                    Arrayf::assign(-FLT_MAX, dst_data, channels);
                        
                    for(int l = 0; l < this->kernel_size_x; l++) {
                        for(int m = 0; m < this->kernel_size_y; m++) {
                                
                            float *src_data = (*src[0])(i2 + l, j2 + m, k);
                            
                            for(int ch = 0; ch < channels; ch++) {
                                dst_data[ch] = dst_data[ch] > src_data[ch] ? dst_data[ch] : src_data[ch];
                            }
                        }
                    }
                }
            }
        }
    }

public:

    /**
     * @brief FilterMaxPooling
     * @param strideX
     * @param strideY
     */
    FilterMaxPooling(int kernel_size_x, int kernel_size_y) : FilterNPasses()
    {
        this->kernel_size_x = MAX(kernel_size_x, 1);
        this->kernel_size_y = MAX(kernel_size_y, 1);
    }

    ~FilterMaxPooling()
    {
        release();
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
        if(imgIn.size() == 1) {
            width       = imgIn[0]->width / this->kernel_size_x;
            height      = imgIn[0]->height / this->kernel_size_y;
        }

        channels    = imgIn[0]->channels;
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param kernel_size_x
     * @param kernel_size_y
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, int kernel_size_x = 2, int kernel_size_y = 2)
    {
        FilterMaxPooling flt(kernel_size_x, kernel_size_y);
        return flt.Process(Single(imgIn), imgOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_DOWNSAMPLER_2D_HPP */

