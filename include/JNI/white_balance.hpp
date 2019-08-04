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

#ifndef PIC_JNI_WHITE_BALANCE_HPP
#define PIC_JNI_WHITE_BALANCE_HPP

#include "../base.hpp"

#include "../filtering/filter_white_balance.hpp"

namespace pic {

/**
 * @brief applyWhiteBalanceJNI
 * @param imageInPath
 * @param imageOutPath
 * @param x
 * @param y
 * @param bRobust
 * @return
 */
PIC_INLINE int applyWhiteBalanceJNI(std::string imageInPath, std::string imageOutPath, int x, int y, bool bRobust = false)
{
    if(x < 0 || y < 0) {
        return 0;
    }

    Image in;
    bool bRead = in.Read(imageInPath, LT_NOR_GAMMA);

    if(bRead) {
        Image *out = FilterWhiteBalance::execute(&in, x, y, bRobust);

        bool bWrite = out->Write(imageOutPath.c_str(), LT_NOR_GAMMA, 0);

        if(!bWrite) {
            printf("applyWhiteBalanceJNI: the image could not be written.\n");
        }

        if(out != NULL) {
            delete out;
        }

        return bWrite ? 1 : 0;
    } else {
        printf("applyWhiteBalanceJNI: the image could not be read.\n");
        return 0;
    }
}

} // end namespace pic

#endif /* PIC_JNI_WHITE_BALANCE_HPP */

