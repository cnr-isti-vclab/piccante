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

#ifndef PIC_GL_COLORS_COLOR_CONV_HPP
#define PIC_GL_COLORS_COLOR_CONV_HPP

#include <string>

#include "../../util/gl/technique.hpp"

namespace pic {

/**
 * @brief The ColorConvGL class
 */
class ColorConvGL
{
protected:
    int direct;
    TechniqueGL techniques[2];

public:

    /**
     * @brief ColorConv
     */
    ColorConvGL(bool direct = true)
    {
        setTransform(direct);
    }

    /**
     * @brief getDirectFunction
     * @return
     */
    virtual std::string getDirectFunction() = 0;

    /**
     * @brief getDirectFunctionAux
     * @return
     */
    virtual std::string getDirectFunctionAux() = 0;

    /**
     * @brief getDirectUniforms
     * @return
     */
    virtual std::string getDirectUniforms() = 0;

    /**
     * @brief getInverseFunction
     * @return
     */
    virtual std::string getInverseFunction() = 0;

    /**
     * @brief getInverseUniforms
     * @return
     */
    virtual std::string getInverseUniforms() = 0;

    /**
     * @brief getInverseFunctionAux
     * @return
     */
    virtual std::string getInverseFunctionAux() = 0;

    /**
     * @brief generatePrograms
     * @param vertex_source
     */
    void generatePrograms(std::string vertex_source)
    {
        //direct transform
        techniques[0].initStandard("330", vertex_source, getDirectFunction(), "ColorConv (direct)");

        techniques[0].bind();
        techniques[0].setUniform1i("u_tex", 0);
        techniques[0].unbind();

        //inverse transform
        techniques[1].initStandard("330", vertex_source, getInverseFunction(), "ColorConv (inverse)");

        techniques[1].bind();
        techniques[1].setUniform1i("u_tex", 0);
        techniques[1].unbind();
    }

    /**
     * @brief setTransform
     * @param direct
     */
    void setTransform(bool direct)
    {
        this->direct = direct ? 0 : 1;
    }

    /**
     * @brief bindProgram
     */
    void bindProgram()
    {
        techniques[direct].bind();
    }

    /**
     * @brief unbindProgram
     */
    void unbindProgram()
    {
        techniques[direct].unbind();
    }
    
    /**
     * @brief setUniforms
     */
    virtual void setUniforms()
    {        
    }
};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_HPP */

