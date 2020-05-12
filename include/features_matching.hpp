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

#ifndef PIC_FEATURES_MATCHING_HPP
#define PIC_FEATURES_MATCHING_HPP

#include "util/eigen_util.hpp"

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
#include "features_matching/orb_descriptor.hpp"
#include "features_matching/sift_descriptor.hpp"

#include "features_matching/patch_comp.hpp"
#include "features_matching/transform_data.hpp"

#include "features_matching/ward_alignment.hpp"
#include "features_matching/motion_estimation.hpp"

//binary feature matcher
#include "features_matching/feature_matcher.hpp"
#include "features_matching/binary_feature_brute_force_matcher.hpp"
#include "features_matching/binary_feature_lsh_matcher.hpp"

#endif /* PIC_FEATURES_MATCHING_HPP */

