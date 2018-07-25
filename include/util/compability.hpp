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

#ifndef PIC_UTIL_COMPABILITY_HPP
#define PIC_UTIL_COMPABILITY_HPP

#include "../base.hpp"

namespace pic {

#ifndef PIC_WIN32
PIC_INLINE int timeGetTime()
{
    return 1;
}
#endif

} // end namespace pic

#endif /* PIC_UTIL_COMPABILITY_HPP */

