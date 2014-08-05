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

#ifndef PIC_IO_PPM_HPP
#define PIC_IO_PPM_HPP

#include <iostream>
#include <fstream>

#include "base.hpp"

namespace pic {

/**ReadPPM: reads an .ppm file*/
PIC_INLINE unsigned char *ReadPPM(std::string nameFile, unsigned char *data,
                                  int &width, int &height, int &channels)
{
    std::ifstream ppm_in(nameFile.c_str(), std::ios::binary);

    std::string magic_number("  ");

    ppm_in.get(magic_number[0]);
    ppm_in.get(magic_number[1]);

    bool bBinary = true;

    if(magic_number != std::string("P6")) {
        ppm_in.close();

        if(magic_number == std::string("P3")) {
            bBinary = false;
            ppm_in.open(nameFile.c_str(), std::ios::in);
            ppm_in.get(magic_number[0]);
            ppm_in.get(magic_number[1]);
        } else {
            return data;
        }
    }

    unsigned tmpWidth, tmpHeight, bpp;

    ppm_in >> tmpWidth >> tmpHeight >> bpp;

    if(bpp > 255) {
        printf("ERROR ReadPPM: BPP\n");
        return data;
    }

    channels = 3;

    //Allocate memory
    if(data == NULL) {
        data = new unsigned char[tmpWidth * tmpHeight * channels];
    }

    width = tmpWidth;
    height = tmpHeight;

    char ch;
    ppm_in.get(ch); // Trailing white space.

    for(int y = 0; y < height; y++) {
        int ind = y * width;

        for(int x = 0; x < width; x++) {
            int c = (ind + x) * 3;

            if(bBinary) {
                char r, g, b;
                ppm_in.get(r);
                ppm_in.get(g);
                ppm_in.get(b);

                data[c    ] = (static_cast<unsigned char>(r) * 255) / bpp;
                data[c + 1] = (static_cast<unsigned char>(g) * 255) / bpp;
                data[c + 2] = (static_cast<unsigned char>(b) * 255) / bpp;
            } else {
                int r, g, b;
                ppm_in >> r;
                ppm_in >> g;
                ppm_in >> b;

                data[c    ] = (r * 255) / bpp;
                data[c + 1] = (g * 255) / bpp;
                data[c + 2] = (b * 255) / bpp;
            }
        }
    }

    ppm_in.close();

    return data;
}

/**WritePPM: writes an .ppm file*/
PIC_INLINE bool WritePPM(std::string nameFile, const unsigned char *data,
                         int width, int height, int channels)
{
    std::ofstream ppm_out(nameFile.c_str(), std::ios::binary);

    if(!ppm_out.is_open()) {
        return false;
    }

    ppm_out << "P6";
    ppm_out << ' ';
    ppm_out << '\n';
    ppm_out << width;
    ppm_out << ' ';
    ppm_out << height;
    ppm_out << ' ';
    ppm_out << '\n';
    ppm_out << "255";
    ppm_out << '\n';

    int shiftG = 1;
    int shiftB = 2;

    if(channels == 1) {
        shiftG = 0;
        shiftB = 0;
    }

    for(int y = 0; y < height; y++) {
        int ind = y * width;

        for(int x = 0; x < width; x++) {
            int c = (ind + x) * channels;
            ppm_out << data[c  ];
            ppm_out << data[c + shiftG];
            ppm_out << data[c + shiftB];
        }
    }

    ppm_out.flush();
    ppm_out.close();
    return true;
}

} // end namespace pic

#endif /* PIC_IO_PPM_HPP */

