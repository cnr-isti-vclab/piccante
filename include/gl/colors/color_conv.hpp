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

namespace pic {

/**
 * @brief The ColorConvGL class
 */
class ColorConvGL
{
protected:
    bool direct;

public:

    /**
     * @brief ColorConv
     */
    ColorConv()
    {
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
     * @brief setDirect
     * @param direct
     */
    void setDirect(bool direct)
    {
        this->direct = direct;
    }
    
    /**
     * @brief setUniforms
     * @param program
     */
    virtual setUniforms(glw::program program)
    {
        
    }
};

} // end namespace pic

#endif /* PIC_GL_COLORS_COLOR_CONV_HPP */

