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

#ifndef PIC_UTIL_CONVERT_RAW_TO_IMAGES_HPP
#define PIC_UTIL_CONVERT_RAW_TO_IMAGES_HPP

#include "../base.hpp"
#include "../util/raw.hpp"
#include "../util/string.hpp"
#include "../image.hpp"

namespace pic {

/**
 * @brief ConvertRAWtoImages converts .RAW into format
 * @param nameDirectory
 * @param format
 * @param width
 * @param height
 */
PIC_INLINE void ConvertRAWtoImages(std::string nameDirectory, std::string format,
                        int width, int height)
{
    StringVec vec;
    FileLister::List(nameDirectory, "raw", &vec);

    Image img;

    for(unsigned int i = 0; i < vec.size(); i++) {
        img.ReadRAW(vec[i], "NULL", RAW_U16_RGGB, width, height);

        std::string out = removeExtension(vec[i]);
        out += ".";
        out += format;
        printf("%s\n", out.c_str());
        img.Write(out);
    }
}

/**
 * @brief ConvertDetect
 * @param nameDirectory
 * @param format
 * @param width
 * @param height
 */
PIC_INLINE void ConvertDetect(std::string nameDirectory, std::string format, int width,
                   int height)
{

    RAW<unsigned short> *imgMean = CalculateRAWMeanFromFile<unsigned short>
                                   (nameDirectory, "raw", width, height);

    StringVec vec;
    FileLister::List(nameDirectory, "raw", &vec);

    RAW<unsigned short> tmp;
    Image img;

    for(unsigned int i = 0; i < vec.size(); i++) {
        tmp.Read(vec[i], width * height);

        for(int j = 0; j < tmp.nData; j++) {
            int diff = (tmp.data[j] - imgMean->data[j]);
            diff = diff > 0 ? diff : -diff;

            if(diff > 6200) {
                tmp.data[j] = 10000;
            } else {
                tmp.data[j] = 0;
            }
        }

        tmp.Write("tmpRAW.raw");
        img.ReadRAW("tmpRAW.raw", "NULL", RAW_U16_RGGB, width, height);

        std::string out = vec[i];
        out += ".";
        out += format;
        printf("%s\n", out.c_str());
        img.Write(out);
    }
}

} // end namespace pic

#endif /* PIC_UTIL_CONVERT_RAW_TO_IMAGES_HPP */

