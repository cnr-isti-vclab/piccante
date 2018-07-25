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

#ifndef PIC_IO_HDR_HPP
#define PIC_IO_HDR_HPP

#include <stdio.h>
#include <string.h>

#include "../colors/rgbe.hpp"
#include "../base.hpp"
//SYSTEM: X NEG Y POS

namespace pic {

/**
 * @brief ReadHDR reads a .hdr/.pic file.
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @return
 */
PIC_INLINE float *ReadHDR(std::string nameFile, float *data, int &width,
                          int &height)
{
    FILE *file = fopen(nameFile.c_str(), "rb");

    if(file == NULL) {
        return NULL;
    }

    char tmp[512];

    //Is it a Radiance file?
    fscanf(file, "%s\n", tmp);

    if(strcmp(tmp, "#?RADIANCE") != 0) {
        return NULL;
    }

    while(true) { //Reading Radiance Header
        std::string line = "";

        while(true) { //read property line
            char *tmp2 = fgets(tmp, 512, file);

            if(tmp2 == NULL) {
                return NULL;
            }

            line += tmp2;
            size_t pos = line.find("\n");

            if(pos != std::string::npos) {
                break;
            }
        }

        if(line.compare("\n") == 0) {
            break;
        }

        //Properties:
        if(line.find("FORMAT") != std::string::npos) { //Format
            if(line.find("32-bit_rle_rgbe") == std::string::npos) {
                return NULL;
            }
        }

        if(line.find("EXPOSURE=") != std::string::npos) { //Exposure
            //TODO: ...
        }
    }

    //width and height
    fscanf(file, "-Y %d +X %d", &height, &width);
    fgetc(file);

    if(data == NULL) {
        data = new float[width * height * 3];
    }

    //File size
    long int s_cur = ftell(file);
    fseek(file, 0 , SEEK_END);
    long int s_end = ftell(file);
    fseek(file, s_cur, SEEK_SET);
    int total = s_end - s_cur;

#ifdef PIC_DEBUG
    printf("%d %d\n", total, width * height * 4);
#endif

    //Compressed?
    if(total == (width * height * 4)) { //uncompressed
        unsigned char colRGBE[4];

        int c = 0;

        for(int i = 0; i < width; i++) {
            for(int j = 0; j < height; j++) {
                fread(colRGBE, 1, 4, file);
                fromRGBEToFloat(colRGBE, &data[c]);
                c += 3;
            }
        }
    } else { //RLE compressed
        unsigned char *buffer = new unsigned char[total];
        fread(buffer, sizeof(unsigned char)*total, 1, file);

        int line_width3 = width * 3;
        int line_width4 = width * 4;

        unsigned char *buffer_line_start;
        unsigned char *buffer_line = new unsigned char[line_width4];
        int c = 4;
        int c_buffer_line = 0;

        //for each line
        for(int i = 0; i < height; i++) {
            buffer_line_start = &buffer[c - 4];

            int width_check  = buffer_line_start[2];
            int width_check2 = buffer_line_start[3];

            bool b1 = buffer_line_start[0] != 2;
            bool b2 = buffer_line_start[1] != 2;
            bool b3 = width_check  != (width >> 8); 
            bool b4 = width_check2 != (width & 0xFF);

            if(b1 || b2 || b3 || b4) {
                #ifdef PIC_DEBUG
                    printf("ReadHDR ERROR: the file is not a RLE encoded .hdr file.\n");
                #endif

                fclose(file);

                return NULL;
            }

            for(int j = 0; j < 4; j++) {
                int k = 0;

                //decompression of a single channel line
                while(k < width) {
                    int num = buffer[c];

                    if(num > 128) {
                        num -= 128;

                        for(int l = k; l < (k + num); l++) {
                            buffer_line[l * 4 + j] = buffer[c + 1];
                        }

                        c += 2;
                        k += num;
                    } else {
                        for(int l = 0; l < num; l++) {
                            buffer_line[(l + k) * 4 + j] = buffer[c + 1 + l];
                        }

                        c += num + 1;
                        k += num;
                    }
                }
            }

            //From RGBE to Float
            for(int j = 0; j < width; j++) {
                fromRGBEToFloat(&buffer_line[j * 4], &data[c_buffer_line + j * 3]);
            }

            c += 4;
            c_buffer_line += line_width3;
        }

        delete[] buffer_line;
        delete[] buffer;
    }

    fclose(file);
    return data;
}

/**
 * @brief WriteLineHDR writes a scanline of an image using RLE and RGBE encoding.
 * @param file
 * @param buffer_line
 * @param width
 */
PIC_INLINE void WriteLineHDR(FILE *file, unsigned char *buffer_line, int width)
{
    int cur_pointer = 0;

    while(cur_pointer < width) {
        int run_length = 0;
        int run_length_old = 0;

        int run_start = cur_pointer;

        //we need to find a long run; length>3
        while((run_length < 4 ) && (run_start < width)) {
            run_start += run_length;
            run_length_old = run_length;

            int start = (run_start + 1);
            int end = MIN(run_start + 127, width); 
            unsigned char tmp = buffer_line[run_start];
            run_length = 1;

            //finding a run
            for(int i=start; i<end; i++) {
                if(tmp == buffer_line[i]) {
                    run_length++;
                } else {
                    break;
                }
            }
        }

        //do we have a short run <4 before a long one?
        if((run_length_old > 1) && (run_length_old == (run_start - cur_pointer))){
            unsigned char length_to_write = run_length_old + 128;
            unsigned char value_to_write = buffer_line[cur_pointer];
            fwrite(&length_to_write, sizeof(unsigned char), 1, file);
            fwrite(&value_to_write, sizeof(unsigned char), 1, file);

            cur_pointer = run_start;
        }

        //writing non-runs
        while(cur_pointer < run_start) {
            int non_run_length = run_start - cur_pointer;

            if(non_run_length > 128) {
                unsigned char length_to_write = 128;
                fwrite(&length_to_write, sizeof(unsigned char), 1, file);
                fwrite(&buffer_line[cur_pointer], sizeof(unsigned char)*length_to_write, 1, file);

                cur_pointer += length_to_write;
            } else {
                fwrite(&non_run_length, sizeof(unsigned char), 1, file);
                fwrite(&buffer_line[cur_pointer], sizeof(unsigned char)*non_run_length, 1, file);

                cur_pointer += non_run_length;
            }
        }

        //writing the found long run
        if(run_length > 3) {
            unsigned char length_to_write = run_length + 128;
            unsigned char value_to_write = buffer_line[run_start];
            fwrite(&length_to_write, sizeof(unsigned char), 1, file);
            fwrite(&value_to_write, sizeof(unsigned char), 1, file);

            cur_pointer += run_length;
        }

    }
}

/**
 * @brief WriteHDR  writes a .hdr/.pic file
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @param appliedExposure
 * @param bRLE
 * @return
 */
PIC_INLINE bool WriteHDR(std::string nameFile, float *data, int width,
                         int height, int channels, float appliedExposure = 1.0f, bool bRLE = true)
{
    FILE *file;

    if(data==NULL) {
        return false;
    }
    
    file = fopen(nameFile.c_str(), "wb");

    if( file == NULL) {
        return false;
    }

    if((channels == 2) || (channels == 0)) {
        return false;
    }

    //writing the header...
    fprintf(file, "#?RADIANCE\n");
    fprintf(file, "#Spiced by Piccante\n");
    fprintf(file, "FORMAT=32-bit_rle_rgbe\n");
    fprintf(file, "EXPOSURE= %f\n\n", appliedExposure);
    fprintf(file, "-Y %d +X %d\n", height, width);

    //RLE encoding is not allowed in some cases
    if(((width < 8) || (width > 32767)) && bRLE) {
        bRLE = false;
    }

    if(bRLE) {
        //buffers
        unsigned char *buffer_line = new unsigned char[width * 4];
        unsigned char buffer_rgbe[4];
        unsigned char buffer_line_start[4];

        //new line start "header"
        buffer_line_start[0] = 2;
        buffer_line_start[1] = 2;
        buffer_line_start[2] = width >> 8;
        buffer_line_start[3] = width & 0xFF;

        int width2 = width * 2;
        int width3 = width * 3;

        for(int i=0; i<height; i++) {
            int ind = i * width;

            //Converting the line data into the RGBE format
            for(int j = 0; j < width; j++) {
                int ind2 = (ind + j) * channels;

                if(channels == 1) {
                    fromSingleFloatToRGBE(&data[ind2], buffer_rgbe);
                } else {
                    fromFloatToRGBE(&data[ind2], buffer_rgbe);
                }

                buffer_line[         j] = buffer_rgbe[0];
                buffer_line[width  + j] = buffer_rgbe[1];
                buffer_line[width2 + j] = buffer_rgbe[2];
                buffer_line[width3 + j] = buffer_rgbe[3];
            }

            //Here a new line start
            fwrite(buffer_line_start, sizeof(unsigned char)*4, 1, file);

            //RLE encoding for each line
            for(int j=0; j<4; j++) {
                WriteLineHDR(file, &buffer_line[j * width], width);
            }
        }

    } else {
        unsigned char colRGBE[4];
        for(int j = 0; j < height; j++) {
            int ind = j * width;

            for(int i = 0; i < width; i++) {
                int c = (ind + i);

                if(channels == 3) {
                    c *= 3;
                    fromFloatToRGBE(&data[c], colRGBE);
                } else {
                    fromSingleFloatToRGBE(&data[c], colRGBE);
                }

                fwrite(colRGBE, 1, 4 * sizeof(unsigned char), file);
            }
        }
    }

    fclose(file);
    return true;
}

/**
 * @brief WriteHDRBlock writes a .hdr file.
 * @param nameFile
 * @param buffer_line
 * @param width
 * @param height
 * @param channels
 * @param blockID
 * @param nBlocks
 * @return
 */
PIC_INLINE bool WriteHDRBlock(std::string nameFile, float *buffer_line, int width,
                              int height, int channels, int blockID, int nBlocks)
{
    FILE *file;

    if((file = fopen(nameFile.c_str(), "wb")) == NULL || (buffer_line == NULL)) {
        return false;
    }

    //TODO: compressed version!

    //writing the header...
    if(nBlocks < 1) {
        nBlocks = 10;
    }

    int blockWidth = width / nBlocks;

    int xStart = blockWidth * blockID;
    int xEnd   = xStart + blockWidth;

    if(xEnd > width) {
        xEnd = width;
    }

    blockWidth = xEnd - xStart;

    fprintf(file, "#?RADIANCE\n");
    fprintf(file, "#Spiced by Piccante\n");
    fprintf(file, "FORMAT=32-bit_rle_rgbe\n");
    fprintf(file, "EXPOSURE= 1.0\n\n");
    fprintf(file, "-Y %d +X %d\n", height, blockWidth);

    unsigned char colRGBE[4];

    for(int j = 0; j < height; j++) {
        int ind = j * width;

        for(int i = xStart; i < xEnd; i++) {
            int c = (ind + i);

            if(channels == 3) {
                c *= 3;
                fromFloatToRGBE(&buffer_line[c], colRGBE);
            } else {
                fromSingleFloatToRGBE(&buffer_line[c], colRGBE);
            }

            fwrite(colRGBE, 1, 4 * sizeof(unsigned char), file);
        }
    }

    fclose(file);
    return true;
}

} // end namespace pic

#endif /* PIC_IO_HDR_HPP */

