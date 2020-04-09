/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_UTIL_RAW_HPP
#define PIC_UTIL_RAW_HPP

#include <string>
#include <iostream>
#include <vector>

#include "../base.hpp"
#include "../util/array.hpp"
#include "../util/file_lister.hpp"
#include "../util/string.hpp"

namespace pic {

enum RAW_type { RAW_U16_RGGB, RAW_U8_RGGB, RAW_DOUBLE, RAW_FLOAT};

template <class T>
class RAW: public Array<T>
{
protected:
    bool valid;

public:

    /**
     * @brief RAW
     */
    RAW() : Array<T>()
    {
        valid = false;
    }

    /**
     * @brief RAW
     * @param nData
     */
    RAW(int n): Array<T>(n)
    {
        valid = true;
    }

    /**
     * @brief RAW
     * @param nameFile
     * @param nData
     */
    RAW(std::string nameFile, int nData = -1)
    {
        valid = false;
        this->data = NULL;
        nData = 0;

        Read(nameFile, nData);
    }

    ~RAW()
    {
        this->release();
    }

    /**
     * @brief Read
     * @param nameFile
     * @param nData
     * @return
     */
    bool Read(std::string nameFile, int nData)
    {
        std::ifstream file;
        file.open(nameFile.c_str(), std::ios::in | std::ios::binary);

        if(!file.is_open()) {
            return false;
        }

        //Calculate length of the file
        file.seekg(0, std::ios::end);
        std::ios::streampos length = file.tellg();
        file.seekg(0, std::ios::beg);

        if(nData < 1) {
            nData = length / sizeof(T);
        }

        this->allocate(nData);

        file.read((char *)this->data, this->nData * sizeof(T) / sizeof(char));
        file.close();

        valid = true;
        return true;
    }

    /**
     * @brief Write
     * @param nameFile
     * @return
     */
    bool Write(std::string nameFile)
    {
        std::ofstream file;
        file.open(nameFile.c_str(), std::ios::binary);

        if(file.is_open()) {
            file.write((char *)this->data, this->nData * sizeof(T) / sizeof(char));
            file.close();
            valid = true;
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief getMeanRAWStack
     * @param stack
     * @return
     */
    static RAW<T> *getMeanRAWStack(std::vector<RAW<T> > &stack)
    {
        if(stack.size() <= 0) {
            return NULL;
        }

        int nData = stack[0].nData;
        RAW<T> *dataOut = new RAW<T>();
        dataOut->data = new T[nData];
        dataOut->nData = nData;

        int stackSize = stack.size();

        for(int i = 0; i < nData; i++) {
            unsigned long tmpData = 0;

            for(int j = 0; j < stackSize; j++) {
                tmpData += stack[j].data[i];
            }

            dataOut->data[i] = tmpData / stackSize;
        }

        dataOut->valid = true;
        return dataOut;
    }

    static unsigned long *getMeanRAWIterative(RAW<T> *img,
            unsigned long *dataAcc, bool bStart)
    {

        if(dataAcc == NULL) {
            dataAcc = new unsigned long[img->nData];
        }

        if(bStart) {
            for(int i = 0; i < img->nData; i++) {
                dataAcc[i] = img->data[i];
            }
        } else {
            for(int i = 0; i < img->nData; i++) {
                dataAcc[i] += img->data[i];
            }
        }

        return dataAcc;
    }

    /**
     * @brief getMeanRAWFromFile
     * @param nameDir
     * @param nameFilter
     * @param width
     * @param height
     * @return
     */
    static RAW<T> *getMeanRAWFromFile(
        std::string nameDir,
        std::string nameFilter,
        int width,
        int height)
    {
        StringVec vec;

        FileLister::getList(nameDir, nameFilter, &vec);

        RAW<T> imgRAW;
        unsigned long *dataAcc = NULL;

        for(unsigned int i = 0; i < vec.size(); i++) {
            imgRAW.Read(vec[i], width * height);
            dataAcc = getMeanRAWIterative(&imgRAW, dataAcc, i == 0);
        }

        RAW<T> *imgOut = imgRAW.copy();

        for(int i = 0; i < imgOut->nData; i++) {
            imgOut->data[i] = dataAcc[i] / vec.size();
        }

        return imgOut;
    }

    /**
     * @brief getMeanRAWFromFile
     * @param nameDir
     * @param nameFilter
     * @param nameOut
     * @param width
     * @param height
     */
    static void getMeanRAWFromFile(
        std::string nameDir,
        std::string nameFilter,
        std::string nameOut,
        int width,
        int height)
    {
        (getMeanRAWFromFile(nameDir, nameFilter, width,
                            height))->Write(nameOut);
    }
};


} // end namespace pic

#endif /* PIC_UTIL_RAW_HPP */

