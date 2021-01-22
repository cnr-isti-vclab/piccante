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

#ifndef PIC_GL_FILTERING_FILTER_WARP_2D_HPP
#define PIC_GL_FILTERING_FILTER_WARP_2D_HPP

#include "../util/matrix_3_x_3.hpp"

#include "../filtering/filter.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"

namespace pic {

/**
 * @brief The FilterWarp2D class
 */
class FilterWarp2D: public Filter
{
protected:
    ImageSamplerBilinear isb;
    Matrix3x3 h, h_inv;
    int bmin[2], bmax[2];
    float mid[2];
    bool bComputeBoundingBox;

    /**
     * @brief ProcessBBox
     * @param dst
     * @param src
     * @param box
     */
    void ProcessBBox(Image *dst, ImageVec src, BBox *box)
    {
        int channels = src[0]->channels;

        float pos[2], pos_out[2];

        for(int j = box->y0; j < box->y1; j++) {
            pos[1] = float(j + bmin[1]) - mid[1];

            for(int i = box->x0; i < box->x1; i++) {
                float *tmp_dst = (*dst)(i, j);

                pos[0] = float(i + bmin[0]) - mid[0];

                h_inv.projection(pos, pos_out);

                pos_out[0] += mid[0];
                pos_out[1] += mid[1];

                if(pos_out[0] >= 0.0f && pos_out[0] <= src[0]->width1f &&
                   pos_out[1] >= 0.0f && pos_out[1] <= src[0]->height1f) {
                    isb.SampleImageUC(src[0], pos_out[0], pos_out[1], tmp_dst);
                } else {
                    Arrayf::assign(0.0f, tmp_dst, channels);
                }
            }
        }
    }

    bool bSameSize, bCentroid;

public:

    /**
     * @brief FilterWarp2D
     */
    FilterWarp2D() : Filter()
    {
        this->bComputeBoundingBox = true;
        this->bCentroid = false;
        this->bSameSize = false;

        h.getIdentity();
        h_inv.getIdentity();
    }

    /**
     * @brief FilterWarp2D
     * @param h
     * @param bSameSize
     * @param bCentroid
     */
    FilterWarp2D(Matrix3x3 h, bool bSameSize = false, bool bCentroid = false) : Filter()
    {
        this->bComputeBoundingBox = true;
        update(h, bSameSize, bCentroid);
    }

    /**
     * @brief getBCentroid
     * @return
     */
    bool getBCentroid()
    {
        return bCentroid;
    }

    /**
     * @brief computeBoundingBox
     * @param h
     * @param bCentroid
     * @param width
     * @param height
     * @param bmin
     * @param bmax
     */
    static void computeBoundingBox(Matrix3x3 &h, bool bCentroid,
                                   float width, float height,
                                   int *bmin, int *bmax ) {
        float bbox[4][2];
        float bbox_out[4][2];

        bbox[0][0] = 0.0f;
        bbox[0][1] = 0.0f;

        bbox[1][0] = 0.0f;
        bbox[1][1] = height;

        bbox[2][0] = width;
        bbox[2][1] = 0.0f;

        bbox[3][0] = width;
        bbox[3][1] = height;

        float mid[2];

        if(bCentroid) {
            mid[0] = width  * 0.5f;
            mid[1] = height * 0.5f;
        } else {
            mid[0] = 0.0f;
            mid[1] = 0.0f;
        }

        //compute the bounding box
        bmin[0] = 1 << 24;
        bmin[1] = 1 << 24;

        bmax[0] = -1;
        bmax[1] = -1;

        for(int i = 0; i < 4; i++) {

            bbox[i][0] -= mid[0];
            bbox[i][1] -= mid[1];

            h.projection(&bbox[i][0], &bbox_out[i][0]);

            bbox_out[i][0] += mid[0];
            bbox_out[i][1] += mid[1];

            int x = int(bbox_out[i][0]);
            int y = int(bbox_out[i][1]);

            //min point
            bmin[0] = (x < bmin[0]) ? x : bmin[0];
            bmin[1] = (y < bmin[1]) ? y : bmin[1];

            bmax[0] = (x > bmax[0]) ? x : bmax[0];
            bmax[1] = (y > bmax[1]) ? y : bmax[1];
        }

        #ifdef PIC_DEBUG
            printf("%d %d %d %d\n", bmax[0], bmin[0], bmax[1], bmin[1]);
        #endif
    }

    /**
     * @brief setBoundingBox
     * @param bmin
     * @param bmax
     */
    void setBoundingBox(int *bmin, int *bmax)
    {
        memcpy(this->bmin, bmin, sizeof(int) * 2);
        memcpy(this->bmax, bmax, sizeof(int) * 2);
        bComputeBoundingBox = false;
    }

    /**
     * @brief update
     * @param h
     * @param bSameSize
     * @param bCentroid
     */
    void update(Matrix3x3 h, bool bSameSize, bool bCentroid = false)
    {
        this->bComputeBoundingBox = true;

        this->bSameSize = bSameSize;
        this->bCentroid = bCentroid;

        this->h = h;
        h.inverse(&h_inv);
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
        if(bCentroid) {
            mid[0] = imgIn[0]->widthf;
            mid[1] = imgIn[0]->heightf;
        } else {
            mid[0] = 0.0f;
            mid[1] = 0.0f;
        }

        if(!bSameSize) {
            if(bComputeBoundingBox) {
                computeBoundingBox(h, bCentroid,
                                   imgIn[0]->widthf, imgIn[0]->heightf,
                                   bmin, bmax);
            }

            width  = bmax[0] - bmin[0];
            height = bmax[1] - bmin[1];
        } else {
            bmin[0] = 0;
            bmin[1] = 0;

            bmax[0] = 0;
            bmax[1] = 0;

            width  = imgIn[0]->width;
            height = imgIn[0]->height;
        }

        frames   = imgIn[0]->frames;
        channels = imgIn[0]->channels;
    }

    /**
     * @brief execute
     * @param img
     * @param imgOut
     * @param h
     * @param bSameSize
     * @param bCentroid
     * @return
     */
    static Image *execute(Image *img, Image *imgOut, Matrix3x3 h, bool bSameSize = false, bool bCentroid = false)
    {
        FilterWarp2D flt(h, bSameSize, bCentroid);
        imgOut = flt.Process(Single(img), imgOut);
        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_WARP_2D_HPP */

