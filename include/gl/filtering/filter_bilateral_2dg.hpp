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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DG_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DG_HPP

#include "../../base.hpp"

#include "../../gl/filtering/filter.hpp"
#include "../../gl/filtering/filter_slicer.hpp"
#include "../../gl/filtering/filter_scatter.hpp"
#include "../../gl/filtering/filter_gaussian_3d.hpp"

namespace pic {

/**
 * @brief The FilterGLBilateral2DG class
 */
class FilterGLBilateral2DG: public FilterGL
{
protected:
    float sigma_s, sigma_r, s_S, s_R;

    FilterGLScatter     *scatter;
    FilterGLGaussian3D  *gauss3D;
    ImageGL             *gridGL, *gridBlurGL;

public:
    FilterGLSlicer      *slicer;

    /**
     * @brief FilterGLBilateral2DG
     * @param sigma_s
     * @param sigma_r
     * @param width
     * @param height
     */
    FilterGLBilateral2DG(float sigma_s, float sigma_r);

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);

    /**
     * @brief execute
     * @param imgIn
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, float sigma_s, float sigma_r)
    {
        FilterGLBilateral2DG *filter = new FilterGLBilateral2DG(sigma_s, sigma_r);
        GLuint testTQ1 = glBeginTimeQuery();
        ImageGL *imgOut = filter->Process(SingleGL(imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);

        printf("Bilateral 2DG Filter on GPU time: %f ms\n",
               double(timeVal) / 1000000.0);

        return imgOut;
    }

    static ImageGL *execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, int testing = 1)
    {
        Image tmp_imgIn(nameIn);
        float maxVal = tmp_imgIn.getMaxVal()[0];
        tmp_imgIn /= maxVal;
        sigma_r = sigma_r / maxVal;

        ImageGL imgIn(&tmp_imgIn, true);
        imgIn.generateTextureGL();

        FilterGLBilateral2DG *filter = new FilterGLBilateral2DG(sigma_s, sigma_r);//, imgIn.channels);

        ImageGL *imgOut = new ImageGL(1, imgIn.width, imgIn.height, 4,
                                      IMG_GPU_CPU, GL_TEXTURE_2D);

        GLuint testTQ1;

        if(testing > 1) {
            filter->Process(SingleGL(&imgIn), imgOut);

            testTQ1 = glBeginTimeQuery();

            for(int i = 0; i < testing; i++) {
                filter->Process(SingleGL(&imgIn), imgOut);
            }
        } else {
            testTQ1 = glBeginTimeQuery();
            filter->Process(SingleGL(&imgIn), imgOut);
        }

       // GLuint64EXT timeVal = glEndTimeQuery(testTQ1);

      //  double ms = double(timeVal) / (double(testing) * 1000000.0);
      //  printf("Bilateral Grid on the GPU time: %g ms\n", ms);

        std::string sign = genBilString("G", sigma_s, sigma_r);

        /*
        std::string nameTime = FileLister::FileNumber(sign, "txt");

        FILE *file = fopen(nameTime.c_str(), "w");

        if(file != NULL) {
            fprintf(file, "%f", ms);
            fclose(file);
        }*/

        //Read from the GPU
        *imgOut /= maxVal;
        imgOut->loadToMemory();
        imgOut->Write(nameOut);

        return imgOut;
    }
};

PIC_INLINE FilterGLBilateral2DG::FilterGLBilateral2DG(float sigma_s, float sigma_r): FilterGL()
{
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;

    s_S = 1.0f / sigma_s; //Spatial Sampling rate
    s_R = 1.0f / sigma_r; //Range Sampling rate

    gridGL = NULL;
    gridBlurGL = NULL;

    scatter = NULL;
    gauss3D = new FilterGLGaussian3D(1.0f);
    slicer  = new FilterGLSlicer(s_S, s_R);
}

PIC_INLINE ImageGL *FilterGLBilateral2DG::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL || imgIn.size() > 1) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = imgIn[0]->allocateSimilarOneGL();
    }

    if(scatter == NULL) {
        scatter = new FilterGLScatter(s_S, s_R, imgIn[0]->width, imgIn[0]->height);
    }

    //splat
    gridGL = scatter->Process(imgIn, gridGL);

    //blur
    gridBlurGL = gauss3D->Process(SingleGL(gridGL), gridBlurGL);

    //slice
    imgOut = slicer->Process(DoubleGL(imgIn[0], gridBlurGL), imgOut);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DG_HPP */

