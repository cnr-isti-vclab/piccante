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

#ifndef PIC_BASE_HPP
#define PIC_BASE_HPP

namespace pic {

typedef unsigned int uint;
typedef unsigned char uchar;

typedef uint* puint;
typedef uchar* puchar;


#ifndef PIC_DISABLE_INLINING

#ifndef PIC_INLINE
    #define PIC_INLINE inline
#endif

#else

#ifndef PIC_INLINE
    #define PIC_INLINE
#endif

#endif /* PIC_ENABLE_INLINING */

}

#endif /* PIC_BASE_HPP */

