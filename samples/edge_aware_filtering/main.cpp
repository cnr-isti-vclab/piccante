/*

PICCANTE
The hottest HDR imaging library!
http://piccantelib.net

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

    pic::Image img;
    img.Read("../data/input/bottles.hdr");
    printf("Ok\n");

    printf("Is it valid? ");
    if(img.isValid()) {
        printf("Ok\n");

        pic::ImageVec input = pic::Single(&img);
        pic::Image *output = NULL;

        //Filtering with the bilateral filter
        printf("Filtering the image with a Fast Bilateral filter;\n");
        printf("this has sigma_s = 4.0 and sigma_r = 0.05 ... ");

        //Creating the filter
        pic::FilterBilateral2DS flt(4.0f, 0.05f);

        //Filtering the image
        output = flt.ProcessP(input, output);

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

        //Filering with the Guided Filter
        printf("Filtering the image with the Guided filter...");
        pic::FilterGuided fltG;
        output = fltG.ProcessP(input, output);//filtering the image

        printf("Writing the file to disk...");
        bWritten = output->Write("../data/output/filtered_guided.png");

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

        if(bWritten) {
            printf("Ok\n");
        } else {
            printf("Writing had some issues!\n");
        }

    } else {
        printf("No it is not a valid file!\n");
    }
}
