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

#ifndef PIC_IO_PGM_HPP
#define PIC_IO_PGM_HPP

#include <iostream>
#include <fstream>

#include "../base.hpp"

namespace pic {

/**
 * @brief ReadPGM reads an .ppm file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE unsigned char *ReadPGM(std::string nameFile, unsigned char *data,
                                  int &width, int &height, int &channels)
{

    std::ifstream ppm_in(nameFile.c_str(), std::ios::binary);

    std::string magic_number("  ");

    ppm_in.get(magic_number[0]);
    ppm_in.get(magic_number[1]);

    bool bBinary = true;

    if(magic_number != std::string("P5")) {
        ppm_in.close();

        if(magic_number == std::string("P2")) {
            bBinary = false;
            ppm_in.open(nameFile.c_str(), std::ios::in);
            ppm_in.get(magic_number[0]);
            ppm_in.get(magic_number[1]);
        } else {
            return data;
        }
    }

    unsigned int tmpWidth, tmpHeight, bpp;

    ppm_in >> tmpWidth >> tmpHeight >> bpp;

    if(bpp > 255) {
        printf("ERROR ReadPGM: BPP\n");
        return data;
    }

    channels = 1;
    width = int(tmpWidth);
    height = int(tmpHeight);

    //Allocate memory
    if(data == NULL) {
        data = new unsigned char[width * height * channels];
    }

    for(int y = 0; y < height; y++) {
        int ind = y * width;

        for(int x = 0; x < width; x++) {
            int c = (ind + x);

            if(bBinary) {
                char value;
                ppm_in.get(value);
                data[c] = (static_cast<unsigned char>(value) * 255) / bpp;
            } else {
                int value;
                ppm_in >> value;
                data[c] = (static_cast<unsigned char>(value) * 255) / bpp;
            }
        }
    }

    ppm_in.close();

    return data;
}

/**
 * @brief WritePGM writes an .ppm file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @param bAscii
 * @return
 */
PIC_INLINE bool WritePGM(std::string nameFile, const unsigned char *data,
                         int width, int height, int channels, bool bAscii = false)
{
    std::ofstream pgm_out;

    if(bAscii) {
         pgm_out.open(nameFile.c_str(), std::ios::out);
    } else {
         pgm_out.open(nameFile.c_str(), std::ios::binary);
    }

    if(!pgm_out.is_open()) {
        return false;
    }

    if(bAscii){
        pgm_out << "P2";
    } else {
        pgm_out << "P5";
    }

    pgm_out << ' ';
    pgm_out << '\n';
    pgm_out << width;
    pgm_out << ' ';
    pgm_out << height;
    pgm_out << ' ';
    pgm_out << '\n';
    pgm_out << "255";
    pgm_out << '\n';

    for(int y = 0; y < height; y++) {
        int ind = y * width;

        for(int x = 0; x < width; x++) {
            int c = (ind + x) * channels;

            if(bAscii) {
                int value = data[c];
                pgm_out << value << ' ';
            } else {
                pgm_out << data[c];
            }
        }
        if(bAscii) {
            pgm_out << '\n';
        }
    }

    pgm_out.flush();
    pgm_out.close();
    return true;
}

} // end namespace pic

#endif /* PIC_IO_PPM_HPP */

