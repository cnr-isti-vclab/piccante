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

#ifndef PIC_UTIL_HPP
#define PIC_UTIL_HPP

#include "util/array.hpp"
#include "util/indexed_array.hpp"
#include "util/bbox.hpp"
#include "util/buffer.hpp"
#include "util/mask.hpp"
#include "util/cached_table.hpp"
#include "util/compability.hpp"
#include "util/convert_raw_to_images.hpp"
#include "util/file_lister.hpp"

#ifndef PIC_DISABLE_OPENGL
#include "util/gl/stroke.hpp"
#include "util/gl/fbo.hpp"
#include "util/gl/formats.hpp"
#include "util/gl/quad.hpp"
#include "util/gl/timings.hpp"
#include "util/gl/tone.hpp"
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

