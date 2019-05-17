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

#ifndef PIC_UTIL_GL_STROKE_HPP
#define PIC_UTIL_GL_STROKE_HPP

#include "../../util/rasterizer.hpp"
#include "../../gl/image.hpp"
#include "../../util/gl/quad.hpp"
#include "../../util/gl/technique.hpp"

namespace pic {

//TODO: removing immediate mode

/**
 * @brief The StrokeGL class
 */
class StrokeGL
{
protected:
    int	 width, height, brushSize;
    ImageGL *shape;
    float size, rSize;
    float color[4];
    float tmpColor[3];

    QuadGL *quad;

    std::vector<float>	positions;

    TechniqueGL annotationProgram;
    TechniqueGL brushProgram;

public:
    float annotation;

    /**
     * @brief StrokeGL
     * @param width
     * @param height
     * @param brushSize
     * @param color
     */
    StrokeGL(int width, int height, int brushSize, float *color);

    ~StrokeGL();

    /**
     * @brief initShaders
     */
    void initShaders();

    /**
     * @brief Resample
     */
    void Resample();

    /**
     * @brief Reset
     */
    void Reset();

    /**
     * @brief Insert2DPoint
     * @param x
     * @param y
     */
    void Insert2DPoint(int x, int y);

    /**
     * @brief RenderGL
     */
    void RenderGL();

    /**
     * @brief RenderAnnotationGL
     */
    void RenderAnnotationGL();

    /**
     * @brief RenderBrushGL
     * @param x
     * @param y
     */
    void RenderBrushGL(int x, int y);

    /**
     * @brief Size
     * @return
     */
    unsigned int Size()
    {
        return (unsigned int)(positions.size());
    }

    /**
     * @brief bindCol
     * @param val
     */
    void bindCol(float val)
    {
        memcpy(tmpColor, color, 3 * sizeof(float));
        color[0] = color[1] = color[2] = val;
    }

    /**
     * @brief unBindCol
     */
    void unBindCol()
    {
        memcpy(color, tmpColor, 3 * sizeof(float));
    }

    /**
     * @brief Straightner
     */
    void Straightner()
    {
        if(positions.size() > 3) {
            float x0 = positions[0];
            float y0 = positions[1];

            int n = int(positions.size()) - 2;
            float x1 = positions[n];
            float y1 = positions[n + 1];

            positions.clear();
            positions.push_back(x0);
            positions.push_back(y0);
            positions.push_back(x1);
            positions.push_back(y1);
        }
    }
};

PIC_INLINE StrokeGL::StrokeGL(int width, int height, int brushSize = 128,
                 float *color = NULL)
{
    annotation = 1.0f;


    this->width = width;
    this->height = height;

    if(brushSize > 1) {
        this->brushSize = brushSize;
    } else {
        this->brushSize = 2;
    }

    int halfBrushSize = this->brushSize >> 1;

    float halfBrushSizeXf = float(halfBrushSize) / float(width);
    float halfBrushSizeYf = float(halfBrushSize) / float(height);

    #ifdef PIC_DEBUG
        printf("%f %f\n", halfBrushSizeXf, halfBrushSizeYf);
    #endif

    this->quad = new QuadGL(true, halfBrushSizeXf, halfBrushSizeYf);

    size = 4.0f;
    rSize = size / float(MAX(width, height));

    shape = new ImageGL(1, this->brushSize, this->brushSize, 1, IMG_CPU, GL_TEXTURE_2D);
    evaluateSolid(shape);
    shape->generateTextureGL(GL_TEXTURE_2D, false);

    if(color != NULL) {
        for(int i = 0; i < 3; i++) {
            this->color[i] = color[i];
        }
    } else {
        this->color[0] = this->color[1] = this->color[2] = 1.0f;
    }

    initShaders();
}

PIC_INLINE StrokeGL::~StrokeGL()
{
    if(shape != NULL) {
        delete shape;
    }

    shape = NULL;
}

PIC_INLINE void StrokeGL::initShaders()
{
    //common vertex program
    std::string vertex_source = MAKE_STRING
                                (
    in vec2 a_position;
    in vec2 a_tex_coord;
    uniform vec2 shift_position;
    out vec2 v_tex_coord;

    void main(void) {
        v_tex_coord  = a_tex_coord;
        gl_Position = vec4(a_position + shift_position, 0.0, 1.0);
    }
                                );

    //render
    std::string fragment_source_brush = MAKE_STRING
                                  (
    uniform sampler2D   u_tex;
    uniform vec4        current_color;
    in vec2             v_tex_coord;
    out vec4            f_color;

    void main(void) {
        float shape = texture2D(u_tex, v_tex_coord).x;
        f_color = vec4(current_color * shape);
    }
                                  );

    brushProgram.init("330", vertex_source, fragment_source_brush);
    brushProgram.printLog("Brush - StrokeGL");
    
    brushProgram.bind();
    brushProgram.setAttributeIndex("a_position", 0);
    brushProgram.setAttributeIndex("a_tex_coord", 1);
    brushProgram.setOutputFragmentShaderIndex("f_color", 0);
    brushProgram.link();
    brushProgram.unbind();

    brushProgram.bind();
    brushProgram.setUniform1i("u_tex", 0);
    brushProgram.setUniform2f("shift_position", 0.0f, 0.0f);
    brushProgram.setUniform4f("current_color", 1.0f, 1.0f, 1.0f, 1.0f);
    brushProgram.unbind();

    //annotation
    std::string fragment_source_annotation = MAKE_STRING
                                  (
    uniform sampler2D   u_tex;
    uniform float       annotation;
    in vec2             v_tex_coord;
    out vec4            f_color;

    void main(void) {
        float shape = texture2D(u_tex, v_tex_coord).x;
        float shapeVal = shape * annotation;
        f_color = vec4(shapeVal, shapeVal, shapeVal, shape);
    }
                                  );

    annotationProgram.init("330", vertex_source, fragment_source_annotation);
    annotationProgram.printLog("Brush (Annotation) - StrokeGL");

    annotationProgram.bind();
    annotationProgram.setAttributeIndex("a_position", 0);
    annotationProgram.setAttributeIndex("a_tex_coord", 1);
    annotationProgram.setOutputFragmentShaderIndex("f_color", 0);
    annotationProgram.link();
    annotationProgram.unbind();

    annotationProgram.bind();
    annotationProgram.setUniform1i("u_tex", 0);
    annotationProgram.setUniform2f("shift_position", 0.0f, 0.0f);
    annotationProgram.setUniform1f("annotation", annotation);
    annotationProgram.unbind();
}

PIC_INLINE void StrokeGL::Resample()
{
    if(positions.size() <= 0) {
        return;
    }

    bool *sampleGrid = Mask::assign(NULL, height * width, false);

    //calculate length of the path
    float len = 0.0f;
    float tmpLen;
    const int n = int(positions.size()) - 2;
    float x, y;
    std::vector<float> lengths;

    for(int i = 0; i < n; i += 2) {
        x = positions[i + 2] - positions[i];
        y = positions[i + 3] - positions[i + 1];
        tmpLen = sqrtf(x * x + y * y);
        lengths.push_back(tmpLen);
        len += tmpLen;
    }

    //resample
    std::vector<float> resampledPos;
    resampledPos.push_back(positions[0]);
    resampledPos.push_back(positions[1]);

    float workLen = 0.0f;
    int nSamples = int(len / (rSize * 0.25f));
    float deltaL = len / float(nSamples);

#ifdef PIC_DEBUG
    printf("Len: %f Samples: %d DeltaL: %f\n", len, nSamples, deltaL);
#endif

    for(int i = 1; i < nSamples; i++) {
        workLen += deltaL;
        unsigned int j;
        bool test = false;
        float tmpWork = 0.0f;

        for(j = 0; j < lengths.size(); j++) {
            if(workLen >= tmpWork && workLen < (tmpWork + lengths[j])) {
                test = true;
                break;
            }

            tmpWork += lengths[j];
        }

        if(!test) {
            break;
        }

        int ind = j << 1;

        float x0 = positions[ind];
        float y0 = positions[ind + 1];

        float x1 = positions[ind + 2];
        float y1 = positions[ind + 3];

        float shift = (deltaL - (tmpWork - workLen)) / lengths[j];

        x =  shift * (x1 - x0) + x0;
        y =  shift * (y1 - y0) + y0;

        int tmpX = CLAMP(int((x / 2.0f + 0.5f) * width), width);
        int tmpY = CLAMP(int((y / 2.0f + 0.5f) * height), height);
        int indSG = tmpY * width + tmpX;
        bool tmpSampleGrid = sampleGrid[indSG];

//		float *val = ((*sampleGrid)(x/2.0f+0.5,y/2.0f+0.5f));

        if(tmpSampleGrid == false) {
            resampledPos.push_back(x);
            resampledPos.push_back(y);
            sampleGrid[indSG] = true;
        }

        //printf("%f %f %d %f\n",x,y,j,workLen);
        workLen += deltaL;

        if(workLen > len) {
            break;
        }
    }

    positions.clear();
    positions.insert(positions.begin(), resampledPos.begin(), resampledPos.end());

    if(sampleGrid != NULL) {
        delete[] sampleGrid;
    }
}

PIC_INLINE void StrokeGL::Insert2DPoint(int x, int y)
{
    /*
    	float xf = -(x/float(width) -0.5f)*2.0f;
    	float yf =  (y/float(height)-0.5f)*2.0f;
    */
    float xf = (x / float(width) - 0.5f) * 2.0f;
    float yf = (y / float(height) - 0.5f) * 2.0f;

    positions.push_back(xf);
    positions.push_back(yf);
}

PIC_INLINE void StrokeGL::Reset()
{
    positions.clear();
}

PIC_INLINE void StrokeGL::RenderBrushGL(int x, int y)
{
    float xf = (x / float(width)  - 0.5f) * 2.0f;
    float yf = (y / float(height) - 0.5f) * 2.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    shape->bindTexture();
      
        
    brushProgram.bind();
    if(annotation < 0.0f) {
        brushProgram.setUniform4f("current_color", 1.0f - color[0], 1.0f - color[1], 1.0f - color[2], 1.0f);
    } else {
        brushProgram.setUniform4f("current_color", color[0], color[1], color[2], 0.5f);
    }
    
    brushProgram.setUniform2f("shift_position", xf, yf);

    quad->Render();

    brushProgram.unbind();

    shape->unBindTexture();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

PIC_INLINE void StrokeGL::RenderGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    shape->bindTexture();

    glBindTexture(GL_TEXTURE_2D, shape->getTexture());

    brushProgram.bind();

    brushProgram.setUniform4f("current_color", color[0], color[1], color[2], 1.0f);
    const int n = int(positions.size());

    for(int i = 0; i < n; i += 2) {
        brushProgram.setUniform2f("shift_position", positions[i], positions[i + 1]);
        quad->Render();
    }

    brushProgram.unbind();

    shape->unBindTexture();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

PIC_INLINE void StrokeGL::RenderAnnotationGL()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    annotationProgram.bind();

#ifdef PIC_DEBUG
    printf("Annotation value: %f\n", annotation);
#endif

    annotationProgram.setUniform1f("annotation",  annotation);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    shape->bindTexture();

    const int n = int(positions.size());

    for(int i = 0; i < n; i += 2) {
        annotationProgram.setUniform2f("shift_position", positions[i], positions[i + 1]);
        quad->Render();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    annotationProgram.unbind();
    glDisable(GL_BLEND);
}

} // end namespace pic

#endif /* PIC_UTIL_GL_STROKE_HPP */
