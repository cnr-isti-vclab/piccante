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

#ifndef PIC_UTIL_CONVERT_RAW_TO_IMAGES_HPP
#define PIC_UTIL_CONVERT_RAW_TO_IMAGES_HPP

#include "util/string.hpp"
#include "image_raw.hpp"

namespace pic {

/**
 * @brief ConvertRAWtoImages converts .RAW into format
 * @param nameDirectory
 * @param format
 * @param width
 * @param height
 */
void ConvertRAWtoImages(std::string nameDirectory, std::string format,
                        int width, int height)
{
    StringVec vec;
    FileLister::List(nameDirectory, "raw", &vec);

    ImageRAW img;

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
void ConvertDetect(std::string nameDirectory, std::string format, int width,
                   int height)
{

    RAW<unsigned short> *imgMean = CalculateRAWMeanFromFile<unsigned short>
                                   (nameDirectory, "raw", width, height);

    StringVec vec;
    FileLister::List(nameDirectory, "raw", &vec);

    RAW<unsigned short> tmp;
    ImageRAW img;

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

