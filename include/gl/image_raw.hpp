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

#ifndef PIC_GL_IMAGE_RAW_HPP
#define PIC_GL_IMAGE_RAW_HPP

#include "image_raw.hpp"
#include "gl.hpp"
#include "util/gl/fbo.hpp"
#include "util/gl/formats.hpp"
#include "util/gl/timings.hpp"

namespace pic {

enum IMAGESTORE {IMG_GPU_CPU, IMG_CPU_GPU, IMG_CPU, IMG_GPU, IMG_NULL};

class ImageRAWGL: public ImageRAW
{
protected:
    GLuint		texture;
    GLenum		target;
    IMAGESTORE	mode;	        //TODO: check if the mode is always correctly updated
    bool		notOwnedGL;     //do we own the OpenGL texture??

    Fbo			*tmpFbo;

    void	Destroy();

public:

    /**
     * @brief ImageRAWGL
     */
    ImageRAWGL();

    ~ImageRAWGL();

    /**
     * @brief ImageRAWGL
     * @param tex
     * @param target
     */
    ImageRAWGL(GLuint tex, GLenum target);

    /**
     * @brief ImageRAWGL
     * @param img
     * @param mipmap
     */
    ImageRAWGL(ImageRAW *img, bool mipmap);

    /**
     * @brief ImageRAWGL
     * @param nameFile
     */
    ImageRAWGL(std::string nameFile): ImageRAW(nameFile)
    {
        notOwnedGL = false;
        mode = IMG_CPU;
        texture = 0;
        target = 0;
        tmpFbo = NULL;
    }

    /**
     * @brief ImageRAW
     * @param frames
     * @param width
     * @param height
     * @param channels
     * @param data
     */
    ImageRAWGL(int frames, int width, int height, int channels, float *data) : ImageRAW (frames, width, height, channels, data)
    {
        notOwnedGL = false;
        mode = IMG_CPU;
        texture = 0;
        target = 0;
        tmpFbo = NULL;
    }

    /**
     * @brief ImageRAWGL
     * @param frames
     * @param width
     * @param height
     * @param channels
     * @param mode
     */
    ImageRAWGL(int frames, int width, int height, int channels, IMAGESTORE mode);

    /**
     * @brief AllocateSimilarOneGL
     * @return
     */
    ImageRAWGL *AllocateSimilarOneGL();

    /**
     * @brief CloneGL
     * @return
     */
    ImageRAWGL *CloneGL();

    /**
     * @brief AssignGL
     * @param r
     * @param g
     * @param b
     * @param a
     */
    void AssignGL(float r, float g, float b, float a);

    /**
     * @brief generateTextureGL
     * @param mipmap
     * @return
     */
    GLuint generateTextureGL(bool mipmap);

    /**
     * @brief generateTextureGLU32
     * @return
     */
    GLuint	generateTextureGLU32();

    /**
     * @brief loadFromMemory
     * @param mipmap
     */
    void loadFromMemory(bool mipmap);

    /**
     * @brief loadToMemory
     */
    void loadToMemory();

    /**
     * @brief generateTexture3DGL
     * @return
     */
    GLuint generateTexture3DGL();

    /**
     * @brief generateTextureCubeMapGL
     * @return
     */
    GLuint generateTextureCubeMapGL();

    /**
     * @brief generateTextureArrayGL
     * @return
     */
    GLuint generateTextureArrayGL();

    /**
     * @brief loadSliceIntoTexture
     * @param i
     */
    void loadSliceIntoTexture(int i);

    /**
     * @brief loadAllSlicesIntoTex
     */
    void loadAllSlicesIntoTex();

    /**
     * @brief readFromBindedFBO
     */
    void readFromBindedFBO();

    /**
     * @brief readFromFBO
     * @param fbo
     */
    void readFromFBO(Fbo *fbo);

    /**
     * @brief readFromFBO
     * @param fbo
     * @param format
     */
    void readFromFBO(Fbo *fbo, GLenum format);

    /**
     * @brief getTexture
     * @return
     */
    GLuint getTexture()
    {
        return texture;
    }

    /**
     * @brief bindTexture
     */
    void bindTexture();

    /**
     * @brief unBindTexture
     */
    void unBindTexture();

    /**
     * @brief updateModeGPU
     */
    void updateModeGPU()
    {
        if(mode == IMG_NULL) {
            mode = IMG_GPU;
        }

        if(mode == IMG_CPU) {
            mode = IMG_CPU_GPU;
        }
    }

    /**
     * @brief updateModeCPU
     */
    void updateModeCPU()
    {
        if(mode == IMG_NULL) {
            mode = IMG_CPU;
        }

        if(mode == IMG_GPU) {
            mode = IMG_CPU_GPU;
        }
    }

    /**
     * @brief SetTexture
     * @param texture
     */
    void SetTexture(GLuint texture)
    {
        //TODO: UNSAFE!
        this->texture = texture;
    }

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
    static GLuint GenerateMask(int width, int height, bool *buffer = NULL,
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8 , width, height, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, data);

        if(mipmap && bGen) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        if(data != NULL && tmpBuffer == NULL) {
            delete[] data;
        }

        return tex;
    }
};

ImageRAWGL::ImageRAWGL() : ImageRAW()
{
    notOwnedGL = false;
    texture = 0;
    target = 0;
    mode = IMG_NULL;
    tmpFbo = NULL;
}

ImageRAWGL::ImageRAWGL(GLuint tex, GLuint target) : ImageRAW()
{
    notOwnedGL = true;

    tmpFbo = NULL;

    mode = IMG_GPU;

    this->target = target;
    texture = tex;

    GLint internalFormat;

    switch(target) {
    case GL_TEXTURE_2D: {
        glBindTexture(GL_TEXTURE_2D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);

        frames = 1;

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    break;

    case GL_TEXTURE_CUBE_MAP: {

    }
    break;

    case GL_TEXTURE_2D_ARRAY: {
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &frames);
        glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);
        
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
    break;

    case GL_TEXTURE_3D: {
        glBindTexture(GL_TEXTURE_3D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &frames);
        glGetTexLevelParameteriv(GL_TEXTURE_3D, 0, GL_TEXTURE_INTERNAL_FORMAT,
                                 &internalFormat);

        channels = getChannelsFromInternalFormatGL(internalFormat);

        glBindTexture(GL_TEXTURE_3D, 0);
    }
    break;
    }

    AllocateAux();
}

ImageRAWGL::ImageRAWGL(ImageRAW *img, bool mipmap): ImageRAW()
{
    notOwnedGL = false;

    tmpFbo = NULL;

    width    = img->width;
    height   = img->height;
    frames   = img->frames;
    channels = img->channels;
    data     = img->data;

    CalculateStrides();

    target  = 0;
    texture = 0;

    if(frames > 1) {
        generateTexture3DGL();
    } else {
        generateTextureGL(mipmap);
    }

    mode = IMG_CPU_GPU;
}

ImageRAWGL::ImageRAWGL(int frames, int width, int height, int channels,
                       IMAGESTORE mode) : ImageRAW()
{
    notOwnedGL = false;
    tmpFbo = NULL;

    this->mode = mode;

    if(this->mode == IMG_GPU_CPU) {
        this->mode = IMG_CPU_GPU;
    }

    switch(this->mode) {
    case IMG_CPU_GPU: {
        Allocate(width, height, channels, frames);
        generateTextureGL(false);
    }
    break;

    case IMG_CPU: {
        Allocate(width, height, channels, frames);
    }
    break;

    case IMG_GPU: {
        this->width = width;
        this->height = height;
        this->frames = frames;
        this->depth = frames;
        this->channels = channels;

        AllocateAux();

        if(frames == 1) {
            generateTextureGL(false);
        } else {
            generateTexture3DGL();
        }
    }
    break;
    }
}

ImageRAWGL::~ImageRAWGL()
{
    Destroy();
}

ImageRAWGL *ImageRAWGL::CloneGL()
{
    //TODO: to improve CloneGL
    ImageRAW *tmp = this->Clone();
    return new ImageRAWGL(tmp, false);
}

void ImageRAWGL::Destroy()
{
    if(notOwnedGL) {
        return;
    }

    if(texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
        target = 0;
    }
}

ImageRAWGL *ImageRAWGL::AllocateSimilarOneGL()
{
#ifdef PIC_DEBUG
    printf("%d %d %d %d %d\n", frames, width, height, channels, mode);
#endif

    ImageRAWGL *ret = new ImageRAWGL(frames, width, height, channels, mode);
    return ret;
}

void ImageRAWGL::AssignGL(float r = 0.0f, float g = 0.0f, float b = 0.0f,
                          float a = 1.0f)
{
    if(tmpFbo == NULL) {
        tmpFbo = new Fbo();
        tmpFbo->create(width, height, 1, false, texture);
    }

    glClearColor(r, g, b, a);

    //Rendering
    tmpFbo->bind();
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glClear(GL_COLOR_BUFFER_BIT);

    //Fbo
    tmpFbo->unbind();
}

GLuint ImageRAWGL::generateTextureGL(bool mipmap = false)
{
    if(width <1 || height < 1 || channels < 1) {
        return 0;
    }

    updateModeGPU();

    target = GL_TEXTURE_2D;

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

GLuint	ImageRAWGL::generateTextureCubeMapGL()
{
    if(width <1 || height < 1 || channels < 1 || frames < 6) {
        return 0;
    }

    updateModeGPU();

    target = GL_TEXTURE_CUBE_MAP;

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Order Pos,Neg X,Y,Z
    for(int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, modeInternalFormat, width,
                     height, 0, mode, GL_FLOAT, &data[tstride * i]);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

void ImageRAWGL::loadFromMemory(bool mipmap = false)
{
    glBindTexture(target, texture);

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);
    glTexImage2D(target, 0, modeInternalFormat, width, height, 0,
                 mode, GL_FLOAT, data);

    glBindTexture(target, 0);
}

void ImageRAWGL::loadToMemory()
{
    if(texture == 0) {
        #ifdef PIC_DEBUG
            printf("This texture can not be trasferred from GPU memory\n");
        #endif
        return;
    }

    if(data == NULL) {
        #ifdef PIC_DEBUG
            printf("RAM memory allocated: %d %d %d %d\n", width, height, channels, frames);
        #endif

        Allocate(width, height, channels, frames);
        this->mode = IMG_CPU_GPU;
    }

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    bindTexture();

    glGetTexImage(target, 0, mode, GL_FLOAT, data);

    unBindTexture();
}

GLuint ImageRAWGL::generateTextureGLU32()
{
    if(width <1 || height < 1 || channels < 1) {
        return 0;
    }

    target = GL_TEXTURE_2D;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int *buffer = new int[width * height * channels];

    for(int i = 0; i < (width * height * channels); i++) {
        buffer[i] = lround(data[i]);
    }

    int mode, modeInternalFormat;
    getModesIntegerGL(channels, mode, modeInternalFormat);

    glTexImage2D(GL_TEXTURE_2D, 0, modeInternalFormat, width, height, 0,
                 mode, GL_INT, buffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] buffer;

    return texture;
}

GLuint ImageRAWGL::generateTexture3DGL()
{
    if(width <1 || height < 1 || channels < 1 || frames < 1) {
        return 0;
    }

    updateModeGPU();

    target = GL_TEXTURE_3D;

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

GLuint ImageRAWGL::generateTextureArrayGL()
{
    if(width <1 || height < 1 || channels < 1 || frames < 1) {
        return 0;
    }

    updateModeGPU();

    target = GL_TEXTURE_2D_ARRAY;

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

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

void ImageRAWGL::loadSliceIntoTexture(int i)
{
    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glBindTexture(target, texture);
    i = i % frames;
    glTexSubImage3D(target, 0, 0, 0, i, width, height, 1, mode, GL_FLOAT,
                    &data[i * tstride]);

    glBindTexture(target, 0);
}

void ImageRAWGL::loadAllSlicesIntoTex()
{
    for(int i = 0; i < frames; i++) {
        loadSliceIntoTexture(i);
    }
}

void ImageRAWGL::readFromFBO(Fbo *fbo, GLenum format)
{
    //TO DO: check data
    bool bCheck =	(fbo->width  != width) ||
                    (fbo->height != height);

    if(data == NULL || bCheck) {
        Allocate(fbo->width, fbo->height, 4, 1);
    }

    //ReadPixels from the FBO
    fbo->bind();
    glReadPixels(0, 0, width, height, format, GL_FLOAT, data);
    fbo->unbind();

    /*	glBindTexture(GL_TEXTURE_2D, fbo->tex);
    	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
    	glBindTexture(GL_TEXTURE_2D, 0);*/
}

void ImageRAWGL::readFromFBO(Fbo *fbo)
{
    if(mode == IMG_NULL) {
        mode = IMG_CPU;
    }

    readFromFBO(fbo, GL_RGBA);
}

void ImageRAWGL::readFromBindedFBO()
{

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    if(mode == 0x0) {
        #ifdef PIC_DEBUG
            printf("void ImageRAWGL::readFromBindedFBO(): error unknown format!");
        #endif
        return;
    }

    //TODO: check width height and data (mode and modeInternalFormat)

    glReadPixels(0, 0, width, height, mode, GL_FLOAT, data);
    FlipV();
}

void ImageRAWGL::bindTexture()
{
    glBindTexture(target, texture);
}

void ImageRAWGL::unBindTexture()
{
    glBindTexture(target, 0);
}

} // end namespace pic

#endif /* PIC_GL_IMAGE_RAW_HPP */

