/*

PICCANTE
The hottest HDR imaging library!
http://piccantelib.net

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

/*! \mainpage Welcome to the PICCANTE documentation.
 *
 * \section intro_sec Introduction
 *
 * <a href="http://piccantelib.net">PICCANTE</a> is a C++11 image processing library aimed
 * to provide structures and functionalities for enabling
 * both High Dynamic Range (HDR) and standard imaging.
 *
 * \subsection Usage
 *
 * To use <a href="http://piccantelib.net">PICCANTE</a> simply set the wanted options and include \c "piccante.hpp"
 *
 * The options are set with a  \c #define and are:
 *
 * \li \c PIC_DEBUG used for debugging; it mostly enables some printf messages;
 * i.e. for warning when a computation succeeds or fails.
 * \li \c PIC_DISABLE_OPENGL disables the OpenGL support.
 * \li \c PIC_ENABLE_OPEN_EXR enables the support for the OpenEXR library. This may be useful to have
 * in the case .exr images are used. Note that you need to manually install OpenEXR on your developing maching in order
 * to enable this flag.
 * \li \c PIC_DISABLE_TINY_EXR disables the support for the reading EXR files using TinyEXR library (https://github.com/syoyo/tinyexr).
 * This may be useful to have in the case .exr images are used. Note that TinyEXR is already bundled into Piccante (include/externals).
 * \li \c PIC_DISABLE_STB disables the use of STB for reading/writing PNG and JPEG files (https://github.com/nothings/stb).
 * If it is not defined, picccante.hpp searchs for STB in "../../stb"
 * \li \c PIC_DISABLE_STB_LOCAL disables the use of local STB (i.e., placed in "../../stb")
 *
 * Note that when using Eigen types and standard containters, if you do not align containters, a good practice is to enable the following #define:
 * \li \c EIGEN_DONT_VECTORIZE
 * \li \c EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
 *
 * If you want to use your Eigen version you need to the add the following #define:
 * \li \c PIC_EIGEN_NOT_BUNDLED
 *
 * \section descr_sec Modules
 *
 * <a href="http://piccantelib.net">PICCANTE</a> is a modular library with different modules. The main classes are
 * pic::Image and pic::Histogram.
 * \li \c pic::Image is the base class for managing an image. Pixels are stored
 * in an array of interleaved channels float values; i.e. pic::Image::data.
 * Pixels are stored as float values, because the library is meant mostly for accurate and HDR
 * imaging processing. This class provides standard functions for extracting
 * image statistics (e.g. maximum value, minimum value, mean value, etc.), image operators
 * (e.g. add, sub, mul, div, etc.) and memory management functions (e.g. allocation, cloning, etc.).
 * Note that this class supports multi-channels (e.g. alpha channel) and temporal/volumetric images.
 * It also provides an I/O interface for reading different file formats (ppm, pgm, pbm, bmp, tga, hdr, pfm, etc.)
 * natively in Piccante and through other optional external libraries (e.g. OpenEXR and QT).
 * \li \c pic::Histogram is a class for creating, managing, and processing LDR/HDR image histograms.
 *
 * \subsection filters_module Filtering
 *
 * The main class of this module is pic::Filter. This is a base class for managing
 * a filter; it provides mechanisms for multi-threading, memory allocation, and so on.
 * Typically, a filter in <a href="http://piccantelib.net">PICCANTE</a> can have multiple pic::Image images as input, imgIn,
 * and a single output, imgOut.
 * Many image filters are implemented in <a href="http://piccantelib.net">PICCANTE</a> susch as: linear filters
 * (e.g. Gaussian, gradient based, DCT, etc.), non-linear filters
 * (e.g. bilateral, anisotropic diffusion, guided, median, etc.), and image transforms
 * (e.g. warping).
 *
 * \subsection algo_module Algorithms
 *
 * This module contains high-level imaging functionalities such as
 * Laplacian/Gaussian pyramids, Push-pull, a simple Poisson solver, SuperPixels (SLIC),
 * a simple gradient based demosacing method, live-wire counturing, Grow-Cut segmentation, etc.
 * This module provides classes and functions for HDR imaging such as a class for
 * merging LDR images at different exposures, a class for estimating a camera response function (CRF), etc.
 *
 * \subsection colors_module Colors
 *
 * This module provides classes and methods for editing, processing
 * and converting colors.
 * Supported color spaces:
 *
 * For example, the class pic::Color3 provides a basic type for three color
 * components representations. This can be useful for some applications
 * such as a 3D renderer.
 *
 * \subsection io_module Input and Output
 *
 * This module provides functions for reading and writing images natively in different
 * file formats such as:
 * \li \c BMP: Windows bitmap file; 24-bit color images are only supported.
 * \li \c HDR: Greg Ward's RGBE format.
 * \li \c PGM: Portable Gray Map images; greyscale images.
 * \li \c PPM: Portable Pixel Map images; color images.
 * \li \c PFM: Portable Float Map images; HDR color images.
 * \li \c TGA: targa file; 24-bit color images are only supported.
 * \li \c TMP: a dump of the pic::Image data.
 * \li \c VOL: a volumetric format for rendering; 32-bit per voxel.
 *
 * The module provides an interface for OpenEXR, but it requires
 * either the linking with the OpenEXR library (see <a href="http://www.openexr.com">the official website</a>)
 * or the use of TinyEXR (see <a href="https://github.com/syoyo/tinyexr"> the official website</a>):
 * \li \c EXR: ILM's OpenEXR format; HDR color images at 16-bit per component.
 *
 * In addition, the module provides an interface for STB, which is a library for reading/writing LDR images. This
 * library is required for reading/writing JPEG and PNG files. This can be found at its <a href="https://github.com/nothings/stb">official website</a>.
 *
 * \subsection metrics_module Metrics
 *
 * This module provides classic objective metrics for measuring differences in images.
 * Several metrics are provided such as: PSNR, mPSNR (for HDR images), TMQI (for tone mapped images), SSIM, RMSE,
 * logRMSE (for HDR images), maximum error, relative error, etc. This metrics can be applied to HDR images using PU encoding.
 *
 * \subsection ps_module Point Samplers
 *
 * This module provides structures and functions for generating points' set in n-dimensions using
 * different distributions such as: random, stratified random, regular, Poisson-disk, etc.
 * Points generated with such distributions may be useful for filtering algorithms.
 *
 * \subsection is_module Image Samplers
 *
 * This module provides methods for sampling 2D and 3D images using different filters
 * such as: nearest neighbors filter, bilinear filter, bi-cubic filter, Gaussian filter,
 * etc.
 *
 * \subsection tm_module Tone Mapping
 *
 * This module provides tone mapping operators (TMOs) for reducing the dynamic
 * range in HDR images. Several TMOs are present such as: Ward Histogram Adjustment,
 * Reinhard Photographic Tone Reproduction Operator, Lischinski Improved Photographic Tone
 * Reproduction Operator, Drago TMO, Banterle Hybrid TMO, Schlick TMO, Tumblin TMO, Ward Global TMO,
 * Raman TMO, Durand and Dorsey TMO, etc.
 *
 * \subsection fm_module Features and Matching
 *
 * This module provides classes and functions for extracting 2D features from 2D images,
 * and matching the extracted features. This may be useful for aligning images for different
 * tasks such as: HDR exposures stack alignment, generation of panoramas, etc.
 * Different features can be extracted and matched:
 * \li \c Corners (Key-point): SUSAN, Harris' method, and FAST.
 * \li \c Edges: Canny's method, and Ward's MTB.
 * \li \c Key-point descriptors: BRIEF, ORB, and LUCID.
 *
 *  \subsection cv_module Computer Vision
 *
 * This module provides classes and functions for Computer Vision tasks such as
 * checker board extraction, computation of the Essential matrix, computation of the Fundamental Matrix,
 * estimation of homographies, and triangulation.
 *
 * \subsection gl_module OpenGL
 * This module provides GPU acceleration for some functionalities of PICCANTE through OpenGL.
 * In particular, the module uses OpenGL 4.0 Core profile only, and it is independent from OpenGL
 * loading extensions libraries; users are free to use their favourite ones.
 * Note that when using OpenGL and QT together; QT will load OpenGL functions by default
 * in order to avoid clashes.
 * In the examples (folder “examples”), we generated .h and .c files for loading OpenGL extensions
 * using glLoadGen. This is meant for learning purposes only; we do not want to force users to use
 * it; e.g. GLEW or other libraries can be employed instead.
 *
 * \subsection utils_module Utilities
 *
 * This module provides different utilies for manipulating strings,
 * arrays, indexed arrays, math functions, 2D arrays, vectors, etc.
 *
 */

#ifndef PIC_PICCANTE_HPP
#define PIC_PICCANTE_HPP

#ifdef _MSC_VER
//we are using windows
    #define PIC_WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#elif __APPLE__
    //we are using mac os x
    #define PIC_MAC_OS_X
#else
    // we assume that we are using a UNIX system
    #define PIC_UNIX
#endif

//Mac OS X
#ifdef PIC_MAC_OS_X
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"
#pragma clang diagnostic ignored "-Wsign-compare"
#pragma clang diagnostic ignored "-Wformat"
#endif

//Win32
#ifdef PIC_WIN32

#pragma warning(disable:4100)
#pragma warning(disable:4146)

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <winuser.h>
#include <vfw.h>
#include <tchar.h>
#include <direct.h>
#include <mmsystem.h>
#define strcasecmp stricmp
#pragma comment( lib, "Winmm" )
#pragma comment( lib, "vfw32" )
#endif

// base stuff
#include "base.hpp"
#include "image.hpp"
#include "image_vec.hpp"
#include "histogram.hpp"

// sub dirs
#include "algorithms.hpp"
#include "colors.hpp"
#include "features_matching.hpp"
#include "filtering.hpp"
#include "gl.hpp"
#include "image_samplers.hpp"
#include "io.hpp"
#include "metrics.hpp"
#include "point_samplers.hpp"
#include "tone_mapping.hpp"
#include "util.hpp"
#include "computer_vision.hpp"

#include "JNI.hpp"

#ifdef PIC_MAC_OS_X
#pragma clang diagnostic pop
#endif

#endif /* PIC_PICCANTE_HPP */

