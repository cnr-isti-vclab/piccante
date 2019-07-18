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

#ifndef PIC_GL_FILTERING_FILTER_HPP
#define PIC_GL_FILTERING_FILTER_HPP

#include "../../base.hpp"
#include "../../util/array.hpp"
#include "../../util/std_util.hpp"
#include "../../gl/image.hpp"
#include "../../gl/image_vec.hpp"
#include "../../util/array.hpp"
#include "../../util/gl/technique.hpp"
#include "../../util/gl/quad.hpp"

namespace pic {

/**
 * @brief The FilterGL class
 */
class FilterGL
{
protected:
    //FBO
    Fbo *fbo;

    //Quad
    QuadGL *quad;

    //Shaders
    TechniqueGL technique;

    GLenum target;

    ImageGLVec param;

    bool bFboOwn;
public:

    bool bDelete;
    std::vector< FilterGL* > filters;

    std::string vertex_source, geometry_source, fragment_source;

    /**
     * @brief FilterGL
     */
    FilterGL()
    {
        bDelete = false;

        fbo = NULL;

        this->bFboOwn = true;
        quad = new QuadGL(false, 1.0f, 1.0f);

        target = GL_TEXTURE_2D;

        //getting a vertex program for screen aligned quad
        vertex_source = QuadGL::getVertexProgramV3();
    }

    ~FilterGL()
    {
        release();
    }

    /**
     * @brief release
     */
    void release()
    {
        quad = delete_s(quad);

        if(bFboOwn) {
            fbo = delete_s(fbo);
        }

        releaseAux();
    }

    /**
     * @brief releaseAux
     */
    virtual void releaseAux()
    {

    }

    /**
     * @brief setFbo
     * @param fbo
     */
    void setFbo(Fbo *fbo)
    {
        this->fbo = fbo;
        this->bFboOwn = false;
    }

    /**
     * @brief getFbo
     * @return
     */
    virtual Fbo *getFbo()
    {
        return fbo;
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    virtual void OutputSize(ImageGLVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width    = imgIn[0]->width;
        height   = imgIn[0]->height;
        channels = imgIn[0]->channels;
        frames   = imgIn[0]->frames;
    }

    /**
     * @brief insertFilter
     * @param flt
     */
    void insertFilter(FilterGL *flt)
    {
        if(flt == NULL) {
            return;
        }

        if(!flt->filters.empty()) {
            for(unsigned int i = 0; i < flt->filters.size(); i++) {
                insertFilter(flt->filters[i]);
            }
        } else {
            filters.push_back(flt);
        }
    }

    /**
     * @brief setTarget
     * @param target
     */
    void setTarget(GLenum target)
    {
        this->target = target;
    }

    /**
     * @brief changePass
     * @param pass
     * @param tPass
     */
    virtual void changePass(int pass, int tPass)
    {
    }

    /**
     * @brief gammaCorrection
     * @param fragment_source
     * @param bGammaCorrection
     * @return
     */
    static std::string gammaCorrection(std::string fragment_source,
                                       bool bGammaCorrection)
    {
        size_t processing_found = fragment_source.find("__GAMMA__CORRECTION__");

        if(processing_found != std::string::npos) {
            if(bGammaCorrection) {
                fragment_source.replace(processing_found, 21,
                                        " color = pow(color, vec3(1.0 / 2.2)); ");
            } else {
                fragment_source.replace(processing_found, 21, " ");
            }
        }

        return fragment_source;
    }

    /**
     * @brief setupAux
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual ImageGL *setupAux(ImageGLVec imgIn, ImageGL *imgOut)
    {
        return allocateOutputMemory(imgIn, imgOut, bDelete);
    }

    /**
     * @brief allocateOutputMemory
     * @param imgIn
     * @param imgOut
     * @param bDelete
     * @return
     */
    ImageGL *allocateOutputMemory(ImageGLVec imgIn, ImageGL *imgOut, bool bDelete)
    {
        int w, h, c, f;
        OutputSize(imgIn, w, h, c, f);

        if(imgOut == NULL) {
            imgOut = new ImageGL(f, w, h, c, IMG_GPU, imgIn[0]->getTarget());
        } else {
            bool bSame = imgOut->width == w &&
                         imgOut->height == h &&
                         imgOut->channels == c &&
                         imgOut->frames == f;

            if(!bSame) {
                if(bDelete) {
                    delete imgOut;
                }

                imgOut = new ImageGL(f, w, h, c, IMG_GPU, imgIn[0]->getTarget());
            }
        }

        return imgOut;
    }

    virtual void bindTechnique()
    {
        technique.bind();
    }

    virtual void unbindTechnique()
    {
        technique.unbind();
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    virtual ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut)
    {
        if(imgIn.empty()) {
            return imgOut;
        }

        if(imgIn[0] == NULL) {
            return imgOut;
        }

        imgOut = setupAux(imgIn, imgOut);

        if(imgOut == NULL) {
            return NULL;
        }

        //create an FBO
        if(fbo == NULL) {
            fbo = new Fbo();
            bFboOwn = true;
        }

        fbo->create(imgOut->width, imgOut->height, imgOut->frames, false, imgOut->getTexture());

        //bind the FBO
        fbo->bind();
        glViewport(0, 0, (GLsizei)imgIn[0]->width, (GLsizei)imgIn[0]->height);

        //bind shaders
        bindTechnique();

        //bind textures
        int n = int(imgIn.size());
        for(auto i = 0; i < n; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            imgIn[i]->bindTexture();
        }

        //bind texture internal filter parameters
        int m = int(param.size());
        for(auto i = 0; i < m; i++) {
            glActiveTexture(GL_TEXTURE0 + n + i);
            param[i]->bindTexture();
        }

        //render an aligned quad
        quad->Render();

        //unbind the FBO
        fbo->unbind();

        //unbind shaders
        unbindTechnique();

        //unbind textures
        for(auto i = 0; i< n; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            imgIn[i]->unBindTexture();
        }

        for(auto i = 0; i < m; i++) {
            glActiveTexture(GL_TEXTURE0 + n + i);
            param[i]->unBindTexture();
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_HPP */

