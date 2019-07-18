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

#ifndef PIC_GL_POINT_SAMPLERS_SAMPLER_RANDOM_M_HPP
#define PIC_GL_POINT_SAMPLERS_SAMPLER_RANDOM_M_HPP

#include "../../base.hpp"

#include "../../gl/image.hpp"

#include "../../point_samplers/sampler_random_m.hpp"

namespace pic {

/**
 * @brief glGetPrintError
 */
PIC_INLINE void glGetPrintError()
{
    GLenum err = glGetError();

    if(err != GL_NO_ERROR) {
        printf("----------- %d\n", err);
    }
}

/**
 * @brief The MRSamplersGL class
 */
template <unsigned int N>
class MRSamplersGL: public MRSamplers<N>
{
protected:
    GLuint texture;
    GLuint levelsRtexture;
    int width, height;

public:
    int		nSamples;

    /**
     * @brief MRSamplersGL
     * @param type
     * @param window
     * @param nSamples
     * @param nLevels
     * @param nSamplers
     */
    MRSamplersGL(SAMPLER_TYPE type, Vec<N, int> window, int nSamples, int nLevels,
                 int nSamplers): MRSamplers<N>(type, window, nSamples, nLevels, nSamplers)
    {
        texture = 0;
    }

    /**
     * @brief updateGL
     * @param window
     * @param nSamples
     */
    void   updateGL(Vec<N, int> window, int nSamples);

    /**
     * @brief getTexture
     * @return
     */
    GLuint getTexture()
    {
        return texture;
    }

    /**
     * @brief getImage
     * @return
     */
    ImageGL *getImage()
    {
        ImageGL *ret = new ImageGL(texture, GL_TEXTURE_2D);

        return ret;
    }

    /**
     * @brief getImageLevelsR
     * @return
     */
    ImageGL *getImageLevelsR()
    {
        ImageGL *ret = new ImageGL(levelsRtexture, GL_TEXTURE_2D);

        return ret;
    }

    /**
     * @brief generateTexture
     * @return
     */
    GLuint generateTexture();

    /**
     * @brief getLevelsRTexture
     * @return
     */
    GLuint getLevelsRTexture()
    {
        return levelsRtexture;
    }

    /**
     * @brief generateLevelsRTexture
     * @return
     */
    GLuint generateLevelsRTexture();
};

template <unsigned int N> void MRSamplersGL<N>::updateGL(Vec<N, int> window,
        int nSamples)
{
    if(texture == -1) {
        return;
    }

#ifdef PIC_DEBUG
    printf("window: %d %d\n", window[0], window[1]);
#endif

    if(!this->update(window, nSamples)) {
        return;
    }

    glDeleteTextures(1, &texture);
    generateTexture();

    this->oldSamples = nSamples;
    this->oldWindow = window;
}

template <unsigned int N> GLuint MRSamplersGL<N>::generateTexture()
{
    if(this->nSamplers <= 0) {
        texture = 0;

#ifdef PIC_DEBUG
        printf("No samplers in MRSamplersGL.\n");
#endif
        nSamples = -1;
        return 0;
    }

    nSamples = int(this->samplers[0]->samplesR.size());

    for(int i = 1; i < this->nSamplers; i++) {
        nSamples = MIN(nSamples, int(this->samplers[i]->samplesR.size()));
    }

#ifdef PIC_DEBUG
    printf("Samples in samplers: %d %d\n", nSamples / N, N);
#endif

    //Create the buffer in the main memory
    int *buffer = new int [this->nSamplers * (nSamples / N) * 4];

    int ind = 0;

    for(int i = 0; i < this->nSamplers; i++) {
        for(int j = 0; j < nSamples; j += N) {
            int ind2 = ind * 4;

            for(int k = 0; k < 4; k++) {
                buffer[ind2 + k] = 0;
            }

            for(int k = 0; k < N; k++) {
                buffer[ind2 + k]   = this->samplers[i]->samplesR[j + k];
            }

            buffer[ind2 + N] =	this->samplers[i]->samplesR[j]	*
                                this->samplers[i]->samplesR[j] +
                                this->samplers[i]->samplesR[j + 1] * this->samplers[i]->samplesR[j + 1];
            ind++;
        }
    }

    //Transfer the buffer into graphics card's memory
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    width = nSamples / N;
    height = this->nSamplers;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32I, width, height, 0,
                 GL_RGBA_INTEGER, GL_INT, buffer);

    //nSamples = nSamples>>1;
    //glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    //release memory from the buffer
    delete[] buffer;
    return texture;
}

template <unsigned int N> GLuint MRSamplersGL<N>::generateLevelsRTexture()
{
    //Create the buffer in the main memory
    int *buffer = new int [this->nSamplers * this->nLevels];
    int ind = 0;

    for(int i = 0; i < this->nSamplers; i++) {
        for(int j = 0; j < this->nLevels; j++) {
            buffer[ind] = this->samplers[i]->levelsR[j];
            printf("%d ", buffer[ind]);
            ind++;
        }

        printf("\n");
    }

    //Transfer the buffer into graphics card's memory
    glGenTextures(1, &levelsRtexture);
    glBindTexture(GL_TEXTURE_2D, levelsRtexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, this->nLevels, this->nSamplers, 0,
                 GL_RED_INTEGER, GL_INT, buffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    //release memory from the buffer
    delete[] buffer;
    return levelsRtexture;
}

} // end namespace pic

#endif /* PIC_GL_POINT_SAMPLERS_SAMPLER_RANDOM_M_HPP */

