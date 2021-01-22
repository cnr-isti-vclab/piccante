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

#include "../base.hpp"

#include "../image.hpp"

#include "../gl.hpp"
#include "../util/gl/fbo.hpp"
#include "../util/gl/formats.hpp"
#include "../util/gl/timings.hpp"
#include "../util/gl/buffer_ops.hpp"
#include "../util/gl/buffer_allocation.hpp"
#include "../util/gl/mask.hpp"
#include "../util/gl/redux.hpp"
#include "../util/gl/redux_ops.hpp"

namespace pic {

enum IMAGESTORE {IMG_GPU_CPU, IMG_CPU_GPU, IMG_CPU, IMG_GPU, IMG_NULL};

/**
 * @brief The ImageGL class
 */
class ImageGL: public Image
{
protected:
    GLuint      texture;
    GLenum      target;
    IMAGESTORE  mode;	        //TODO: check if the mode is always correctly updated
    bool        notOwnedGL;     //do we own the OpenGL texture??
    Fbo         *tmpFbo;

    //stack for statistics
    std::vector<GLuint> stack;

    /**
     * @brief releaseGL
     */
    void releaseGL();

    /**
     * @brief assignGL assigns an (r, g, b, a) value to an image using glClearColor.
     * @param r is the value for the red channel.
     * @param g is the value for the green channel.
     * @param b is the value for the blue channel.
     * @param a is the value for the alpha channel.
     */
    void assignGL(float r = 0.0f, float g = 0.0f, float b = 0.0f,
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

        ops->list[op]->update(a);
        ops->list[op]->Process(getTexture(), 0, getTexture(), width, height);
    }

    /**
     * @brief thisOperatorConstColor
     * @param a
     * @param op
     */
    inline void thisOperatorConstColor(const Arrayf &a, BOGL op)
    {
        BufferOpsGL *ops = BufferOpsGL::getInstance();

        float c0[4];
        Arrayf::assign(a.data, MIN(a.nData, 4), c0);

        ops->list[op]->update(c0);
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
     * @brief newOperatorConstColor
     * @param a
     * @param op
     * @return
     */
    inline ImageGL newOperatorConstColor(const Arrayf &a, BOGL op)
    {
        ImageGL ret(frames, width, height, channels, IMG_GPU, target);
        BufferOpsGL *ops = BufferOpsGL::getInstance();

        float c0[4];
        Arrayf::assign(a.data, MIN(a.nData, 4), c0);

        ops->list[op]->update(c0);
        ops->list[op]->Process(getTexture(), 0, ret.getTexture(), width, height);

        return ret;
    }

    /**
     * @brief newOperatorConst
     * @param a
     * @param op
     * @return
     */
    inline ImageGL newOperatorConst(const float &a, BOGL op)
    {
        ImageGL ret(frames, width, height, channels, IMG_GPU, target);
        BufferOpsGL *ops = BufferOpsGL::getInstance();

        ops->list[op]->update(a);
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
     * @brief allocateSimilarOneGL
     * @return
     */
    ImageGL *allocateSimilarOneGL();

    /**
     * @brief cloneGL
     * @return
     */
    ImageGL *cloneGL();

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
        if(texture == 0) {
            return ret;
        }

        if(ret == NULL) {
            ret = new float [channels];
        }

        if(stack.empty()) {
            ReduxGL::allocateReduxData(width, height, channels, stack, 1);
        }

        GLuint output = flt->Redux(texture, width, height, channels, stack);

        //copy data from GPU to main memory
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
     * @brief getSumVal
     * @param imgIn
     * @param ret
     * @return
     */
    float *getSumVal(float *ret = NULL)
    {
        ReduxOpsGL *ops = ReduxOpsGL::getInstance();
        return getVal(ret, ops->list[REDGL_SUM]);
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
     * @brief getLogMeanVal
     * @param imgIn
     * @return
     */
    float *getLogMeanVal(float *ret = NULL)
    {
        ReduxOpsGL *ops = ReduxOpsGL::getInstance();

        ret = getVal(ret, ops->list[REDGL_LOG_MEAN]);

        for(int i = 0; i < channels; i++) {
            ret[i] = expf(ret[i]);
        }

        return ret;
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
     * @brief operator /=
     * @param a
     */
    void operator /=(const Arrayf &a);

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

PIC_INLINE ImageGL::ImageGL() : Image()
{
    notOwnedGL = false;
    texture = 0;
    target = 0;
    mode = IMG_NULL;
    tmpFbo = NULL;
}

PIC_INLINE ImageGL::ImageGL(GLuint texture, GLuint target) : Image()
{
    notOwnedGL = true;

    tmpFbo = NULL;

    mode = IMG_GPU;

    this->texture = texture;

    this->target = target;

    getTextureInformationGL(texture, target, width, height, frames, channels);

    allocateAux();
}

PIC_INLINE ImageGL::ImageGL(Image *img, GLenum target, bool mipmap, bool transferOwnership = false): Image()
{
    if(transferOwnership) {
        notOwned = false;
        img->changeOwnership(true);
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

    allocateAux();

    texture = 0;

    generateTextureGL(target, GL_FLOAT, mipmap);

    mode = IMG_CPU_GPU;
}

PIC_INLINE ImageGL::ImageGL(Image *img, bool transferOwnership = false) : Image()
{
    if(transferOwnership) {
        notOwned = false;
        img->changeOwnership(true);
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

    allocateAux();

    texture = 0;

    mode = IMG_CPU;
}

PIC_INLINE ImageGL::ImageGL(int frames, int width, int height, int channels,
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
        allocate(width, height, channels, frames);

        generateTextureGL(target, GL_FLOAT, false);
    }
    break;

    case IMG_CPU: {
        allocate(width, height, channels, frames);
    }
    break;

    case IMG_GPU: {
        this->width = width;
        this->height = height;
        this->frames = frames;
        this->depth = frames;
        this->channels = channels;

        allocateAux();

        generateTextureGL(target, GL_FLOAT, false);
    }
    break;

    default: {

    }break;

    }
}

PIC_INLINE ImageGL::~ImageGL()
{   
    releaseGL();
    release();
}

/**
 * @brief ImageGL::generateTextureGL
 * @param target
 * @param format_type
 * @param mipmap
 * @return
 */
PIC_INLINE GLuint ImageGL::generateTextureGL(GLenum target = GL_TEXTURE_2D, GLenum format_type = GL_FLOAT, bool mipmap = false)
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

PIC_INLINE ImageGL *ImageGL::cloneGL()
{
    //call Image clone function
    Image *tmp = this->clone();

    //wrap tmp into an ImageGL
    return new ImageGL(tmp, target, false, true);
}

PIC_INLINE void ImageGL::releaseGL()
{
    if(notOwnedGL) {
        return;
    }

    if(texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
        target = 0;
    }

    uint n = stack.size();
    for(uint i = 0; i < n; i++) {
        if(stack[i] != 0) {
            glDeleteTextures(1, &stack[i]);
            stack[i] = 0;
        }
    }
}

PIC_INLINE ImageGL *ImageGL::allocateSimilarOneGL()
{
#ifdef PIC_DEBUG
    printf("ImageGL::allocateSimilarOneGL -- %d %d %d %d %d\n", frames, width, height, channels, mode);
#endif

    ImageGL *ret = new ImageGL(frames, width, height, channels, mode, target);
    return ret;
}

PIC_INLINE void ImageGL::loadFromMemory()
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

PIC_INLINE void ImageGL::loadToMemory()
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

        allocate(width, height, channels, frames);
        this->mode = IMG_CPU_GPU;
    }

    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    bindTexture();

    glGetTexImage(target, 0, mode, GL_FLOAT, data);

    unBindTexture();
}

PIC_INLINE void ImageGL::loadSliceIntoTexture(int i)
{
    int mode, modeInternalFormat;
    getModesGL(channels, mode, modeInternalFormat);

    glBindTexture(target, texture);
    i = i % frames;
    glTexSubImage3D(target, 0, 0, 0, i, width, height, 1, mode, GL_FLOAT,
                    &data[i * tstride]);

    glBindTexture(target, 0);
}

PIC_INLINE void ImageGL::loadAllSlicesIntoTexture()
{
    if(target != GL_TEXTURE_3D && target != GL_TEXTURE_2D_ARRAY) {
        return;
    }

    for(int i = 0; i < frames; i++) {
        loadSliceIntoTexture(i);
    }
}

PIC_INLINE void ImageGL::readFromFBO(Fbo *fbo, GLenum format)
{
    //TO DO: check data
    bool bCheck =   (fbo->width  != width) ||
                    (fbo->height != height);

    if(data == NULL || bCheck) {
        allocate(fbo->width, fbo->height, 4, 1);
    }

    //ReadPixels from the FBO
    fbo->bind();
    glReadPixels(0, 0, width, height, format, GL_FLOAT, data);
    fbo->unbind();

    /*	glBindTexture(GL_TEXTURE_2D, fbo->tex);
    	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);
    	glBindTexture(GL_TEXTURE_2D, 0);*/
}

PIC_INLINE void ImageGL::readFromFBO(Fbo *fbo)
{
    if(mode == IMG_NULL) {
        mode = IMG_CPU;
    }

    readFromFBO(fbo, GL_RGBA);
}

PIC_INLINE void ImageGL::readFromBindedFBO()
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
    flipV();
}

PIC_INLINE void ImageGL::bindTexture()
{
    glBindTexture(target, texture);
}

PIC_INLINE void ImageGL::unBindTexture()
{
    glBindTexture(target, 0);
}

PIC_INLINE void ImageGL::clamp(float a = 0.0f, float b = 1.0f)
{
    BufferOpsGL *ops = BufferOpsGL::getInstance();
    ops->list[BOGL_CLAMP]->update(a, b);
    ops->list[BOGL_CLAMP]->Process(getTexture(), 0, getTexture(), width, height);
}

PIC_INLINE void ImageGL::operator =(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_ID);
}

PIC_INLINE void ImageGL::operator =(const float &a)
{
    thisOperatorConst(a, BOGL_ID_CONST);
}

PIC_INLINE void ImageGL::operator +=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_ADD);
}

PIC_INLINE void ImageGL::operator +=(const float &a)
{
    thisOperatorConst(a, BOGL_ADD_CONST);
}

PIC_INLINE ImageGL ImageGL::operator +(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_ADD);
}

PIC_INLINE ImageGL ImageGL::operator +(const float &a)
{
    return newOperatorConst(a, BOGL_ADD_CONST);
}

PIC_INLINE void ImageGL::operator -=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_SUB);
}

PIC_INLINE void ImageGL::operator -=(const float &a)
{
    thisOperatorConst(a, BOGL_SUB_CONST);
}

PIC_INLINE ImageGL ImageGL::operator -(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_SUB);
}

PIC_INLINE ImageGL ImageGL::operator -(const float &a)
{
    return newOperatorConst(a, BOGL_SUB_CONST);
}

PIC_INLINE void ImageGL::operator *=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_MUL);
}

PIC_INLINE void ImageGL::operator *=(const float &a)
{
    thisOperatorConst(a, BOGL_MUL_CONST);
}

PIC_INLINE ImageGL ImageGL::operator *(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_MUL);
}

PIC_INLINE ImageGL ImageGL::operator *(const float &a)
{
    return newOperatorConst(a, BOGL_MUL_CONST);
}

PIC_INLINE void ImageGL::operator /=(const ImageGL &a)
{
    thisOperatorImage(a, BOGL_DIV);
}

PIC_INLINE void ImageGL::operator /=(const float &a)
{
    thisOperatorConst(a, BOGL_DIV_CONST);
}

PIC_INLINE void ImageGL::operator /=(const Arrayf &a)
{
    thisOperatorConstColor(a, BOGL_DIV_CONST);
}

PIC_INLINE ImageGL ImageGL::operator /(const ImageGL &a)
{
    return newOperatorImage(a, BOGL_DIV);
}

PIC_INLINE ImageGL ImageGL::operator /(const float &a)
{
    return newOperatorConst(a, BOGL_DIV_CONST);
}

} // end namespace pic

#endif /* PIC_GL_IMAGE_RAW_HPP */

