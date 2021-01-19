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


#ifndef PIC_FILTERING_HPP
#define PIC_FILTERING_HPP

#include "filtering/filter.hpp"
#include "filtering/filter_down_pp.hpp"
#include "filtering/filter_up_pp.hpp"
#include "filtering/filter_white_balance.hpp"
#include "filtering/filter_integral_image.hpp"
#include "filtering/filter_reconstruct.hpp"
#include "filtering/filter_local_extrema.hpp"
#include "filtering/filter_warp_2d.hpp"
#include "filtering/filter_absolute_difference.hpp"
#include "filtering/filter_anisotropic_diffusion.hpp"
#include "filtering/filter_assemble_hdr.hpp"
#include "filtering/filter_backward_difference.hpp"
#include "filtering/filter_bilateral_1d.hpp"
#include "filtering/filter_bilateral_2das.hpp"
#include "filtering/filter_bilateral_2df.hpp"
#include "filtering/filter_bilateral_2dg.hpp"
#include "filtering/filter_bilateral_2ds.hpp"
#include "filtering/filter_bilateral_2dsp.hpp"
#include "filtering/filter_channel.hpp"
#include "filtering/filter_color_conv.hpp"
#include "filtering/filter_color_distance.hpp"
#include "filtering/filter_combine.hpp"
#include "filtering/filter_conv_1d.hpp"
#include "filtering/filter_conv_2d.hpp"
#include "filtering/filter_conv_2dsp.hpp"
#include "filtering/filter_crop.hpp"
#include "filtering/filter_dct_1d.hpp"
#include "filtering/filter_dct_2d.hpp"
#include "filtering/filter_diff_gauss_2d.hpp"
#include "filtering/filter_diff_gauss_2d_opt.hpp"
#include "filtering/filter_log_2d.hpp"
#include "filtering/filter_log_2d_opt.hpp"
#include "filtering/filter_divergence.hpp"
#include "filtering/filter_downsampler_2d.hpp"
#include "filtering/filter_drago_tmo.hpp"
#include "filtering/filter_gaussian_1d.hpp"
#include "filtering/filter_gaussian_2d.hpp"
#include "filtering/filter_gaussian_3d.hpp"
#include "filtering/filter_gradient.hpp"
#include "filtering/filter_gradient_harris_opt.hpp"
#include "filtering/filter_guided_a_b.hpp"
#include "filtering/filter_guided.hpp"
#include "filtering/filter_iterative.hpp"
#include "filtering/filter_kuwahara.hpp"
#include "filtering/filter_laplacian.hpp"
#include "filtering/filter_linear_color_space.hpp"
#include "filtering/filter_luminance.hpp"
#include "filtering/filter_max.hpp"
#include "filtering/filter_mean.hpp"
#include "filtering/filter_med.hpp"
#include "filtering/filter_med_vec.hpp"
#include "filtering/filter_min.hpp"
#include "filtering/filter_mosaic.hpp"
#include "filtering/filter_demosaic.hpp"
#include "filtering/filter_normal.hpp"
#include "filtering/filter_npasses.hpp"
#include "filtering/filter_nswe.hpp"
#include "filtering/filter_zero_crossing.hpp"
#include "filtering/filter_remove_nuked.hpp"
#include "filtering/filter_remove_inf_nan.hpp"
#include "filtering/filter_sampler_1d.hpp"
#include "filtering/filter_sampler_2d.hpp"
#include "filtering/filter_sampler_2dadd.hpp"
#include "filtering/filter_sampler_2dsub.hpp"
#include "filtering/filter_sampler_3d.hpp"
#include "filtering/filter_sampling_map.hpp"
#include "filtering/filter_sigmoid_tmo.hpp"
#include "filtering/filter_simple_tmo.hpp"
#include "filtering/filter_wls.hpp"
#include "filtering/filter_grow_cut.hpp"
#include "filtering/filter_deform_grid.hpp"
#include "filtering/filter_radial_basis_function.hpp"
#include "filtering/filter_disparity.hpp"
#include "filtering/filter_deconvolution.hpp"
#include "filtering/filter_noise_estimation.hpp"
#include "filtering/filter_tmqi.hpp"
#include "filtering/filter_nearest_neighbors.hpp"
#include "filtering/filter_luminance_adaptation.hpp"
#include "filtering/filter_clahe.hpp"
#include "filtering/filter_color_correction_pouli.hpp"

//360 panoramic images
#include "filtering/filter_rotation.hpp"

#endif /* PIC_FILTERING_HPP */

