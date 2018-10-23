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
        return  (data[0] << 8) + (data[1]);
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
    tag_ref[0] = (tag_r >> 256) & 0x00ff;
    tag_ref[1] = tag_r & 0x00ff;

    bool bRet = false;
    if(bMotorola) {
        bRet = (tag[0] == tag_ref[0]) && (tag[1] == tag_ref[1]);
    } else {
        bRet = (tag[1] == tag_ref[0]) && (tag[1] == tag_ref[0]);
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
};

bool readEXIF(std::string name, EXIFInfo &info)
{
        FILE *file = fopen(name.c_str(), "rb");

        unsigned char buf[2];
        fread(buf, 1, 2, file);

        if(buf[0] != 0xff || buf[1] != 0xD8) {
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

            if((buf2[0] == 0xff && buf2[1] == 0xe1)) {
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

        if(bMotorola) {
            bCheck = (buf2[0] == 0x00) && (buf2[1] == 0x2a);
        } else {
            bCheck = (buf2[0] == 0x2a) && (buf2[1] == 0x00);
        }

        if(!bCheck) {
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

        int nIFD = 0;

        if(bMotorola) {
            nIFD = (buf2[0] << 8) + (buf2[1]);
        } else {
            nIFD = (buf2[0]) + (buf2[1] << 8);
        }
        //printf("nIFD: %d\n", nIFD);

        unsigned int offset;
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
            if(tag[0] == 0x01 && tag[1] == 0x0f) {

                int df = twoByteToValue(data_format, bMotorola);
                int nc = fourByteToValue(num_components, bMotorola);

                int total_data_byte = getBytesForComponents(df) * nc;

                if(total_data_byte > 4) {
                    int offset = fourByteToValue(data, bMotorola);

                    fpos_t tmppos;
                    fgetpos(file, &tmppos);


                    fseek(file, pos + offset, SEEK_SET);

                    for(int j = 0; j < nc; j++) {
                        unsigned char tmpj;
                        fread(&tmpj, 1, 1, file);
                        printf("%c", tmpj);
                    }
                    printf("\n");

                    fseek(file, tmppos, SEEK_SET);
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
            fseek(file, pos + offset, SEEK_SET);
        }

        //
        // IFD 1
        //

        fread(buf2, 1, 2, file);

        nIFD = 0;

        if(bMotorola) {
            nIFD = (buf2[0] << 8) + (buf2[1]);
        } else {
            nIFD = (buf2[0]) + (buf2[1] << 8);
        }

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

            float data_value;
            if(total_data_byte > 4) {
                int offset = fourByteToValue(data, bMotorola);

                fpos_t tmp_pos;
                fgetpos(file, &tmp_pos);
                fseek(file, pos + offset, SEEK_SET);

                data_value = readUnsignedRational(file, bMotorola);
                //unsigned rational

                fseek(file, tmp_pos, SEEK_SET);
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

            default:
            {

            } break;

            }
        }

        fclose(file);

        return true;
}

} // end namespace pic

#endif /* PIC_IO_VOL_HPP */

