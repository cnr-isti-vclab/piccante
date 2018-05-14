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

#ifndef PIC_IO_VOL_HPP
#define PIC_IO_VOL_HPP

#include <stdio.h>
#include <string>

#include "../base.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief ReadVOL
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param depth
 * @param channels
 * @return
 */
PIC_INLINE float *ReadVOL(std::string nameFile, float *data, int &width,
                          int &height, int &depth, int &channels)
{
    FILE *file = fopen(nameFile.c_str(), "rb");

    if(file == NULL) {
        return NULL;
    }

    //File size
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file) / 3;
    rewind(file);

    //Check size
    int c64  = 64 * 64 * 64;
    int c128 = 128 * 128 * 128;
    int c256 = 256 * 256 * 256;

    if(fileSize != c64 && fileSize != c128 && fileSize != c256) {
        return NULL;
    }

    int len;
    len = fileSize == c64  ?  64 : 128;
    len = fileSize == c128 ? 128 : len;
    len = fileSize == c256 ? 256 : len;

    width	= len;
    height	= len;
    depth	= len;

    if(data == NULL) {
        data = new float[len * len * len * 4];
    }

    unsigned char *tmpData = new unsigned char[fileSize * 3];

    fread(tmpData, sizeof(unsigned char), fileSize * 3, file);

    int ind0, ind1;

    for(int i = 0; i < len; i++) {
        int tmpI = len * len * i;

        for(int j = 0; j < len; j++) {
            int tmpJ = tmpI + len * j;

            for(int k = 0; k < len; k++) {
                ind0 = (tmpJ + k) * 3;
                ind1 = (tmpJ + k) * channels;
                data[ind1    ] = float(tmpData[ind0    ]) / 255.0f;
                data[ind1 + 1] = float(tmpData[ind0 + 1]) / 255.0f;
                data[ind1 + 2] = float(tmpData[ind0 + 2]) / 255.0f;
                data[ind1 + 3] = sqrtf(data[ind1    ] * data[ind1    ] +
                                       data[ind1 + 1] * data[ind1 + 1] +
                                       data[ind1 + 2] * data[ind1 + 2]);
//				data[ind1+3] = 1.0f;
            }
        }
    };

    delete[] tmpData;

    fclose(file);

    return data;
}

/**
 * @brief WriteVOL
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param depth
 * @param channels
 * @return
 */
PIC_INLINE bool WriteVOL(std::string nameFile, float *data, int width, int height,
               int depth, int channels = 3)
{

    FILE *file = fopen(nameFile.c_str(), "wb");

    if(file == NULL) {
        return false;
    }

    int tot = width * height * depth * 3;

    unsigned char *tmpData = new unsigned char[tot];

    int sh1 = 0;
    int sh2 = 0;

    if(channels == 2) {
        sh1 = 1;
        sh2 = 1;
    }

    if(channels > 2) {
        sh1 = 1;
        sh2 = 2;
    }

    for(int i = 0; i < tot; i += 3) {
        int j = (i / 3) * channels;
        tmpData[i    ] = int(CLAMPi(data[j      ] * 255.0f, 0.0f, 255.0f));
        tmpData[i + 1] = int(CLAMPi(data[j + sh1] * 255.0f, 0.0f, 255.0f));
        tmpData[i + 2] = int(CLAMPi(data[j + sh2] * 255.0f, 0.0f, 255.0f));
    }

    fwrite(tmpData, sizeof(unsigned char), tot, file);

    fclose(file);

    return true;
}

} // end namespace pic

#endif /* PIC_IO_VOL_HPP */

