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

#ifndef PIC_FILTERING_FILTER_BILATERAL_2DG_HPP
#define PIC_FILTERING_FILTER_BILATERAL_2DG_HPP

//#define BILATERAL_GRID_MULTI_PASS

#include "../util/std_util.hpp"

#include "../filtering/filter.hpp"
#include "../filtering/filter_gaussian_3d.hpp"
#include "../image_samplers/image_sampler_bilinear.hpp"

namespace pic {

/**
 * @brief The FilterBilateral2DG class
 */
class FilterBilateral2DG: public Filter
{
protected:
    ImageSamplerBilinear isb;
    FilterGaussian3D *fltG;
    int width, height, range, padding;
    float sigma_s, sigma_r;
    float edge_min_val, edge_max_val;

    Image *grid, *gridBlur;
    bool parallel;

    /**
     * @brief Splat splats values into the grid.
     * @param base
     * @param edge
     * @param channel
     * @return
     */
    Image *Splat(Image *base, Image *edge, int channel);

    /**
     * @brief Slice slices the grid into the output image
     * @param out
     * @param base
     * @param edge
     * @param channels
     */
    void Slice(Image *out, Image *base, Image *edge, int channel);

public:

    /**
     * @brief FilterBilateral2DG
     * @param sigma_s
     * @param sigma_r
     */
    FilterBilateral2DG(float sigma_s, float sigma_r)
    {
        //protected values are assigned/computed
        this->sigma_s = sigma_s;
        this->sigma_r = sigma_r;
        
        if (this->sigma_s <= 1e-6f) {
            this->sigma_s = 1.0f;
        }

        if (this->sigma_r <= 1e-6f) {
            this->sigma_r = 0.05f;
        }

        edge_min_val = 0.0f;
        edge_max_val = 1.0f;
        
        padding = 3;
        
        parallel = false;

        grid = NULL;
        gridBlur = NULL;

        fltG = new FilterGaussian3D(1.0f);
    }

    ~FilterBilateral2DG()
    {
        delete_s(grid);
        delete_s(gridBlur);
        delete_s(fltG);
    }

    float s_S, s_R, mul_E;

    /**
     * @brief Signature
     * @return
     */
    std::string signature()
    {
        return genBilString("G", sigma_s, sigma_r);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut);

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static Image *execute(Image *imgIn, Image *imgOut, float sigma_s, float sigma_r)
    {
        FilterBilateral2DG filter(sigma_s, sigma_r);

        //long t0 = timeGetTime();

        imgOut = filter.Process(Single(imgIn), imgOut); //Filtering

        //long t1 = timeGetTime();
        //printf("Bilateral Grid Filter time: %f\n", float(t1 - t0) / 1000.0f);

        return imgOut;
    }
};

PIC_INLINE Image *FilterBilateral2DG::Splat(Image *base, Image *edge, int channel)
{
    if(grid == NULL) {
        #ifdef PIC_DEBUG
            printf("S Rate: %f R Rate: %f Mul E: %f\n", s_S, s_R, mul_E);
        #endif
        
        width =  int(ceilf(float(base->width)  * s_S)) + padding * 2 + 1;
        height = int(ceilf(float(base->height) * s_S)) + padding * 2 + 1;
        range =  int(ceilf((edge_max_val - edge_min_val) * s_R)) + padding * 2 + 1;

        #ifdef PIC_DEBUG
            printf("Grid Size: %d %d %d\n", width, height, range);
        #endif

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
        #ifdef PIC_DEBUG
        printf("Grid - Memory Mb: %3.2f\n",
               float(width + 1)*float(height + 1)*float(range + 1) * 8.0f /
               (1024.0f * 1024.0f));
        #endif

        grid = new Image(range + 1, width + 1, height + 1, 2);
        gridBlur = new Image(range + 1, width + 1, height + 1, 2);
#else
        #ifdef PIC_DEBUG
        printf("Grid - Memory Mb: %3.2f\n",
               float(width + 1)*float(height + 1)*float(range + 1) * 16.0f /
               (1024.0f * 1024.0f));
        #endif

        grid = new Image(range + 1, width + 1, height + 1, base->channels + 1);
        gridBlur = new Image(range + 1, width + 1, height + 1, base->channels + 1);
#endif
    }

    grid->setZero();
    
    float channelsf = 1.0f / float(edge->channels);

    for(int j = 0; j < base->height; j++) {
        
        int y = int(lround(float(j) * s_S)) + padding;

        for(int i = 0; i < base->width; i++) {

            int ind = i * base->xstride + j * base->ystride;

            int ind_edge = i * edge->xstride + j * edge->ystride;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            float E = edge->data[ind_edge + channel];
#else
            float E = edge->data[ind_edge];
            for(int k = 1; k < edge->channels; k++) {
                E += edge->data[ind_edge + k];
            }
            
            E *= channelsf;
#endif
            E = (E - edge_min_val) * mul_E;

            int x = int(lround(float(i) * s_S)) + padding;
            int r = int(lround(E)) + padding;

            int ind_grid = x * grid->xstride + y * grid->ystride + r * grid->tstride;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            grid->data[ind_grid    ] += base->data[ind + channel];
            grid->data[ind_grid + 1] += 1.0f;
#else
            for(int k = 0; k < base->channels; k++) {
                grid->data[ind_grid + k] += base->data[ind + k];
            }
            grid->data[ind_grid + base->channels] += 1.0f;	//Counter
#endif
        }
    }
    return grid;
}

PIC_INLINE void FilterBilateral2DG::Slice(Image *out, Image *base, Image *edge, int channel)
{
    float widthf = grid->width1f;
    float heightf = grid->height1f;
    float rangef = grid->frames1f;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
    float vOut[2];
#else
    float *vOut = new float [out->channels + 1];
#endif

    float channelsf = 1.0f / float(edge->channels);
    
    for(int j = 0; j < out->height; j++) {
        float y = float(j) * s_S + padding;

        for(int i = 0; i < out->width; i++) {
            int ind = i * out->xstride + j * out->ystride;

            float x = float(i) * s_S + padding;

            int ind_edge = i * edge->xstride + j * edge->ystride;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            float E = edge->data[ind_edge + channel];
#else
            float E = edge->data[ind_edge];
            for(int k = 1; k < edge->channels; k++) {
                E += edge->data[ind_edge + k];
            }
            
            E *= channelsf;
#endif
            E = (E - edge_min_val) * mul_E + padding;

            //Trilinear filtering
            isb.SampleImage(gridBlur, x / widthf, y / heightf, E / rangef, vOut);

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            if(vOut[1] > 0.0f) {
                out->data[ind + channel] = vOut[0] / vOut[1];
            } else {
                out->data[ind + channel] = 0.0f;
            }
#else
            bool bFlag = (vOut[out->channels] > 0.0f);
            for(int k = 0; k < out->channels; k++) {
                out->data[ind + k] = bFlag ? vOut[k] / vOut[out->channels] : 0.0f;
            }
#endif
        }
    }
    
#ifndef PIC_BILATERAL_GRID_MULTI_PASS
    delete[] vOut;
#endif
}

PIC_INLINE Image *FilterBilateral2DG::Process(ImageVec imgIn, Image *imgOut)
{
    if(!checkInput(imgIn)) {
        return imgOut;
    }

    imgOut = setupAux(imgIn, imgOut);

    if(imgOut == NULL) {
        return imgOut;
    }

    Image *base = NULL;
    Image *edge = NULL;

    base = imgIn[0];

    bool bFlag = false;
    if(imgIn.size() == 2) {
        bFlag = true;
        edge = imgIn[1];
    } else {
        edge = base;

        int index;
        
        float *edgeMinVal = edge->getMinVal(NULL, NULL);
        edge_min_val = Arrayf::getMin(edgeMinVal, edge->channels, index);
        delete[] edgeMinVal;

        float *edgeMaxVal = edge->getMaxVal(NULL, NULL);
        int index_min;
        edge_max_val = Arrayf::getMax(edgeMaxVal, edge->channels, index);
        delete[] edgeMaxVal;


    }

    //Range in [0,1]
    
    //float tmpSigma_r = sigma_r;
    //sigma_r /= maxVal;

    //Grid's Initialization
    s_S = 1.0f / sigma_s;       //Spatial Sampling rate
    s_R = 1.0f / sigma_r;       //Range Sampling rate

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
    int n = imgIn[0]->channels;
#else
    int n = 1;
#endif

    mul_E = s_R;

    for(int i = 0; i < n; i++) {
        //splat
        Splat(base, edge, i);

        //blur
        fltG->Process(Single(grid), gridBlur);

        //slice
        Slice(imgOut, base, edge, i);
    }

    return imgOut;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DG_HPP */

