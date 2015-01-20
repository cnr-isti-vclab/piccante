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

#ifndef PIC_GL_FILTERING_FILTER_BILATERAL_3DAS_HPP
#define PIC_GL_FILTERING_FILTER_BILATERAL_3DAS_HPP

namespace pic {

/**
 * @brief The FilterGLBilateral3DAS class
 */
class FilterGLBilateral3DAS: public FilterGL
{
protected:
    float sigma_s, sigma_r, sigma_t;
    int	frame;
    MRSamplersGL<3> *ms;

    //Random numbers tile
    ImageGL *imageRand;

    //Sampling map
    FilterGLSamplingMap *fGLsm;

    ImageGL			*imgFrame;
    ImageGL			*imgTmp;

    void InitShaders();
    void FragmentShader();

public:

    /**
     * @brief FilterGLBilateral3DAS
     */
    FilterGLBilateral3DAS();

    /**
     * @brief FilterGLBilateral3DAS
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    FilterGLBilateral3DAS(float sigma_s, float sigma_r, float sigma_t);

    ~FilterGLBilateral3DAS();

    /**
     * @brief Update
     * @param sigma_s
     * @param sigma_r
     * @param sigma_t
     */
    void Update(float sigma_s, float sigma_r, float sigma_t);

    /**
     * @brief setFrame
     * @param frame
     */
    void setFrame(int frame)
    {
        this->frame = frame;
        Update(-1.0f, -1.0f, -1.0f);
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    ImageGL *Process(ImageGLVec imgIn, ImageGL *imgOut);
};

//Basic constructor
FilterGLBilateral3DAS::FilterGLBilateral3DAS(): FilterGL()
{
    ms = NULL;
    imageRand = NULL;
    fGLsm = NULL;
    imgTmp = NULL;
}

FilterGLBilateral3DAS::~FilterGLBilateral3DAS()
{
    delete ms;
    delete imageRand;
    delete fGLsm;
    delete imgTmp;
}

//Init constructors
FilterGLBilateral3DAS::FilterGLBilateral3DAS(float sigma_s, float sigma_r,
        float sigma_t): FilterGL()
{
    //protected values are assigned/computed
    this->sigma_s = sigma_s;
    this->sigma_r = sigma_r;
    this->sigma_t = sigma_t;

    fGLsm = new FilterGLSamplingMap(sigma_s);
    imgTmp = NULL;

    frame = 0;

    int nRand = 32;

    Image tmp_imageRand(1, 128, 128, 1);
    tmp_imageRand.SetRand();
    tmp_imageRand *= float(nRand - 1);

    imageRand = new ImageGL(&tmp_imageRand, true);
    imageRand->generateTextureGL(GL_TEXTURE_2D, GL_INT);

    //Precomputation of the Gaussian Kernel
    int kernelSize = PrecomputedGaussian::KernelSize(sigma_s);
    int kernelSizeTime  = PrecomputedGaussian::KernelSize(sigma_t);

    int halfKernelSize = kernelSize >> 1;
    int halfKernelSizeTime = kernelSizeTime >> 1;

    //Poisson samples
#ifdef PIC_DEBUG
    printf("Window: %d\n", halfKernelSize);
#endif

    Vec<3, int> window = Vec<3, int>(halfKernelSize, halfKernelSize,
                                     halfKernelSizeTime);
    ms = new MRSamplersGL<3>(ST_BRIDSON, window, halfKernelSize, 3, nRand);

    ms->generateTexture();
    ms->generateLevelsRTexture();

    FragmentShader();
    InitShaders();
}

void FilterGLBilateral3DAS::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2DArray	u_tex;
                          uniform isampler2D	u_poisson;
                          uniform sampler2D	u_sample;
                          uniform isampler2D	u_rand;
                          uniform isampler2D	u_levelsR;
                          uniform float		sigmas2;
                          uniform float		sigmar2;
                          uniform	int			levelsR_Size;
                          uniform	int			frame;
                          out     vec4		f_color;

                          //Calculate the number of samples
    int CalculateSamples(int shifter, ivec3 tSize) {
        //Fetch to the sampling map
        float levelVal = 2.0 * texture(u_sample, gl_FragCoord.xy / tSize.xy).x;
        levelVal = clamp(1.0f - levelVal, 0.0, 1.0) * float(levelsR_Size);

        int levelInt = int(floor(levelVal));

        int nSamples = texelFetch(u_levelsR, ivec2(levelInt, shifter), 0).x;

        if(levelInt < (levelsR_Size - 1)) {
            float tmp = (levelVal - float(levelInt));

            if(tmp > 0.0) {
                int nSamples1 = texelFetch(u_levelsR, ivec2(levelInt + 1, shifter), 0).x;

                nSamples += int(float(nSamples1 - nSamples) * tmp);
            }
        }

        return nSamples / 2;
    }

    void main(void) {
        //Calculating the number of samples
        ivec3 tSize =  textureSize(u_tex, 0);
        ivec3 coordsFrag = ivec3(gl_FragCoord.xy, frame % tSize.z);

        //Coordinates
        int shifter = texelFetch(u_rand, coordsFrag.xy % 128, 0).x;

        int nSamples = CalculateSamples(shifter, tSize);

        //Filtering
        vec3 tmpCol;
        vec3 colRef = texelFetch(u_tex, coordsFrag, 0).xyz;
        vec3  color = vec3(0.0);
        float weight = 0.0;

        for(int i = 0; i < nSamples; i++) {
            ivec4 coords = texelFetch(u_poisson, ivec2(i, shifter), 0);

            //Texture fetch
            ivec3 tmpCoords;
            tmpCoords.xy = coords.xy + coordsFrag.xy;
            tmpCoords.z = (frame + coords.z) % tSize.z;

            tmpCol = texelFetch(u_tex, tmpCoords, 0).xyz;
            vec3 tmpCol2 = tmpCol - colRef;
            float dstR = dot(tmpCol2.xyz, tmpCol2.xyz);
            float tmp = exp(-dstR / sigmar2 - float(coords.z) / sigmas2);
            color += tmpCol * tmp;
            weight += tmp;
        }

        color = weight > 0.0 ? color / weight : colRef;
        f_color = vec4(color, 1.0);
    }
                      );
}

void FilterGLBilateral3DAS::InitShaders()
{
    printf("Number of samples: %d\n", ms->nSamples / 2);

    std::string prefix;
    prefix += glw::version("150");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");
    filteringProgram.setup(prefix, vertex_source, fragment_source);
#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif
    glw::bind_program(filteringProgram);
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    filteringProgram.relink();
    glw::bind_program(0);

    Update(-1.0f, -1.0f, -1.0f);
}

//Change parameters
void FilterGLBilateral3DAS::Update(float sigma_s, float sigma_r, float sigma_t)
{

    bool flag = false;

    if(sigma_s > 0.0f) {
        flag = (this->sigma_s == sigma_s);
        this->sigma_s = sigma_s;
    }

    if(sigma_r > 0.0f) {
        flag = flag || (this->sigma_r == sigma_r);
        this->sigma_r = sigma_r;
    }

    if(sigma_t > 0.0f) {
        flag = flag || (this->sigma_t == sigma_t);
        this->sigma_t = sigma_t;
    }

    if(!flag) {
    }

    //shader update
    int kernelSize = PrecomputedGaussian::KernelSize(this->sigma_s);
    int halfKernelSize = kernelSize >> 1;

    ms->updateGL(halfKernelSize, halfKernelSize);

    //shader update
    float sigmas2 = 2.0f * this->sigma_s * this->sigma_s;
    float sigmar2 = 2.0f * this->sigma_r * this->sigma_r;
    float sigmat2 = 2.0f * this->sigma_t *this->sigma_t;

    glw::bind_program(filteringProgram);
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("u_poisson",  1);
    filteringProgram.uniform("u_rand",	 2);
    filteringProgram.uniform("u_sample",	 3);
    filteringProgram.uniform("u_levelsR",	 4);
    filteringProgram.uniform("frame",  frame);
    filteringProgram.uniform("sigmas2",  sigmas2);
    filteringProgram.uniform("sigmar2",  sigmar2);
    filteringProgram.uniform("sigmat2",  sigmat2);
    filteringProgram.uniform("levelsR_Size", ms->nLevels);
    glw::bind_program(0);
}

//Processing
ImageGL *FilterGLBilateral3DAS::Process(ImageGLVec imgIn,
        ImageGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageGL(1, w, h, imgIn[0]->channels, IMG_GPU, imgIn[0]->getTarget());
    }

    if(fbo == NULL) {
        fbo = new Fbo();
        fbo->create(w, h, 1, false, imgOut->getTexture());
    }

    if(imgTmp == NULL) {
        float scale = fGLsm->getScale();
        imgFrame->generateTextureGL(false, GL_TEXTURE_2D);

        imgTmp = new ImageGL(    1,
                                    int(imgIn[0]->widthf  * scale),
                                    int(imgIn[0]->heightf * scale),
                                    1, IMG_GPU, GL_TEXTURE_2D);
    }

    ImageGL *edge, *base;

    if(imgIn.size() == 2) {
        //Joint/Cross Bilateral Filtering
        base = imgIn[0];
        edge = imgIn[1];
    } else {
        base = imgIn[0];
        edge = imgIn[0];
    }

    //Calculating the sampling map
    fGLsm->Process(SingleGL(imgFrame), imgTmp);

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ms->getLevelsRTexture());

    glActiveTexture(GL_TEXTURE3);
    imgTmp->bindTexture();

    glActiveTexture(GL_TEXTURE2);
    imageRand->bindTexture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ms->getTexture());

    glActiveTexture(GL_TEXTURE0);
    base->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE3);
    imgTmp->unBindTexture();

    glActiveTexture(GL_TEXTURE2);
    imageRand->unBindTexture();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);
    base->unBindTexture();

    return imgOut;
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_BILATERAL_3DAS_HPP */

