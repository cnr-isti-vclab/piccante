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

#ifndef PIC_GL_IMAGE_RAW_HPP
#define PIC_GL_IMAGE_RAW_HPP

#include "image.hpp"

#include "gl.hpp"
#include "util/gl/fbo.hpp"
#include "util/gl/formats.hpp"
#include "util/gl/timings.hpp"
#include "util/gl/buffer_ops.hpp"
#include "util/gl/buffer_allocation.hpp"
#include "util/gl/mask.hpp"
#include "util/gl/redux.hpp"
#include "util/gl/redux_ops.hpp"

namespace pic {

enum IMAGESTORE {IMG_GPU_CPU, IMG_CPU_GPU, IMG_CPU, IMG_GPU, IMG_NULL};

/**
 * @brief The ImageGL class
 */
class ImageGL: public Image
{
protected:
    GLuint		texture;
    GLenum		target;
    IMAGESTORE	mode;	        //TODO: check if the mode is always correctly updated
    bool		notOwnedGL;     //do we own the OpenGL texture??    
    Fbo			*tmpFbo;

    //stack for statistics
    std::vector<GLuint> stack;

    /**
     * @brief DestroyGL
     */
    void	DestroyGL();

    /**
     * @brief AssignGL assigns an (r, g, b, a) value to an image using glClearColor.
     * @param r is the value for the red channel.
     * @param g is the value for the green channel.
     * @param b is the value for the blue channel.
     * @param a is the value for the alpha channel.
     */
    void AssignGL(float r = 0.0f, float g = 0.0f, float b = 0.0f,
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

    /**
     * @brief thisOperatorConst
     * @param a
     * @param op
     */
    inline void thisOperatorConst(const float &a, BOGL op)
    {
        BufferOpsGL *ops = BufferOpsGL::getInstance();

        ops->list[op]->Update(a);
        ops->list[op]->Process(getTexture(), 0, getTexture(), width, height);
    }

    /**
     * @brief thisOperatorImage
     * @param a
     * @param op
     */
    inline void thisOperatorImage(const ImageGL &a, BOGL op)
    {
        BufferOpsGL *ops = BufferOpsGL::getInstance();

        if(channels == a.channels && width == a.width && height == a.height) {
            ops->list[op]->Process(getTexture(), a.getTexture(), getTexture(), width, height);
        } else {
            if((nPixels() == a.nPixels()) && (a.channels == 1)) {
                ops->list[op + 8]->Process(getTexture(), a.getTexture(), getTexture(), width, height);
            }
        }
    }

    /**
     * @brief newOperatorImage
     * @param a
     * @param op
     * @return
     */
    inline ImageGL newOperatorConst(const float &a, BOGL op)
    {
        ImageGL ret(frames, width, height, channels, IMG_GPU, target);
        BufferOpsGL *ops = BufferOpsGL::getInstance();

        ops->list[op]->Update(a);
        ops->list[op]->Process(getTexture(), 0, ret.getTexture(), width, height);

        return ret;
    }

    /**
     * @brief newOperatorImage
     * @param a
     * @param op
     */
    inline ImageGL newOperatorImage(const ImageGL &a, BOGL op)
    {
        ImageGL ret(frames, width, height, channels, IMG_GPU, target);

        BufferOpsGL *ops = BufferOpsGL::getInstance();

        if(channels == a.channels && width == a.width && height == a.height) {
            ops->list[op]->Process(getTexture(), a.getTexture(), ret.getTexture(), width, height);
        } else {
            if((nPixels() == a.nPixels()) && ((a.channels == 1) || (channels == 1))) {
                if(a.channels == 1) {
                    ops->list[op + 8]->Process(getTexture(), a.getTexture(), ret.getTexture(), width, height);
                } else {
                    ops->list[op + 8]->Process(a.getTexture(), getTexture(), ret.getTexture(), width, height);
                }
            }
        }

        return ret;
    }

public:

    /**
     * @brief ImageGL
     */
    ImageGL();

    ~ImageGL();

    /**
     * @brief ImageGL
     * @param texture
     * @param target
     */
    ImageGL(GLuint texture, GLenum target);

    /**
     * @brief ImageGL
     * @param img
     * @param transferOwnership
     */
    ImageGL(Image *img, bool transferOwnership);

    /**
     * @brief ImageGL
     * @param img
     * @param target
     * @param mipmap
     * @param transferOwnership
     */
    ImageGL(Image *img, GLenum target, bool mipmap, bool transferOwnership);

    /**
     * @brief ImageGL
     * @param nameFile
     */
    ImageGL(std::string nameFile): Image(nameFile)
    {
        notOwnedGL = false;
        mode = IMG_CPU;
        texture = 0;
        target = 0;
        tmpFbo = NULL;
    }

    /**
     * @brief Image
     * @param frames
     * @param width
     * @param height
     * @param channels
     * @param data
     */
    ImageGL(int frames, int width, int height, int channels, float *data) : Image (frames, width, height, channels, data)
    {
        notOwnedGL = false;
        mode = IMG_CPU;
        texture = 0;
        target = 0;
        tmpFbo = NULL;
    }

    /**
     * @brief ImageGL
     * @param frames
     * @param width
     * @param height
     * @param channels
     * @param mode
     */
    ImageGL(int frames, int width, int height, int channels, IMAGESTORE mode, GLenum target);

    /**
     * @brief AllocateSimilarOneGL
     * @return
     */
    ImageGL *AllocateSimilarOneGL();

    /**
     * @brief CloneGL
     * @return
     */
    ImageGL *CloneGL();

    /**
     * @brief generateTextureGL
     * @param target
     * @param format_type
     * @param mipmap
     * @return
     */
    GLuint generateTextureGL(GLenum target, GLenum format_type, bool mipmap);

    /**
     * @brief loadSliceIntoTexture
     * @param i
     */
    void loadSliceIntoTexture(int i);

    /**
     * @brief loadAllSlicesIntoTexture
     */
    void loadAllSlicesIntoTexture();

    /**
     * @brief loadFromMemory
     */
    void loadFromMemory();

    /**
     * @brief loadToMemory
     */
    void loadToMemory();

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
     * @brief getTexture
     * @return
     */
    GLuint getTexture() const
    {
        return texture;
    }

    /**
     * @brief setTexture
     * @param texture
     */
    void setTexture(GLuint texture)
    {
        //TODO: UNSAFE!
        this->texture = texture;
    }

    /**
     * @brief getTarget
     * @return
     */
    GLenum getTarget()
    {
        return target;
    }

    /**
     * @brief getVal
     * @param ret
     * @param flt
     * @return
     */
    float *getVal(float *ret, ReduxGL *flt)
    {
        if(ret == NULL) {
            ret = new float [channels];
        }

        if(stack.empty()) {
            ReduxGL::AllocateReduxData(width, height, channels, stack, 1);
        }

        GLuint output = flt->Redux(texture, width, height, channels, stack);

        //copying data from GPU to main memory
        int mode, modeInternalFormat;
        getModesGL(channels, mode, modeInternalFormat);

        glBindTexture(GL_TEXTURE_2D, output);
        glGetTexImage(GL_TEXTURE_2D, 0, mode, GL_FLOAT, ret);
        glBindTexture(GL_TEXTURE_2D, 0);

        return ret;
    }

    /**
     * @brief getMinVal
     * @param imgIn
     * @return
     */
    float *getMinVal(float *ret = NULL)
    {
        ReduxOpsGL *ops = ReduxOpsGL::getInstance();
        return getVal(ret, ops->list[REDGL_MIN]);
    }

    /**
     * @brief getMaxVal
     * @param imgIn
     * @param ret
     * @return
     */
    float *getMaxVal(float *ret = NULL)
    {
        ReduxOpsGL *ops = ReduxOpsGL::getInstance();
        return getVal(ret, ops->list[REDGL_MAX]);
    }

    /**
     * @brief getMeanVal
     * @param imgIn
     * @return
     */
    float *getMeanVal(float *ret = NULL)
    {
        ReduxOpsGL *ops = ReduxOpsGL::getInstance();
        return getVal(ret, ops->list[REDGL_MEAN]);
    }

    /**
     * @brief clamp
     * @param a
     * @param b
     */
    void clamp(float a, float b);

    /**
     * @brief operator =
     * @param a
     */
    void operator =(const ImageGL &a);

    /**
     * @brief operator =
     * @param a
     */
    void operator =(const float &a);

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const ImageGL &a);

    /**
     * @brief operator +=
     * @param a
     */
    void operator +=(const float &a);

    /**
     * @brief operator +
     * @param a
     * @return
     */
    ImageGL operator +(const ImageGL &a);

    /**
     * @brief operator +
     * @param a
     * @return
     */
    ImageGL operator +(const float &a);

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const ImageGL &a);

    /**
     * @brief operator -=
     * @param a
     */
    void operator -=(const float &a);

    /**
     * @brief operator -
     * @param a
     * @return
     */
    ImageGL operator -(const ImageGL &a);

    /**
     * @brief operator -
     * @param a
     * @return
     */
    ImageGL operator -(const float &a);

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const ImageGL &a);

    /**
     * @brief operator *=
     * @param a
     */
    void operator *=(const float &a);

    /**
     * @brief operator *
     * @param a
     * @return
     */
    ImageGL operator *(const ImageGL &a);

    /**
     * @brief operator *
     * @param a
     * @return
     */
    ImageGL operator *(const float &a);

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const ImageGL &a);

    /**
     * @brief operator /=
     * @param a
     */
    void operator /=(const float &a);

    /**
     * @brief operator /
     * @param a
     * @return
     */
    ImageGL operator /(const ImageGL &a);

    /**
     * @brief operator /
     * @param a
     * @return
     */
    ImageGL operator /(const float &a);
};

ImageGL::ImageGL() : Image()
{
    notOwnedGL = false;
    texture = 0;
    target = 0;
    mode = IMG_NULL;
    tmpFbo = NULL;
}

ImageGL::ImageGL(GLuint texture, GLuint target) : Image()
{
    notOwnedGL = true;

    tmpFbo = NULL;

    mode = IMG_GPU;

    this->texture = texture;

    this->target = target;

    getTextureInformationGL(texture, target, width, height, frames, channels);

    AllocateAux();
}

ImageGL::ImageGL(Image *img, GLenum target, bool mipmap, bool transferOwnership = false): Image()
{
    if(transferOwnership) {
        notOwned = false;
        img->ChangeOwnership(true);
    } else {
        notOwned = true;
    }

    notOwnedGL = false;

    tmpFbo = NULL;

    width    = img->width;
    height   = img->height;
    frames   = img->frames;
    channels = img->channels;
    data     = img->data;

    CalculateStrides();

    texture = 0;

    generateTextureGL(target, GL_FLOAT, mipmap);

    mode = IMG_CPU_GPU;
}

ImageGL::ImageGL(Image *img, bool transferOwnership = false) : Image()
{
    if(transferOwnership) {
        notOwned = false;
        img->ChangeOwnership(true);
    } else {
        notOwned = true;
    }

    notOwnedGL = false;

    tmpFbo = NULL;

    width    = img->width;
    height   = img->height;
    frames   = img->frames;
    channels = img->channels;
    data     = img->data;

    CalculateStrides();

    texture = 0;

    mode = IMG_CPU;
}

ImageGL::ImageGL(int frames, int width, int height, int channels,
                       IMAGESTORE mode, GLenum target) : Image()
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

        generateTextureGL(target, GL_FLOAT, false);
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

        generateTextureGL(target, GL_FLOAT, false);
    }
    break;
    }
}

ImageGL::~ImageGL()
{
    Destroy();
    DestroyGL();
}

/**
 * @brief ImageGL::generateTextureGL
 * @param target
 * @param format_type
 * @param mipmap
 * @return
 */
GLuint ImageGL::generateTextureGL(GLenum target = GL_TEXTURE_2D, GLenum format_type = GL_FLOAT, bool mipmap = false)
{
    this->texture = 0;
    this->target  = target;

    updateModeGPU();

    if(format_type == GL_INT) {
        int *buffer = new int[width * height * channels];

        for(int i = 0; i < (width * height * channels); i++) {
            buffer[i] = int(lround(data[i]));
        }

        texture = generateTexture2DU32GL(width, height, channels, buffer);

        delete[] buffer;

        return texture;
    }

    switch(target) {
        case GL_TEXTURE_2D:
        {
            texture = generateTexture2DGL(width, height, channels, data, mipmap);
        } break;

        case GL_TEXTURE_3D:
        {
            if(frames > 1) {
                texture = generateTexture3DGL(width, height, channels, frames, data);
            } else {
                texture = generateTexture2DGL(width, height, channels, data, mipmap);
                this->target = GL_TEXTURE_2D;
            }
        } break;

        case GL_TEXTURE_2D_ARRAY: {
            texture = generateTexture2DArrayGL(width, height, channels, frames, data);
        } break;

        case GL_TEXTURE_CUBE_MAP: {
            if(frames > 5) {
                texture = generateTextureCubeMapGL(width, height, channels, frames, data);
            } else {
                if(frames > 1) {
                    texture = generateTexture2DArrayGL(width, height, channels, frames, data);
                    this->target = GL_TEXTURE_2D_ARRAY;
                } else {
                    texture = generateTexture2DGL(width, height, channels, data, mipmap);
                    this->target = GL_TEXTURE_2D;
                }
            }
        } break;
    }

    return texture;
}

ImageGL *ImageGL::CloneGL()
{
    //call Image clone function
    Image *tmp = this->Clone();

    //wrapping tmp into an ImageGL
    return new ImageGL(tmp, target, false, true);
}

void ImageGL::DestroyGL()
{
    if(notOwnedGL) {
        return;
    }

    if(texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
        target = 0;
    }

    for(unsigned int i=0; i<stack.size(); i++) {
        if(stack[i] != 0) {
            glDeleteTextures(1, &stack[i]);
            stack[i] = 0;
        }
    }
}

ImageGL *ImageGL::AllocateSimilarOneGL()
{
#ifdef PIC_DEBUG
    printf("%d %d %d %d %d\n", frames, width, height, channels, mode);
#endif

    ImageGL *ret = new ImageGL(frames, width, height, channels, mode, target);
    return ret;
}

void ImageGL::loadFromMemory()
{
    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glBindTexture(target, texture);

    switch(target) {
        case GL_TEXTURE_2D: {
            glTexImage2D(target, 0, modeInternalFormat, width, height, 0,
                         mode, GL_FLOAT, data);

        } break;

        case GL_TEXTURE_3D: {
            glTexImage3D(GL_TEXTURE_3D, 0, modeInternalFormat, width, height, frames, 0,
                         mode, GL_FLOAT, data);
        } break;
    }

    glBindTexture(target, 0);
}

void ImageGL::loadToMemory()
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

void ImageGL::loadSliceIntoTexture(int i)
{
    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glBindTexture(target, texture);
    i = i % frames;
    glTexSubImage3D(target, 0, 0, 0, i, width, height, 1, mode, GL_FLOAT,
                    &data[i * tstride]);

    glBindTexture(target, 0);
}

void ImageGL::loadAllSlicesIntoTexture()
{
    if(target != GL_TEXTURE_3D && target != GL_TEXTURE_2D_ARRAY) {
        return;
    }

    for(int i = 0; i < frames; i++) {
        loadSliceIntoTexture(i);
    }
}

void ImageGL::readFromFBO(Fbo *fbo, GLenum format)
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

void ImageGL::readFromFBO(Fbo *fbo)
{
    if(mode == IMG_NULL) {
        mode = IMG_CPU;
    }

    readFromFBO(fbo, GL_RGBA);
}

void ImageGL::readFromBindedFBO()
{

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    if(mode == 0x0) {
        #ifdef PIC_DEBUG
            printf("void ImageGL::readFromBindedFBO(): error unknown format!");
        #endif
        return;
    }

    //TODO: check width height and data (mode and modeInternalFormat)

    glReadPixels(0, 0, width, height, mode, GL_FLOAT, data);
    FlipV();
}

void ImageGL::bindTexture()
{
    glBindTexture(target, texture);
}

void ImageGL::unBindTexture()
{
    glBindTexture(target, 0);
}

void ImageGL::clamp(float a = 0.0f, float b = 1.0f)
{
    BufferOpsGL *ops = BufferOpsGL::getInstance();
    ops->list[BOGL_CLAMP]->Update(a, b);
    ops->list[BOGL_CLAMP]->Process(getTexture(), 0, getTexture(), width, height);
}

void ImageGL::operator =(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_ID);
}

void ImageGL::operator =(const float &a)
{
    thisOperatorConst(a, BOGL_ID_CONST);
}

void ImageGL::operator +=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_ADD);
}

void ImageGL::operator +=(const float &a)
{
    thisOperatorConst(a, BOGL_ADD_CONST);
}

ImageGL ImageGL::operator +(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_ADD);
}

ImageGL ImageGL::operator +(const float &a)
{
    return newOperatorConst(a, BOGL_ADD_CONST);
}

void ImageGL::operator -=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_SUB);
}

void ImageGL::operator -=(const float &a)
{
    thisOperatorConst(a, BOGL_SUB_CONST);
}

ImageGL ImageGL::operator -(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_SUB);
}

ImageGL ImageGL::operator -(const float &a)
{
    return newOperatorConst(a, BOGL_SUB_CONST);
}

void ImageGL::operator *=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_MUL);
}

void ImageGL::operator *=(const float &a)
{
    thisOperatorConst(a, BOGL_MUL_CONST);
}

ImageGL ImageGL::operator *(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_MUL);
}

ImageGL ImageGL::operator *(const float &a)
{
    return newOperatorConst(a, BOGL_MUL_CONST);
}

void ImageGL::operator /=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_DIV);
}

void ImageGL::operator /=(const float &a)
{
    thisOperatorConst(a, BOGL_DIV_CONST);
}

ImageGL ImageGL::operator /(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_DIV);
}

ImageGL ImageGL::operator /(const float &a)
{
    return newOperatorConst(a, BOGL_DIV_CONST);
}

} // end namespace pic

#endif /* PIC_GL_IMAGE_RAW_HPP */

