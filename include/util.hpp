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

#ifndef PIC_UTIL_HPP
#define PIC_UTIL_HPP

#include "util/array.hpp"
#include "util/indexed_array.hpp"
#include "util/bbox.hpp"
#include "util/buffer.hpp"
#include "util/mask.hpp"
#include "util/cached_table.hpp"
#include "util/compability.hpp"
//#include "util/convert_raw_to_images.hpp"
#include "util/file_lister.hpp"

#ifndef PIC_DISABLE_OPENGL
#include "util/gl/stroke.hpp"
#include "util/gl/fbo.hpp"
#include "util/gl/formats.hpp"
#include "util/gl/quad.hpp"
#include "util/gl/timings.hpp"
#include "util/gl/tone.hpp"
#include "util/gl/buffer_ops.hpp"
#include "util/gl/buffer_allocation.hpp"
#include "util/gl/redux.hpp"
#include "util/gl/redux_ops.hpp"
#include "util/gl/mask.hpp"
#endif

#include "util/image_sampler.hpp"
#include "util/io.hpp"
#include "util/math.hpp"
#include "util/matrix_3_x_3.hpp"
#include "util/eigen_util.hpp"
#include "util/computer_vision_functions.hpp"
#include "util/point_samplers.hpp"
#include "util/precomputed_difference_of_gaussians.hpp"
#include "util/precomputed_gaussian.hpp"
#include "util/raw.hpp"
#include "util/string.hpp"
#include "util/tile.hpp"
#include "util/tile_list.hpp"
#include "util/vec.hpp"
#include "util/warp_square_circle.hpp"
#include "util/rasterizer.hpp"

//optimization
#include "util/nelder_mead_opt_base.hpp"
#include "util/nelder_mead_opt_homography.hpp"
#include "util/nelder_mead_opt_fundamental.hpp"
#include "util/nelder_mead_opt_triangulation.hpp"
#include "util/nelder_mead_opt_gordon_lowe.hpp"
#include "util/nelder_mead_opt_test_function.hpp"

#endif /* PIC_UTIL_HPP */

