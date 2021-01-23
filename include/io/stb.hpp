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

#ifndef PIC_IO_STB_HPP
#define PIC_IO_STB_HPP

#include <stdio.h>
#include <string>

#include "../base.hpp"


#ifndef PIC_STB_DISABLE
    #define PIC_STB
    #define STBIWDEF inline
    #define STB_IMAGE_STATIC
    #define STB_IMAGE_WRITE_STATIC
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION

#ifndef PIC_STB_NOT_INSTALLED_LOCAL
    #include "../../stb/stb_image_write.h"
    #include "../../stb/stb_image.h"
#else
    #include <stb/stb_image_write.h>
    #include <stb/stb_image.h>
#endif

#endif


namespace pic {

/**
 * @brief ReadSTB
 * @param nameFile
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE unsigned char *ReadSTB(std::string nameFile, int &width,
                          int &height, int &channels)
{
    unsigned char *data = NULL;
    
#ifndef PIC_STB_DISABLE
    int w, h, c;
    stbi_info(nameFile.c_str(), &w, &h, &c);
    data = stbi_load(nameFile.c_str(), &width, &height, &channels, c);
#endif
    
    return data;
}

/**
 * @brief WriteSTB
 * @param nameFile
 * @param data
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE bool WriteSTB(std::string nameFile, unsigned char *data, int width, int height,
                int channels = 3)
{
    int tmp = 0;

    #ifndef PIC_STB_DISABLE
        tmp = stbi_write_png(nameFile.c_str(), width, height, channels, (void*) data, 0);
    #endif
    
    return (tmp == 1);
}

} // end namespace pic

#endif /* PIC_IO_STB_HPP */

