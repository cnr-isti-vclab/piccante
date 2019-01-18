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

#ifndef PIC_FILTERING_FILTER_COMBINE_HPP
#define PIC_FILTERING_FILTER_COMBINE_HPP

#include "../filtering/filter.hpp"

#include "../filtering/filter_channel.hpp"

namespace pic {

/**
 * @brief The FilterCombine class
 */
class FilterCombine: public Filter
{
protected:

    /**
     * @brief f
     * @param data
     */
    virtual void f(FilterFData *data)
    {
        int k2 = 0;

        for(auto i = 0; i < data->nSrc; i++) {
            float *tmp_src = (*data->src[i])(data->x, data->y);

            for(int k = 0; k < data->src[i]->channels; k++) {
                data->out[k2] = tmp_src[k];
                k2++;
            }
        }
    }

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    /*
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        for(int p = box->z0; p < box->z1; p++) {
            for(int j = box->y0; j < box->y1; j++) {
                for(int i = box->x0; i < box->x1; i++) {
                    int c  = p * dst->tstride + j * dst->ystride + i * dst->xstride;
                    int k2 = 0;

                    for(unsigned int im = 0; im < src.size(); im++) {
                        int c2 = p * src[im]->tstride + j * src[im]->ystride + i * src[im]->xstride;

                        for(int k = 0; k < src[im]->channels; k++) {
                            dst->data[c + k2] = src[im]->data[c2 + k];
                            k2++;
                        }
                    }
                }
            }
        }
    }*/

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *setupAux(ImageVec imgIn, Image *imgOut)
    {
        int channels = imgIn[0]->channels;
        for(unsigned int i = 1; i < imgIn.size(); i++) {
            channels += imgIn[i]->channels;

            if(!imgIn[0]->isSimilarType(imgIn[i])) {
                return NULL;
            }
        }

        if(imgOut == NULL) {
            imgOut = new Image(imgIn[0]->frames, imgIn[0]->width, imgIn[0]->height,
                                  channels);
        } else {
            bool bAllocate = false;
            if(!imgOut->isValid()) {
                bAllocate = true;
            } else {
                bAllocate = imgOut->channels != channels;
            }

            if(bAllocate) {
                imgOut = new Image(imgIn[0]->frames, imgIn[0]->width, imgIn[0]->height,
                        channels);
            }
        }

        return imgOut;
    }

public:

    /**
     * @brief FilterCombine
     */
    FilterCombine() : Filter()
    {

    }

    /**
     * @brief addAlpha
     * @param imgIn
     * @param imgOut
     * @param value
     * @return
     */
    static Image *addAlpha(Image *imgIn, Image *imgOut, float value)
    {
        //create an alpha channel
        Image *alpha = new Image(imgIn->frames, imgIn->width, imgIn->height, 1);
        *alpha = value;

        //add the channel to the image
        ImageVec src;
        src.push_back(imgIn);
        src.push_back(alpha);

        FilterCombine filterC;
        imgOut = filterC.Process(src, imgOut);

        delete alpha;
        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @return
     */
    static Image *execute(ImageVec imgIn, Image *imgOut)
    {
        FilterCombine filterC;
        return filterC.Process(imgIn, imgOut);
    }

    /**
     * @brief getOnlyRGB
     * @param nameIn
     * @param nameOut
     * @return
     */
    static Image *getOnlyRGB(Image *imgIn, Image *imgOut)
    {
        ImageVec src;
        FilterChannel filter(SingleInt(0));

        for(int i = 0; i < 3; i++) {
            Image *out = filter.Process(Single(imgIn), NULL);
            src.push_back(out);
            filter.update(SingleInt(i + 1));
        }

        imgOut = execute(src, NULL);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_COMBINE_HPP */

