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

#ifndef PIC_FEATURES_MATCHING_HPP
#define PIC_FEATURES_MATCHING_HPP

//Corner descriptors
#include "features_matching/general_corner_detector.hpp"
#include "features_matching/harris_corner_detector.hpp"
#include "features_matching/susan_corner_detector.hpp"
#include "features_matching/fast_corner_detector.hpp"

//Edge descriptors
#include "features_matching/canny_edge_detector.hpp"

//Feature descriptors
#include "features_matching/lucid_descriptor.hpp"
#include "features_matching/brief_descriptor.hpp"
#include "features_matching/poisson_descriptor.hpp"
#include "features_matching/orb_descriptor.hpp"

#include "features_matching/dense_sift.hpp"
#include "features_matching/patch_comp.hpp"
#include "features_matching/transform_data.hpp"

#include "features_matching/ward_alignment.hpp"
#include "features_matching/motion_estimation.hpp"

#endif /* PIC_FEATURES_MATCHING_HPP */

