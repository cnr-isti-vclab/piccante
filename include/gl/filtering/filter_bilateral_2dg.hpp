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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_2DG_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_2DG_HPP

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_slicer.hpp"
#include "gl/filtering/filter_scatter.hpp"
#include "gl/filtering/filter_gaussian_3d.hpp"

namespace pic {

class FilterGLBilateral2DG: public FilterGL
{
protected:
    float				sigma_s, sigma_r, s_S, s_R;
    int					width, height;

    FilterGLScatter		*scatter;
    FilterGLGaussian3D	*gauss3D;

    ImageRAWGL			*gridGL, *gridBlurGL;

public:
    FilterGLSlicer		*slicer;

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
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);

    /**
     * @brief Execute
     * @param imgIn
     * @param sigma_s
     * @param sigma_r
     * @return
     */
    static ImageRAWGL *Execute(ImageRAWGL *imgIn, float sigma_s, float sigma_r)
    {
        FilterGLBilateral2DG *filter = new FilterGLBilateral2DG(sigma_s, sigma_r);
        GLuint testTQ1 = glBeginTimeQuery();
        ImageRAWGL *imgOut = filter->Process(SingleGL(imgIn), NULL);
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);

        printf("Bilateral 2DG Filter on GPU time: %f ms\n",
               double(timeVal) / 1000000.0);

        return imgOut;
    }

    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut,
                               float sigma_s, float sigma_r, int testing = 1)
    {
        ImageRAWGL imgIn(nameIn);
        float maxVal = imgIn.getMaxVal()[0];
        imgIn.Div(maxVal);
        sigma_r = sigma_r / maxVal;

        imgIn.generateTextureGL(false, GL_TEXTURE_2D);

        FilterGLBilateral2DG *filter = new FilterGLBilateral2DG(sigma_s, sigma_r);//, imgIn.channels);

        ImageRAWGL *imgOut = new ImageRAWGL(1, imgIn.width, imgIn.height, 4,
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

        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);

        double ms = double(timeVal) / (double(testing) * 1000000.0);
        printf("Bilateral Grid on the GPU time: %g ms\n", ms);

        std::string sign = GenBilString("G", sigma_s, sigma_r);

        std::string nameTime = FileLister::FileNumber(sign, "txt");

        FILE *file = fopen(nameTime.c_str(), "w");

        if(file != NULL) {
            fprintf(file, "%f", ms);
            fclose(file);
        }

        //Read from the GPU
        imgOut->loadToMemory();
        imgOut->Mul(maxVal);
        imgOut->Write(nameOut);

        return imgOut;
    }
};

FilterGLBilateral2DG::FilterGLBilateral2DG(float sigma_s, float sigma_r): FilterGL()
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

ImageRAWGL *FilterGLBilateral2DG::Process(ImageRAWGLVec imgIn,
        ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL || imgIn.size() > 1) {
        return imgOut;
    }

    if(imgOut == NULL) {
        imgOut = imgIn[0]->AllocateSimilarOneGL();
    }

    if(scatter == NULL) {
        scatter = new FilterGLScatter(s_S, s_R, imgIn[0]->width, imgIn[0]->height);
    }

    //Splatting
    gridGL = scatter->Process(imgIn, gridGL);

    gridGL->loadToMemory();
    gridGL->SetRand();
    gridGL->loadFromMemory();

    //Blurring
    gridBlurGL = gauss3D->Process(SingleGL(gridGL), gridBlurGL);

    //Slicing
    imgOut = slicer->Process(DoubleGL(imgIn[0], gridBlurGL), imgOut);

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_2DG_HPP */

