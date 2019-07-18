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

#ifndef PIC_GL_FILTERING_FILTER_CHANNEL_HPP
#define PIC_GL_FILTERING_FILTER_CHANNEL_HPP

#include "../../base.hpp"
#include "../../util/math.hpp"
#include "../../gl/filtering/filter.hpp"

namespace pic {

/**
 * @brief The FilterGLChannel class
 */
class FilterGLChannel: public FilterGL
{
protected:
    /**
     * @brief initShaders
     */
    void initShaders()
    {
        fragment_source = MAKE_STRING
                          (
        uniform sampler2D u_tex; \n
        uniform int channel; \n
        out     vec4 f_color; \n
        \n
        void main(void) {
            \n
            ivec2 coords = ivec2(gl_FragCoord.xy); \n
            vec4 color = texelFetch(u_tex, coords, 0); \n
            float v = color[channel]; \n
            f_color = vec4(v, v, v, 1.0); \n

        }
                          );

        technique.initStandard("330", vertex_source, fragment_source, "FilterGLChannel");
    }

    int channel;

public:

    /**
     * @brief FilterGLChannel
     * @param channel
     */
    FilterGLChannel(int channel) : FilterGL()
    {
        initShaders();
        update(channel);
    }

    ~FilterGLChannel()
    {
        release();
    }

    /**
     * @brief update
     * @param channel
     */
    void update(int channel)
    {
        this->channel = CLAMP(channel, 4);

        if(technique.isValid()) {
            technique.bind();
            technique.setUniform1i("u_tex", 0);
            technique.setUniform1i("channel", channel);
            technique.unbind();
        }
    }

    /**
     * @brief OutputSize
     * @param imgIn
     * @param width
     * @param height
     * @param channels
     * @param frames
     */
    void OutputSize(ImageGLVec imgIn, int &width, int &height, int &channels, int &frames)
    {
        width       = imgIn[0]->width;
        height      = imgIn[0]->height;
        channels    = 1;
        frames      = imgIn[0]->frames;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param channel
     * @return
     */
    static ImageGL *execute(ImageGL *imgIn, ImageGL *imgOut, int channel = 0)
    {
        FilterGLChannel flt(channel);
        return flt.Process(SingleGL(imgIn), imgOut);
    }

    /**
     * @brief Test
     */
    static void Test()
    {
        ImageGL imgIn(1, 512, 512, 3, IMG_GPU_CPU, GL_TEXTURE_2D);

        for(auto i = 0; i < imgIn.size(); i += 3) {
            imgIn.data[i    ] = 1.0f;
            imgIn.data[i + 1] = 0.5f;
            imgIn.data[i + 2] = 0.25f;
        }

        imgIn.generateTextureGL();

        FilterGLChannel filter(0);
        ImageGL *outR = filter.Process(SingleGL(&imgIn), NULL);

        filter.update(1);
        ImageGL *outG = filter.Process(SingleGL(&imgIn), NULL);

        filter.update(2);
        ImageGL *outB = filter.Process(SingleGL(&imgIn), NULL);

        outR->loadToMemory();
        outR->Write("channel_R.bmp");
        outG->loadToMemory();
        outG->Write("channel_G.bmp");
        outB->loadToMemory();
        outB->Write("channel_B.bmp");
    }
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_CHANNEL_HPP */

