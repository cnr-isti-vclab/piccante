PICCANTE
========

The hottest hdr imaging library

PICCANTE is a C++11 image processing library aimed to provide structures and functionalities for enabling both High Dynamic Range (HDR) and standard imaging.

HOW TO INSTALL:
===============

1) Unzip the file .zip in a FOLDER on your machine

2) Add piccante include directory in your include path

3) Include "piccante.hpp" in your project

NOTE ON PULL REQUESTS:
=====================
Please, send your pull requests to the develop branch.

TEAM:
=====

Francesco Banterle

Luca Benedetti

LICENSE:
========
Piccante is distributed under the MPL 2.0 license: https://www.mozilla.org/MPL/2.0/

REFERENCE:
==========

If you use PICCANTE in your work, please cite it using this reference:

@misc{banterle:pic:2014,

 Author = {Francesco Banterle and Luca Benedetti},

 Title = {{PICCANTE: An Open and Portable Library
          for HDR Imaging}},

 Year  = {2014},

 Howpublished = {\url{http://vcg.isti.cnr.it/piccante}
 }
 
FREQUENTLY ASKED QUESTIONS (FAQs):
==================================

Q: Can I use my version of Eigen instead of the one in the bundle?
A: Yes, you can do it. You just need to add this line of code before
including piccante.hpp in your project:
#define PIC_EIGEN_NOT_BUNDLED

Q: Can I use OpenEXR instead of TinyEXR?
A: Yes, you can do it. You just need to add these two lines of code before
including piccante.hpp in your project:
#define PIC_DISABLE_TINY_EXR
#define PIC_ENABLE_OPEN_EXR

SCREENSHOTS:
============
Piccante can be used for generating HDR images starting from a stack of classic 8-bit
images.

![HDR Generation](http://vcg.isti.cnr.it/piccante/img/hdr_generation.png?raw=true "HDR Generation")

Piccante can manage different color spaces, and new ones can be added to its core with ease. 

![Color Spaces](http://vcg.isti.cnr.it/piccante/img/color_spaces.png?raw=true "Color Spaces")

Piccante provides algorithms for tone mapping HDR images in order to be visualized on traditional displays.

![Tone Mapping](http://vcg.isti.cnr.it/piccante/img/tone_mapping.png?raw=true "Tone Mapping")

Piccante can filter images using a high quality selection of linear and non linear filters.

![Filtering](http://vcg.isti.cnr.it/piccante/img/filtering.png?raw=true "Filtering")

Piccante can extract local features for different tasks such as image alignments, classification, 3D reconstruction, etc.

![Local Features](http://vcg.isti.cnr.it/piccante/img/local_features.png?raw=true "Local Features")
