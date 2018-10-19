/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/tommaseo_statue.png";
    }

    printf("Reading an image...");
    pic::Image img;
    img.Read(img_str);
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        pic::ImageVec input = pic::Single(&img);
        pic::Image *output = NULL;

        bool bWritten;

        //the median filter
        printf("Filtering the image with the Median filter (radius of 3);\n");

        pic::FilterMed fltM(7);
        output = fltM.Process(input, output);

        printf("Ok!\n");

        printf("Writing the file to disk...");

        bWritten = output->Write("../data/output/filtered_median.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //the vector median filter
        printf("Filtering the image with the Vector Median filter (radius of 3);\n");

        pic::FilterMedVec fltMV(7);
        output = fltMV.Process(input, output);

        printf("Ok!\n");

        printf("Writing the file to disk...");

        bWritten = output->Write("../data/output/filtered_median_vec.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //the bilateral filter
        printf("Filtering the image with a Fast Bilateral filter;\n");
        printf("this has sigma_s = 4.0 and sigma_r = 0.05 ... ");

        pic::FilterBilateral2DS flt(8.0f, 0.05f);
        output = flt.Process(input, output);

        printf("Ok!\n");

        printf("Writing the file to disk...");

        bWritten = output->Write("../data/output/filtered_bilateral.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //the Anisotropic Diffusion
        printf("Filtering the image with the Anisotropic Diffusion;\n");
        printf("this has sigma_s = 4.0 and sigma_r = 0.05 ... ");
        output = pic::FilterAnsiotropicDiffusion::AnisotropicDiffusion(input, output, 8.0f, 0.05f);
        printf("Ok!\n");

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_anisotropic_diffusion.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //the Guided Filter
        printf("Filtering the image with the Guided filter...");
        pic::FilterGuided fltG;
        output = fltG.Process(input, output);//filtering the image

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_guided.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //WLS
        printf("Filtering the image with the WLS filter...");
        pic::FilterWLS fltWLS;//creating the filter
        output = fltWLS.Process(input, output);
        printf("Ok!\n");

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_wls.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //Kuwahara
        printf("Filtering the image with the Kuwahara filter...");
        pic::FilterKuwahara fltK(11);
        output = fltK.Process(input, output);

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_kuwahara.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

    } else {
        printf("No it is not a valid file!\n");
    }
}
