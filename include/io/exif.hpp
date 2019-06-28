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

#ifndef PIC_IO_EXIF
#define PIC_IO_EXIF

#include <stdio.h>
#include <string>

#include "../base.hpp"

#include "../util/math.hpp"

namespace pic {

/**
 * @brief twoByteToValue
 * @param data
 * @param bMotorola
 * @return
 */
unsigned int twoByteToValue(unsigned char data[2], bool bMotorola)
{
    if(bMotorola) {
        return  (data[0] << 8) + (data[1]);
    } else {
        return  (data[1] << 8) + (data[0]);
    }
}

/**
 * @brief fourByteToValue
 * @param data
 * @param bMotorola
 * @return
 */
unsigned int fourByteToValue(unsigned char data[4], bool bMotorola)
{
    if(bMotorola) {
        return (data[0] << 24) + (data[1] << 16) +
               (data[2] << 8) + (data[3]);
    } else {
        return (data[3] << 24) + (data[2] << 16) +
               (data[1] << 8) + (data[0]);
    }
}

/**
 * @brief checkTag
 * @param tag
 * @param tag_r
 * @param bMotorola
 * @return
 */
bool checkTag(unsigned char tag[2], unsigned short tag_r, bool bMotorola)
{
    unsigned char tag_ref[2];
    tag_ref[0] = (tag_r >> 8) & 0x00ff;
    tag_ref[1] = tag_r & 0x00ff;

    bool bRet = false;
    if(bMotorola) {
        bRet = (tag[0] == tag_ref[0]) && (tag[1] == tag_ref[1]);
    } else {
        bRet = (tag[1] == tag_ref[0]) && (tag[0] == tag_ref[1]);
    }

    return bRet;
}

/**
 * @brief getTagID
 * @param tag
 * @param bMotorola
 * @return
 */
int getTagID(unsigned char tag[2], bool bMotorola)
{
    if(checkTag(tag, 0x829a, bMotorola)) {
        return 0;
    }

    if(checkTag(tag, 0x829d, bMotorola)) {
        return 1;
    }

    if(checkTag(tag, 0x8827, bMotorola)) {
        return 2;
    }

    if(checkTag(tag, 0x9202, bMotorola)) {
        return 3;
    }

    if(checkTag(tag, 0x920a, bMotorola)) {
        return 4;
    }

    return -1;
}

/**
 * @brief getBytesForComponents
 * @param value
 * @return
 */
int getBytesForComponents(int value)
{
    switch(value)
    {
    case 1: {
        return 1;
    } break;

    case 2: {
        return 1;
    } break;

    case 3: {
        return 2;
    } break;

    case 4: {
        return 4;
    } break;

    case 5: {
        return 8;
    } break;

    case 6: {
        return 1;
    } break;

    case 7: {
        return 1;
    } break;

    case 8: {
        return 2;
    } break;

    case 9: {
        return 4;
    } break;

    case 10: {
        return 8;
    } break;

    case 11: {
        return 4;
    } break;

    case 12: {
        return 8;
    } break;

    default: {
        return -1;
    }

    }
}

/**
 * @brief readString
 * @param file
 * @param length
 * @return
 */
std::string readString(FILE *file, int length)
{
    char *tmp = new char[length];
    fread(tmp, 1, length, file);
    std::string str(tmp);

    delete[] tmp;

    return str;
}

/**
 * @brief readStringFromUChar
 * @param data
 * @return
 */
std::string readStringFromUChar(unsigned char *data, int length)
{
    std::string str;

    for(int i = 0; i < length; i++) {
        str += (char) data[i];
    }

    return str;
}

/**
 * @brief readUnsignedRational
 * @param file
 * @param bMotorola
 * @return
 */
float readUnsignedRational(FILE *file, bool bMotorola)
{
    unsigned char val0[4];
    fread(val0, 1, 4, file);

    unsigned char val1[4];
    fread(val1, 1, 4, file);

    auto num = fourByteToValue(val0, bMotorola);
    auto denum = fourByteToValue(val1, bMotorola);

    return float(num) / float(denum);
}

struct EXIFInfo
{
    float exposureTime;
    float fNumber;
    float aperture;
    float iso;
    float focal_length;

    std::string camera_maker;
};

/**
 * @brief readEXIF
 * @param name
 * @param info
 * @return
 */
bool readEXIF(std::string name, EXIFInfo &info)
{
        FILE *file = fopen(name.c_str(), "rb");

        unsigned char buf[2];
        fread(buf, 1, 2, file);

        if(!checkTag(buf, 0xffd8, true)) {
            return false;
        }

        unsigned char buf2[2];
        int length = 0;

        bool bFound = false;
        while(!feof(file)) {
            fread(buf2, 1, 2, file);
            //printf("%x%x\n", buf2[0], buf2[1]);

            unsigned char len[2];
            fread(&len, 1, 2, file);

            //printf("LEN: %x %x\n", len[0], len[1]);
            length =  (len[0] << 8) + (len[1]);

            if(checkTag(buf2, 0xffe1, true)) {
                bFound = true;
                break;
            }

            fseek(file, length - 2, SEEK_CUR);
        }

        if(!bFound) {
            fclose(file);
            return false;
        }

        //EXIF header

        unsigned char buf6[6];
        fread(buf6, 1, 6, file);

        if(buf6[0] != 0x45 || buf6[1] != 0x78 ||
           buf6[2] != 0x69 || buf6[3] != 0x66 ||
           buf6[4] != 0x00 || buf6[5] != 0x00) {
            fclose(file);
            return false;
        }

        //TIFF header
        fpos_t pos;
        fgetpos(file, &pos);

        //is it Motorala mode?
        fread(buf2, 1, 2, file);
        bool bMotorola = (buf2[0] == 0x4d) && (buf2[1] == 0x4d);

        fread(buf2, 1, 2, file);
        bool bCheck = false;

        if(!checkTag(buf2, 0x002a, bMotorola)) {
            fclose(file);
            return false;
        }

        unsigned char buf4[4];
        fread(buf4, 1, 4, file); //this is the offset

        if(bMotorola) {
            bCheck = (buf4[0] == 0x00) && (buf4[1] == 0x00) &&
                     (buf4[2] == 0x00) && (buf4[3] == 0x08);
        } else {
            bCheck = (buf4[0] == 0x08) && (buf4[1] == 0x00) &&
                     (buf4[2] == 0x00) && (buf4[3] == 0x00);
        }

        if(!bCheck) {
            fclose(file);
            return false;
        }

        //IFD0: Image file directory
        fread(buf2, 1, 2, file);

        int nIFD = twoByteToValue(buf2, bMotorola);

        //printf("nIFD: %d\n", nIFD);

        unsigned int offset = 0;
        for(int i = 0; i < nIFD; i++) {
            unsigned char tag[2];
            fread(tag, 1, 2, file); //TAG

            unsigned char data_format[2];
            fread(data_format, 1, 2, file); //dataformat

            unsigned char num_components[4];
            fread(num_components, 1, 4, file); //number of components

            unsigned char data[4];
            fread(data, 1, 4, file); //data or offset to data

            //maker
            if(checkTag(tag, 0x010f, bMotorola)) {

                int df = twoByteToValue(data_format, bMotorola);
                int nc = fourByteToValue(num_components, bMotorola);

                int total_data_byte = getBytesForComponents(df) * nc;

                if(total_data_byte > 4) {
                    int offset = fourByteToValue(data, bMotorola);

                    fpos_t tmp_pos;
                    fgetpos(file, &tmp_pos);
                    fseek(file, offset, SEEK_CUR);
                    info.camera_maker = readString(file, nc);
                    fsetpos(file, &tmp_pos);

                } else {
                    info.camera_maker = readStringFromUChar(data, nc);
                }
            }

            if(checkTag(tag, 0x8769, bMotorola)) {
                offset = fourByteToValue(data, bMotorola);
            }
        }

        unsigned char next_IFD[4];
        fread(next_IFD, 1, 4, file);
        int offset_next_IFD = fourByteToValue(next_IFD, bMotorola);
        //printf("OFFSET: %d\n", offset_next_IFD);

        if(offset > 0) {
            fsetpos(file, &pos);
            fseek(file, offset, SEEK_CUR);
            //NOTE: this works but gives warnings --> fseek(file, pos + offset, SEEK_SET);           
        }

        //
        // IFD 1
        //

        fread(buf2, 1, 2, file);

        nIFD = twoByteToValue(buf2, bMotorola);

        for(int i = 0; i < nIFD; i++) {
            unsigned char tag[2];

            fread(tag, 1, 2, file); //TAG

            unsigned char data_format[2];
            fread(data_format, 1, 2, file); //dataformat

            unsigned char num_components[4];
            fread(num_components, 1, 4, file); //number of components

            unsigned char data[4];
            fread(data, 1, 4, file); //data or offset to data

            int df = twoByteToValue(data_format, bMotorola);
            int nc = fourByteToValue(num_components, bMotorola);
            int total_data_byte = getBytesForComponents(df) * nc;

            int id = getTagID(tag, bMotorola);

            float data_value = 0;
            if(total_data_byte > 4) {
                int offset = fourByteToValue(data, bMotorola);

                fpos_t tmp_pos;
                fgetpos(file, &tmp_pos);

                fsetpos(file, &pos);
                fseek(file, offset, SEEK_CUR);
                //fseek(file, pos + offset, SEEK_SET);

                switch(df) {
                case 5: {
                    data_value = readUnsignedRational(file, bMotorola);
                } break;
                }
                //unsigned rational

                fsetpos(file, &tmp_pos);
                //fseek(file, tmp_pos, SEEK_SET);
            } else {
                switch(df) {
                case 3: {
                    data_value = float(twoByteToValue(data, bMotorola));
                } break;
                }
            }

            switch(id) {
            case 0:
            {
                info.exposureTime = data_value;
            } break;

            case 1:
            {
                info.fNumber = data_value;

            } break;

            case 2:
            {
                info.iso = data_value;

            } break;

            case 3:
            {
                info.aperture = data_value;

            } break;

            case 4:
            {
                info.focal_length = data_value;
            } break;

            default:
            {

            } break;

            }
        }

        #ifdef PIC_DEBUG
            printf("%f %f %f\n", info.iso, info.exposureTime, info.fNumber);
        #endif

        fclose(file);

        return true;
}

} // end namespace pic

#endif /* PIC_IO_VOL_HPP */

