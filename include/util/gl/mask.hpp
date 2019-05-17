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

#ifndef PIC_UTIL_GL_MASK_HPP
#define PIC_UTIL_GL_MASK_HPP

#include "../../base.hpp"

namespace pic {

/**
 * @brief GenerateMask creates an opengl mask (a texture) from a buffer of bool values.
 * @param width
 * @param height
 * @param buffer
 * @param tex
 * @param tmpBuffer
 * @param mipmap
 * @return
 */
PIC_INLINE GLuint GenerateMask(int width, int height, bool *buffer = NULL,
                    GLuint tex = 0, unsigned char *tmpBuffer = NULL, bool mipmap = false)
{
    bool bGen = (tex == 0);

    if(bGen) {
        glGenTextures(1, &tex);
    }

    glBindTexture(GL_TEXTURE_2D, tex);

    if(bGen) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    unsigned char *data = NULL;

    if(buffer != NULL) {
        int n = width * height;

        if(tmpBuffer != NULL) {
            data = tmpBuffer;
        } else {
            data = new unsigned char[n * 3];
        }

        #pragma omp parallel for

        for(int i = 0; i < n; i++) {
            data[i] = buffer[i] ? 255 : 0;
        }
    }

    if(bGen) {

        if(mipmap) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    /*
        Note: GL_LUMINANCE is deprecated since OpenGL 3.1
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8 , width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    */

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    if(mipmap && bGen) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    if(data != NULL && tmpBuffer == NULL) {
        delete[] data;
    }

    return tex;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_ */
