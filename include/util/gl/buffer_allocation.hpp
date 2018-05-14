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

#ifndef PIC_UTIL_GL_BUFFER_ALLOCATION_HPP
#define PIC_UTIL_GL_BUFFER_ALLOCATION_HPP

#include "../../base.hpp"

#include "../../util/string.hpp"
#include "../../util/gl/quad.hpp"

namespace pic {

/**
 * @brief generateTexture2DGL
 * @param width
 * @param height
 * @param channels
 * @param data
 * @param mipmap
 * @return
 */
PIC_INLINE GLuint generateTexture2DGL(int width, int height, int channels, float *data = NULL, bool mipmap = false)
{
    if(width < 1 || height < 1 || channels < 1) {
        return 0;
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(mipmap) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, modeInternalFormat, width, height, 0,
                 mode, GL_FLOAT, data);

    if(mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

/**
 * @brief generateTextureCubeMapGL
 * @param width
 * @param height
 * @param channels
 * @param frames
 * @param data
 * @return
 */
PIC_INLINE GLuint generateTextureCubeMapGL(int width, int height, int channels, int frames, float *data = NULL)
{
    if(width < 1 || height < 1 || channels < 1 || frames < 6) {
        return 0;
    }

    GLuint texture;

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Order Pos, Neg and X, Y, Z
    int tstride = width * height * channels;

    for(int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, modeInternalFormat, width,
                     height, 0, mode, GL_FLOAT, &data[tstride * i]);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

/**
 * @brief generateTexture3DGL
 * @param width
 * @param height
 * @param channels
 * @param frames
 * @param data
 * @return
 */
PIC_INLINE GLuint generateTexture3DGL(int width, int height, int channels, int frames, float *data = NULL)
{
    if(width <1 || height < 1 || channels < 1 || frames < 1) {
        return 0;
    }

    GLuint texture;

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_3D, 0, modeInternalFormat, width, height, frames, 0,
                 mode, GL_FLOAT, data);

    glBindTexture(GL_TEXTURE_3D, 0);

//	for(int i=0;i<frames;i++)
//		glTexSubImage3D(GL_TEXTURE_3D,0,0,0,i,width,height,1,mode,GL_FLOAT,&data[i*tstride]);

    return texture;
}

/**
 * @brief generateTexture2DArrayGL
 * @param width
 * @param height
 * @param channels
 * @param frames
 * @param data
 * @return
 */
PIC_INLINE GLuint generateTexture2DArrayGL(int width, int height, int channels, int frames, float *data = NULL)
{
    if(width < 1 || height < 1 || channels < 1 || frames < 1) {
        return 0;
    }

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, modeInternalFormat, width, height, frames,
                 0, mode, GL_FLOAT, data);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    return texture;
}

/**
 * @brief generateTexture2DU32GL
 * @param width
 * @param height
 * @param channels
 * @return
 */
PIC_INLINE GLuint generateTexture2DU32GL(int width, int height, int channels, int *data = NULL)
{
    if(width < 1 || height < 1 || channels < 1) {
        return 0;
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int mode, modeInternalFormat;
    getModesIntegerGL(channels, mode, modeInternalFormat);

    glTexImage2D(GL_TEXTURE_2D, 0, modeInternalFormat, width, height, 0,
                 mode, GL_INT, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

} // end namespace pic

#endif /* PIC_UTIL_GL_BUFFER_ALLOCATION_HPP */
