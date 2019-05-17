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
    int width, height, range;
    float sigma_s, sigma_r;

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
    void Slice(Image *out, Image *base, Image *edge, int channels);

public:

    /**
     * @brief FilterBilateral2DG
     * @param sigma_s
     * @param sigma_r
     */
    FilterBilateral2DG(float sigma_s, float sigma_r);

    ~FilterBilateral2DG();

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
    static Image *execute(Image *imgIn, Image *imgOut, float sigma_s,
                             float sigma_r)
    {
        FilterBilateral2DG filter(sigma_s, sigma_r);

        //long t0 = timeGetTime();

        imgOut = filter.Process(Single(imgIn), NULL); //Filtering

        //long t1 = timeGetTime();
        //printf("Bilateral Grid Filter time: %f\n", float(t1 - t0) / 1000.0f);

        return imgOut;
    }
};

PIC_INLINE FilterBilateral2DG::FilterBilateral2DG(float sigma_s, float sigma_r) : Filter()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    parallel = false;

    grid = NULL;
    gridBlur = NULL;

    fltG = new FilterGaussian3D(1.0f);
}

PIC_INLINE FilterBilateral2DG::~FilterBilateral2DG()
{
    delete_s(grid);
    delete_s(gridBlur);
    delete_s(fltG);
}

PIC_INLINE Image *FilterBilateral2DG::Splat(Image *base, Image *edge, int channels)
{
    if(grid == NULL) {
        #ifdef PIC_DEBUG
            printf("S Rate: %f R Rate: %f Mul E: %f\n", s_S, s_R, mul_E);
        #endif

        width =  int(ceilf(float(base->width) * s_S));
        height = int(ceilf(float(base->height) * s_S));
        range =  int(ceilf(s_R));

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

    for(int j = 0; j < base->height; j++) {
        int y = int(lround(float(j) * s_S));

        for(int i = 0; i < base->width; i++) {

            int ind = i * base->xstride + j * base->ystride;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            float E = edge->data[ind + channel];
#else
            float E = 0.0f;

            for(int k = 0; k < edge->channels; k++) {
                E += edge->data[ind + k];
            }

#endif
            E *= mul_E;

            int x = int(lround(float(i) * s_S));
            int r = int(lround(E));

            int grdInd = x * grid->xstride + y * grid->ystride + r * grid->tstride;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            grid->data[grdInd + 0] += base->data[ind + channels];
            grid->data[grdInd + 1] += 1.0f;
#else

            for(int k = 0; k < base->channels; k++) {
                grid->data[grdInd + k] += base->data[ind + k];
            }

            grid->data[grdInd + base->channels] += 1.0f;	//Counter
#endif
        }
    }
    return grid;
}

PIC_INLINE void FilterBilateral2DG::Slice(Image *out, Image *base, Image *edge,
                               int channels)
{
    float widthf = float(grid->width);
    float heightf = float(grid->height);
    float rangef = float(grid->frames);

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
    float vOut[2];
#else
    float *vOut = new float [out->channels + 1];
#endif

    for(int j = 0; j < out->height; j++) {
        for(int i = 0; i < out->width; i++) {
            int ind = i * out->xstride + j * out->ystride;

            float x = float(i) * s_S;
            float y = float(j) * s_S;

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
            float E = edge->data[ind + channels];
#else
            float E = Arrayf::sum(&edge->data[ind], out->channels);

#endif
            E *= mul_E;

            //Trilinear filtering
            isb.SampleImage(gridBlur, x / widthf, y / heightf, E / rangef, vOut);

#ifdef PIC_BILATERAL_GRID_MULTI_PASS

            if(vOut[1] > 0.0f) {
                out->data[ind + channels] = vOut[0] / vOut[1];
            } else {
                out->data[ind + channels] = 0.0f;
            }

#else
            if(vOut[out->channels] > 0.0f) {
                for(int k = 0; k < out->channels; k++) {
                    out->data[ind + k] = vOut[k] / vOut[out->channels];
                }
            } else {
                Arrayf::assign(0.0f, &out->data[ind], out->channels);
            }

#endif
        }
    }
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

    Image *base, *edge;

    base = imgIn[0];

    int ind;
    float *baseMaxmaxVal = base->getMaxVal(NULL, NULL);
    float maxVal = Arrayf::getMax(baseMaxmaxVal, base->channels, ind);
    delete[] baseMaxmaxVal;

    if(imgIn.size() == 2) {
        edge = imgIn[1];

        float *edgeMaxVal = edge->getMaxVal(NULL, NULL);

        maxVal = MAX(maxVal, Arrayf::getMax(edgeMaxVal, edge->channels, ind));

        delete[] edgeMaxVal;

        *edge /= maxVal;
    } else {
        edge = imgIn[0];
    }

    //Range in [0,1]
    *base /= maxVal;
    float tmpSigma_r = sigma_r;
    sigma_r /= maxVal;

    //Grid's Initialization
    s_S = 1.0f / sigma_s;	//Spatial Sampling rate
    s_R = 1.0f / sigma_r; //Range Sampling rate

#ifdef PIC_BILATERAL_GRID_MULTI_PASS
    int n = imgIn[0]->channels;
    mul_E = s_R;
#else
    int n = 1;
    mul_E = s_R / float(imgIn[0]->channels);
#endif

    for(int i = 0; i < n; i++) {
        //splat
        Splat(base, edge, i);

        //blur
        fltG->Process(Single(grid), gridBlur);

        //slice
        Slice(imgOut, base, edge, i);
    }

    *imgOut *= maxVal;
    sigma_r = tmpSigma_r;

    return imgOut;
}

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_BILATERAL_2DG_HPP */

