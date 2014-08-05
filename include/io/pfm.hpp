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

#ifndef PIC_IO_PFM_HPP
#define PIC_IO_PFM_HPP

#include <stdio.h>
#include <string>

#include "base.hpp"

namespace pic {

/** ReadPFM: reads a .pfm file*/
PIC_INLINE float *ReadPFM(std::string nameFile, float *data, int &width,
                          int &height)
{
    FILE *file = fopen(nameFile.c_str(), "rb");

    if(file == NULL) {
        return NULL;
    }

    char  flagc;
    float flag;
    fgetc(file);
    fgetc(file);
    fgetc(file);
    fscanf(file, "%d %d%c", &width, &height, &flagc);
    fscanf(file, "%f%c", &flag, &flagc);

    if(data == NULL) {
        data = new float[width * height * 3];
    }

    for(int i = height - 1; i > -1; i--) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int tmpInd = (ind + j) * 3;

            fread(&data[tmpInd], sizeof(float), 3, file);
        }
    }

    fclose(file);
    return data;
}

/** WritePFM: writes a .pfm file*/
PIC_INLINE bool WritePFM(std::string nameFile, const float *data, int width,
                         int height, int channels = 3)
{
    if((data == NULL) || (height < 1) || (width < 1) || (channels < 1)) {
        return false;
    }

    FILE *file = fopen(nameFile.c_str(), "wb");

    if(file == NULL) {
        return false;
    }

    //header
    fputc('P', file);
    fputc('F', file);
    fputc(0x0a, file);

    //width and height
    fprintf(file, "%d %d", width, height);
    fputc(0x0a, file);

    //flag
    fprintf(file, "%f", -1.0f);
    fputc(0x0a, file);

    //data
    int ind1 = 1;
    int ind2 = 2;

    if(channels == 1) {
        ind1 = 0;
        ind2 = 0;
    }

    if(channels == 2) {
        ind1 = 1;
        ind2 = 1;
    }

    for(int i = height - 1; i > -1; i--) {
        int ind = i * width;

        for(int j = 0; j < width; j++) {
            int tmpInd = (ind + j) * channels;

            fwrite(&data[tmpInd     ], sizeof(float), 1, file);
            fwrite(&data[tmpInd + ind1], sizeof(float), 1, file);
            fwrite(&data[tmpInd + ind2], sizeof(float), 1, file);
        }
    }

    fclose(file);
    return true;
}

} // end namespace pic

#endif /* PIC_IO_PFM_HPP */

