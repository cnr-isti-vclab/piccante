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

#ifndef PIC_GL_HPP
#define PIC_GL_HPP

#ifndef PIC_DISABLE_OPENGL

//OpenGL library
#ifdef PIC_WIN32
    #include <gl/GL.h>
    #pragma comment( lib, "opengl32" )
#else /* PIC_MAC_OS_X or PIC_UNIX */

    #ifdef PIC_MAC_OS_X
        #include <OpenGL/OpenGL.h>
    #else
        #include <GL/gl.h>
    #endif
#endif /* os selection */

//end os selection

#include "util/gl/program.hpp"
#include "util/gl/technique.hpp"

#include "gl/algorithms/pyramid.hpp"
#include "gl/algorithms/pushpull.hpp"
#include "gl/algorithms/color_to_gray.hpp"
#include "gl/algorithms/richardson_lucy_deconvolution.hpp"
#include "gl/algorithms/grow_cut.hpp"

#include "util/gl/buffer_op.hpp"
#include "util/gl/buffer_ops.hpp"

#include "gl/filtering/filter.hpp"
#include "gl/filtering/filter_luminance.hpp"
#include "gl/filtering/filter_channel.hpp"

//color conversion
#include "gl/colors/color_conv.hpp"
#include "gl/colors/color_conv_linear.hpp"
#include "gl/colors/color_conv_rgb_to_xyz.hpp"
#include "gl/colors/color_conv_rgb_to_srgb.hpp"
#include "gl/colors/color_conv_rgb_to_hsl.hpp"
#include "gl/colors/color_conv_xyz_to_cielab.hpp"
#include "gl/colors/color_conv_xyz_to_lms.hpp"
#include "gl/filtering/filter_color_conv.hpp"

#include "gl/filtering/filter_warp_2d.hpp"
#include "gl/filtering/filter_anisotropic_diffusion.hpp"
#include "gl/filtering/filter_bilateral_1d.hpp"
#include "gl/filtering/filter_bilateral_2das.hpp"
#include "gl/filtering/filter_bilateral_2df.hpp"
#include "gl/filtering/filter_bilateral_2dg.hpp"
#include "gl/filtering/filter_bilateral_2ds.hpp"
#include "gl/filtering/filter_bilateral_2ds_e.hpp"
#include "gl/filtering/filter_bilateral_2dsp.hpp"
#include "gl/filtering/filter_bilateral_3ds.hpp"
#include "gl/filtering/filter_bilateral_3dsp.hpp"
#include "gl/filtering/filter_disp.hpp"
#include "gl/filtering/filter_drago_tmo.hpp"
#include "gl/filtering/filter_1d.hpp"
#include "gl/filtering/filter_conv_1d.hpp"
#include "gl/filtering/filter_conv_2d.hpp"
#include "gl/filtering/filter_non_linear_1d.hpp"
#include "gl/filtering/filter_mean.hpp"
#include "gl/filtering/filter_min.hpp"
#include "gl/filtering/filter_max.hpp"
#include "gl/filtering/filter_gaussian_1d.hpp"
#include "gl/filtering/filter_gaussian_2d.hpp"
#include "gl/filtering/filter_gaussian_3d.hpp"
#include "gl/filtering/filter_gradient.hpp"
#include "gl/filtering/filter_laplacian.hpp"
#include "gl/filtering/filter_hsl_replace.hpp"
#include "gl/filtering/filter_iterative.hpp"
#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_op.hpp"
#include "gl/filtering/filter_remapping.hpp"
#include "gl/filtering/filter_remove_nuked.hpp"
#include "gl/filtering/filter_sampler_2d.hpp"
#include "gl/filtering/filter_sampling_map.hpp"
#include "gl/filtering/filter_scatter.hpp"
#include "gl/filtering/filter_sigmoid_tmo.hpp"
#include "gl/filtering/filter_simple_tmo.hpp"
#include "gl/filtering/filter_slicer.hpp"
#include "gl/filtering/filter_durand_tmo.hpp"
#include "gl/filtering/filter_deform_grid.hpp"
#include "gl/image.hpp"
#include "gl/image_vec.hpp"
#include "gl/point_samplers/sampler_random_m.hpp"

//Tone mapping
#include "gl/tone_mapping/segmentation_tmo_approx.hpp"
#include "gl/tone_mapping/drago_tmo.hpp"
#include "gl/tone_mapping/reinhard_tmo.hpp"
#include "gl/tone_mapping/durand_tmo.hpp"
#include "gl/tone_mapping/hybrid_tmo.hpp"
#include "gl/tone_mapping/get_all_exposures.hpp"
#include "gl/tone_mapping/exposure_fusion.hpp"

//Post tone mapping
#include "gl/filtering/filter_color_correction_pouli.hpp"

//Display
#include "util/gl/display.hpp"

#endif /* PIC_DISABLE_OPENGL */

#endif /* PIC_GL_HPP */

