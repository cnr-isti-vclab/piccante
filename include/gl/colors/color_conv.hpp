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

#ifndef PIC_GL_COLORS_COLOR_CONV_HPP
#define PIC_GL_COLORS_COLOR_CONV_HPP

#include <string>

#include "externals/glw/program.hpp"

namespace pic {

/**
 * @brief The ColorConvGL class
 */
class ColorConvGL
{
protected:
    bool direct;

    glw::program programs[2];

public:

    /**
     * @brief ColorConv
     */
    ColorConvGL(bool direct = true)
    {
        this->direct = direct;
    }

    /**
     * @brief getDirectFunction
     * @return
     */
    virtual std::string getDirectFunction() = 0;

    /**
     * @brief getInverseFunction
     * @return
     */
    virtual std::string getInverseFunction() = 0;

    /**
     * @brief generatePrograms
     * @param vertex_source
     */
    void generatePrograms(std::string vertex_source)
    {
        //direct transform
        programs[0].setup(glw::version("330"), vertex_source, getDirectFunction());

        #ifdef PIC_DEBUG
            printf("[ColorConv direct log]\n%s\n", programs[0].log().c_str());
        #endif

        glw::bind_program(programs[0]);
        programs[0].attribute_source("a_position", 0);
        programs[0].fragment_target("f_color", 0);
        programs[0].relink();
        programs[0].uniform("u_tex", 0);
        glw::bind_program(0);

        //inverse transform
        programs[1].setup(glw::version("330"), vertex_source, getInverseFunction());

        #ifdef PIC_DEBUG
            printf("[ColorConv inverse log]\n%s\n", programs[1].log().c_str());
        #endif

        glw::bind_program(programs[1]);
        programs[1].attribute_source("a_position", 0);
        programs[1].fragment_target("f_color", 0);
        programs[1].relink();
        programs[1].uniform("u_tex", 0);
        glw::bind_program(0);
    }

    /**
     * @brief setTransform
     * @param direct
     */
    void setTransform(bool direct)
    {
        this->direct = direct;
    }

    /**
     * @brief bindProgram
     */
    void bindProgram()
    {
        if(direct) {
            glw::bind_program(programs[0]);
        } else {
            glw::bind_program(programs[1]);
        }
    }

    /**
     * @brief unbindProgram
     */
    void unbindProgram()
    {
        glw::bind_program(0);
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

