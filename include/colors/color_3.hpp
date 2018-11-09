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

#ifndef PIC_COLORS_COLOR_3_HPP
#define PIC_COLORS_COLOR_3_HPP

//typedef float float;
#include "../colors/color.hpp"

namespace pic {

/**
 * @brief Color3
 */
typedef Vec<3, float> Color3;

//basic colors
const Color3 RED    = Color3(1.0f, 0.0f, 0.0f);
const Color3 GREEN  = Color3(0.0f, 1.0f, 0.0f);
const Color3 BLUE   = Color3(0.0f, 0.0f, 1.0f);

const Color3 BLACK  = Color3(0.0f, 0.0f, 0.0f);
const Color3 WHITE  = Color3(1.0f, 1.0f, 1.0f);

} // end namespace pic

#endif /* PIC_COLORS_COLOR_3_HPP */

