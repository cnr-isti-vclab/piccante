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

#include <QCoreApplication>

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    printf("Reading an HDR file...");

    pic::ImageRAW img;
    img.Read("../data/input/bottles.hdr");
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        pic::ImageRAWVec input = pic::Single(&img);
        pic::ImageRAW *output = NULL;

        //Filtering with the bilateral filter
        printf("Filtering the image with a Fast Bilateral filter;\n");
        printf("this has sigma_s = 4.0 and sigma_r = 0.05 ... ");

        pic::FilterBilateral2DS flt(4.0f, 0.05f);//creating the filter
        output = flt.ProcessP(input, output);//filtering the image

        printf("Ok!\n");

        printf("Writing the file to disk...");
        bool bWritten = output->Write("../data/output/filtered_bilateral.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //Filtering with the Anisotropic Diffusion
        printf("Filtering the image with the Anisotropic Diffusion;\n");
        printf("this has sigma_s = 4.0 and sigma_r = 0.05 ... ");
        output = pic::FilterAnsiotropicDiffusion::AnisotropicDiffusion(input, output, 4.0f, 0.05f);
        printf("Ok!\n");

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_anisotropic_diffusion.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //Filtering with WLS
        printf("Filtering the image with the WLS filter...");
        pic::FilterWLS fltWLS;//creating the filter
        output = fltWLS.ProcessP(input, output);//filtering the image
        printf("Ok!\n");

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_wls.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //Filering with Kuwahara
        printf("Filtering the image with the Kuwahara filter...");
        pic::FilterKuwahara fltK(11);
        output = fltK.ProcessP(input, output);//filtering the image

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_kuwahara.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

        //Filering with the Guided Filter
        printf("Filtering the image with the Guided filter...");
        pic::FilterGuided fltG;
        output = fltG.ProcessP(input, output);//filtering the image

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_guided.png");

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

    } else {
        printf("No it is not a valid file!\n");
    }
}
