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

#ifndef PIC_UTIL_GL_FBO_HPP
#define PIC_UTIL_GL_FBO_HPP

#include <iostream>

#include "../../base.hpp"

namespace pic {

    //NOTE: this class needs to be used with an active OpenGL context!

    /**
     * @brief The Fbo class
     */
    class Fbo
    {
    public:
        GLuint fbo;                 // framebuffer object
        GLuint tex;                 // we render into this texture
        GLuint depth;               // and to this depth buffer
        int width, height, frames;  // width and height of the framebuffer
        bool bDepth;                // do we have a depth buffer?

        //MRT
        GLuint       *texMRT;
        unsigned int nMRT;
        GLuint      *attachmentsMRT;

        Fbo();

        /**
         * @brief create
         * @param width
         * @param height
         * @param depth
         * @param bDepth
         * @param tex
         * @return
         */
        bool create(int width, int height, bool bDepth);

        /**
         * @brief create
         * @param width
         * @param height
         * @param bDepth
         * @return
         */
        bool create(int width, int height, int depth, bool bDepth, GLuint tex);

        /**
         * @brief create
         * @param width
         * @param height
         * @param bDepth
         * @return
         */
        bool createMRT(int width, int height, bool bDepth, unsigned int nMRT);

        /**
         * @brief attachColorBuffer
         * @param tex
         * @param target
         * @param slice
         */
        void attachColorBuffer(GLuint tex, GLenum target, int slice = 0);

        /**
         * @brief attachColorBuffer2
         * @param tex
         * @param target
         * @param slice
         */
        void attachColorBuffer2(GLuint tex, GLenum target, int slice);

        /**
         * @brief release
         * @return
         */
        bool release();

        /**
         * @brief bind
         */
        void bind();

        /**
         * @brief unbind
         */
        void unbind();

        /**
         * @brief bindSimple
         */
        void bindSimple()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        }

        /**
         * @brief unbindSimple
         */
        void unbindSimple()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        /**
         * @brief bindMRT
         */
        void bindMRT();

        /**
         * @brief unbind
         */
        void unbindMRT();

        /**
         * @brief clone
         * @return
         */
        Fbo *clone();

        /**
         * @brief checkStatus
         * @param fboStatus
         */
        static void checkStatus(GLenum fboStatus)
        {
            switch (fboStatus) {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                std::cerr << "FBO Incomplete: Attachment" << std::endl;
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                std::cerr << "FBO Incomplete: Missing Attachment" << std::endl;
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                std::cerr << "FBO Incomplete: Draw Buffer" << std::endl;
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                std::cerr << "FBO Incomplete: Read Buffer" << std::endl;
                break;

            default:
                std::cerr << "Undefined FBO error" << std::endl;
                break;
            }
        }
    };

    PIC_INLINE Fbo::Fbo()
    {
        depth = 0;
        fbo = 0;
        tex = 0;

        width = height = frames = 0;
    }

    PIC_INLINE bool Fbo::release()
    {
        if (tex != 0) {
            glDeleteTextures(1, &tex);
            tex = 0;
        }

        if (depth != 0) {
            glDeleteRenderbuffers(1, &depth);
            depth = 0;
        }

        if (fbo != 0) {
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }

        return true;
    }

    PIC_INLINE Fbo *Fbo::clone()
    {
        Fbo *ret = new Fbo();
        ret->create(width, height, frames, bDepth, 0);
        return ret;
    }

    PIC_INLINE bool Fbo::create(int width, int height, bool bDepth)
    {
        return create(width, height, 1, bDepth, 0);
    }

    PIC_INLINE bool Fbo::create(int width, int height, int frames, bool bDepth, GLuint tex)
    {
        this->width = width;
        this->height = height;
        this->frames = frames;
        this->bDepth = bDepth;

        //FBO with one COLOR ATTACHMENT
        //setup texture (render target)
        if (tex == 0) {
            glGenTextures(1, &this->tex);

            if (frames == 1) {
                glBindTexture(GL_TEXTURE_2D, this->tex);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT,
                    0);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            else {
                glBindTexture(GL_TEXTURE_3D, this->tex);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, width, height, frames, 0, GL_RGBA,
                    GL_FLOAT, 0);
                glBindTexture(GL_TEXTURE_3D, 0);
            }
        }
        else {
            this->tex = tex;
        }

        //setup renderbuffer (depth buffer)
        //assert(glGenRenderbuffers);
        if (bDepth) {
            glGenRenderbuffers(1, &depth);
            glBindRenderbuffer(GL_RENDERBUFFER, depth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        // setup FBO
        if (fbo == 0) {
            glGenFramebuffers(1, &fbo);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // attach color buffer (texture)
        if (frames == 1) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                this->tex, 0);
        }
        else {
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D,
                this->tex, 0, 0);
        }

        // attach depth buffer (renderbuffer)
        if (bDepth) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                depth);
        }

#ifdef PIC_DEBUG_GL
        // check framebuffer status
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            checkStatus(fboStatus);
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }

#endif

        // unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return fbo != 0;
    }

    PIC_INLINE bool Fbo::createMRT(int width, int height, bool bDepth, unsigned int nMRT)
    {
        this->width = width;
        this->height = height;
        this->bDepth = bDepth;
        this->nMRT = nMRT;

        texMRT = new GLuint[nMRT];
        attachmentsMRT = new GLuint[nMRT];

        for (unsigned int i = 0; i < nMRT; i++) {
            texMRT[i] = 0;
            glGenTextures(1, &texMRT[i]);
            glBindTexture(GL_TEXTURE_2D, texMRT[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if (bDepth) {
            glGenRenderbuffers(1, &depth);
            glBindRenderbuffer(GL_RENDERBUFFER, depth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        // setup FBO
        if (fbo == 0) {
            glGenFramebuffers(1, &fbo);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // attach color buffer (texture)
        for (unsigned int i = 0; i < nMRT; i++) {
            attachmentsMRT[i] = GL_COLOR_ATTACHMENT0 + i;

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                texMRT[i], 0);
        }


        // attach depth buffer (renderbuffer)
        if (bDepth) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                depth);
        }

#ifdef PIC_DEBUG_GL
        // check framebuffer status
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            checkStatus(fboStatus);
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }

#endif

        // unbind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return fbo != 0;
    }

    PIC_INLINE void Fbo::attachColorBuffer(GLuint tex, GLenum target, int slice)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        GLuint texWork = (tex == 0) ? this->tex : tex;

        switch (target) {
        case GL_TEXTURE_2D: {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                texWork, 0);
        }
                            break;

        case GL_TEXTURE_3D: {
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D,
                texWork, 0, slice);
        }
                            break;
        }

#ifdef PIC_DEBUG_GL
        // check framebuffer status
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            checkStatus(fboStatus);
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }

#endif

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    PIC_INLINE void Fbo::attachColorBuffer2(GLuint tex, GLenum target, int slice)
    {
        GLuint texWork = (tex == 0) ? this->tex : tex;

        switch (target) {
        case GL_TEXTURE_2D: {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                texWork, 0);
        }
                            break;

        case GL_TEXTURE_2D_ARRAY: {
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D_ARRAY, texWork, 0, slice);
        }
                                  break;

        case GL_TEXTURE_3D: {
            glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D,
                texWork, 0, slice);
        }
                            break;
        }
    }

    PIC_INLINE void Fbo::bind()
    {
        if (!fbo) {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        if (bDepth) {
            glBindRenderbuffer(GL_RENDERBUFFER, depth);
        }
    }

    PIC_INLINE void Fbo::unbind()
    {
        if (!fbo) {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glReadBuffer(GL_BACK);

        if (bDepth) {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    }

    PIC_INLINE void Fbo::bindMRT()
    {
        if (!fbo) {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glDrawBuffers(nMRT, attachmentsMRT);
    }

    PIC_INLINE void Fbo::unbindMRT()
    {
        if (!fbo) {
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


} // end namespace pic

#endif /* PIC_UTIL_GL_FBO_HPP */

