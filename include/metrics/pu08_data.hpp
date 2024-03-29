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

#ifndef PIC_METRICS_PU_ENCODE_DATA_HPP
#define PIC_METRICS_PU_ENCODE_DATA_HPP

#include <math.h>

#include "../base.hpp"
#include "../image.hpp"
#include "../util/array.hpp"

namespace pic {

PIC_INLINE float PU08_x[256] = {
    -5.000000f,
    -4.882784f,
    -4.765568f,
    -4.706960f,
    -4.648352f,
    -4.589744f,
    -4.531136f,
    -4.472527f,
    -4.413919f,
    -4.355311f,
    -4.296703f,
    -4.238095f,
    -4.179487f,
    -4.120879f,
    -4.062271f,
    -4.003663f,
    -3.945055f,
    -3.886447f,
    -3.827839f,
    -3.769231f,
    -3.710623f,
    -3.652015f,
    -3.593407f,
    -3.534799f,
    -3.476190f,
    -3.417582f,
    -3.358974f,
    -3.300366f,
    -3.241758f,
    -3.183150f,
    -3.124542f,
    -3.065934f,
    -3.007326f,
    -2.948718f,
    -2.890110f,
    -2.831502f,
    -2.772894f,
    -2.743590f,
    -2.714286f,
    -2.684982f,
    -2.655678f,
    -2.626374f,
    -2.597070f,
    -2.567766f,
    -2.538462f,
    -2.509158f,
    -2.479853f,
    -2.450549f,
    -2.421245f,
    -2.391941f,
    -2.362637f,
    -2.333333f,
    -2.304029f,
    -2.274725f,
    -2.245421f,
    -2.216117f,
    -2.186813f,
    -2.157509f,
    -2.128205f,
    -2.098901f,
    -2.069597f,
    -2.040293f,
    -2.010989f,
    -1.981685f,
    -1.952381f,
    -1.923077f,
    -1.893773f,
    -1.864469f,
    -1.835165f,
    -1.805861f,
    -1.776557f,
    -1.747253f,
    -1.717949f,
    -1.688645f,
    -1.659341f,
    -1.630037f,
    -1.600733f,
    -1.571429f,
    -1.542125f,
    -1.512821f,
    -1.483516f,
    -1.454212f,
    -1.424908f,
    -1.395604f,
    -1.366300f,
    -1.336996f,
    -1.307692f,
    -1.278388f,
    -1.249084f,
    -1.219780f,
    -1.190476f,
    -1.161172f,
    -1.131868f,
    -1.102564f,
    -1.073260f,
    -1.043956f,
    -1.014652f,
    -0.985348f,
    -0.956044f,
    -0.926740f,
    -0.897436f,
    -0.868132f,
    -0.838828f,
    -0.809524f,
    -0.780220f,
    -0.750916f,
    -0.736264f,
    -0.721612f,
    -0.706960f,
    -0.692308f,
    -0.677656f,
    -0.663004f,
    -0.648352f,
    -0.633700f,
    -0.619048f,
    -0.604396f,
    -0.589744f,
    -0.575092f,
    -0.560440f,
    -0.545788f,
    -0.531136f,
    -0.516484f,
    -0.501832f,
    -0.487179f,
    -0.472527f,
    -0.457875f,
    -0.443223f,
    -0.428571f,
    -0.413919f,
    -0.399267f,
    -0.384615f,
    -0.369963f,
    -0.355311f,
    -0.340659f,
    -0.326007f,
    -0.311355f,
    -0.296703f,
    -0.282051f,
    -0.267399f,
    -0.252747f,
    -0.238095f,
    -0.223443f,
    -0.208791f,
    -0.194139f,
    -0.179487f,
    -0.164835f,
    -0.150183f,
    -0.135531f,
    -0.120879f,
    -0.106227f,
    -0.091575f,
    -0.076923f,
    -0.062271f,
    -0.047619f,
    -0.032967f,
    -0.018315f,
    -0.003663f,
    0.010989f,
    0.025641f,
    0.040293f,
    0.054945f,
    0.069597f,
    0.084249f,
    0.098901f,
    0.113553f,
    0.128205f,
    0.142857f,
    0.157509f,
    0.172161f,
    0.186813f,
    0.201465f,
    0.216117f,
    0.230769f,
    0.245421f,
    0.267399f,
    0.285714f,
    0.300366f,
    0.315018f,
    0.329670f,
    0.344322f,
    0.358974f,
    0.373626f,
    0.388278f,
    0.402930f,
    0.417582f,
    0.432234f,
    0.446886f,
    0.461538f,
    0.476190f,
    0.490842f,
    0.505495f,
    0.520147f,
    0.534799f,
    0.549451f,
    0.564103f,
    0.578755f,
    0.593407f,
    0.608059f,
    0.622711f,
    0.637363f,
    0.652015f,
    0.666667f,
    0.681319f,
    0.695971f,
    0.710623f,
    0.725275f,
    0.739927f,
    0.754579f,
    0.769231f,
    0.783883f,
    0.798535f,
    0.813187f,
    0.827839f,
    0.842491f,
    0.857143f,
    0.871795f,
    0.886447f,
    0.915751f,
    0.945055f,
    0.974359f,
    1.003663f,
    1.032967f,
    1.062271f,
    1.091575f,
    1.120879f,
    1.150183f,
    1.179487f,
    1.208791f,
    1.238095f,
    1.267399f,
    1.296703f,
    1.326007f,
    1.355311f,
    1.384615f,
    1.413919f,
    1.443223f,
    1.472527f,
    1.501832f,
    1.531136f,
    1.589744f,
    1.648352f,
    1.706960f,
    1.765568f,
    1.824176f,
    1.882784f,
    1.930403f,
    2.007326f,
    2.095238f,
    2.186813f,
    2.307692f,
    2.399267f,
    2.567766f,
    2.725275f,
    2.871795f,
    3.260073f,
    10.000000f
};

PIC_INLINE float PU08_y[256] = {
    0.000000f,
    0.025875f,
    0.055091f,
    0.071086f,
    0.088082f,
    0.106142f,
    0.125333f,
    0.145726f,
    0.167397f,
    0.190424f,
    0.214893f,
    0.240894f,
    0.268524f,
    0.297884f,
    0.329082f,
    0.362234f,
    0.397462f,
    0.434897f,
    0.474675f,
    0.516944f,
    0.561860f,
    0.609589f,
    0.660307f,
    0.714201f,
    0.771470f,
    0.832325f,
    0.896991f,
    0.965706f,
    1.038724f,
    1.116315f,
    1.198765f,
    1.286377f,
    1.379476f,
    1.478406f,
    1.583530f,
    1.695237f,
    1.813939f,
    1.876050f,
    1.940075f,
    2.006075f,
    2.074110f,
    2.144242f,
    2.216537f,
    2.291062f,
    2.367884f,
    2.447076f,
    2.528709f,
    2.612859f,
    2.699604f,
    2.789024f,
    2.881201f,
    2.976220f,
    3.074169f,
    3.175139f,
    3.279222f,
    3.386514f,
    3.497114f,
    3.611125f,
    3.728651f,
    3.849802f,
    3.974687f,
    4.103424f,
    4.236130f,
    4.372927f,
    4.513943f,
    4.659307f,
    4.809153f,
    4.963618f,
    5.122847f,
    5.286985f,
    5.456184f,
    5.630599f,
    5.810392f,
    5.995728f,
    6.186778f,
    6.383718f,
    6.586730f,
    6.796000f,
    7.011722f,
    7.234093f,
    7.463319f,
    7.699611f,
    7.943187f,
    8.194270f,
    8.453091f,
    8.719889f,
    8.994909f,
    9.278403f,
    9.570633f,
    9.871866f,
    10.182379f,
    10.502456f,
    10.832392f,
    11.172488f,
    11.523055f,
    11.884415f,
    12.256896f,
    12.640840f,
    13.036596f,
    13.444523f,
    13.864993f,
    14.298387f,
    14.745097f,
    15.205526f,
    15.680088f,
    16.169211f,
    16.419369f,
    16.673331f,
    16.931156f,
    17.192899f,
    17.458619f,
    17.728376f,
    18.002228f,
    18.280235f,
    18.562459f,
    18.848962f,
    19.139806f,
    19.435055f,
    19.734773f,
    20.039023f,
    20.347873f,
    20.661388f,
    20.979636f,
    21.302683f,
    21.630599f,
    21.963452f,
    22.301312f,
    22.644250f,
    22.992336f,
    23.345643f,
    23.704242f,
    24.068207f,
    24.437610f,
    24.812526f,
    25.193029f,
    25.579193f,
    25.971094f,
    26.368808f,
    26.772409f,
    27.181974f,
    27.597580f,
    28.019302f,
    28.447218f,
    28.881403f,
    29.321934f,
    29.768887f,
    30.222337f,
    30.682362f,
    31.149035f,
    31.622432f,
    32.102627f,
    32.589692f,
    33.083700f,
    33.584723f,
    34.092830f,
    34.608091f,
    35.130574f,
    35.660343f,
    36.197464f,
    36.741998f,
    37.294006f,
    37.853546f,
    38.420673f,
    38.995440f,
    39.577897f,
    40.168091f,
    40.766065f,
    41.371860f,
    41.985513f,
    42.607056f,
    43.236517f,
    43.873922f,
    44.519290f,
    45.172636f,
    46.167636f,
    47.010542f,
    47.693856f,
    48.385150f,
    49.084410f,
    49.791614f,
    50.506734f,
    51.229738f,
    51.960586f,
    52.699233f,
    53.445627f,
    54.199709f,
    54.961417f,
    55.730679f,
    56.507418f,
    57.291553f,
    58.082994f,
    58.881648f,
    59.687414f,
    60.500187f,
    61.319857f,
    62.146308f,
    62.979420f,
    63.819070f,
    64.665127f,
    65.517461f,
    66.375935f,
    67.240411f,
    68.110747f,
    68.986801f,
    69.868426f,
    70.755476f,
    71.647801f,
    72.545254f,
    73.447684f,
    74.354943f,
    75.266880f,
    76.183346f,
    77.104195f,
    78.029279f,
    78.958452f,
    79.891572f,
    80.828498f,
    82.713208f,
    84.611499f,
    86.522330f,
    88.444709f,
    90.377696f,
    92.320405f,
    94.272002f,
    96.231710f,
    98.198806f,
    100.172619f,
    102.152530f,
    104.137968f,
    106.128409f,
    108.123375f,
    110.122427f,
    112.125165f,
    114.131226f,
    116.140281f,
    118.152030f,
    120.166201f,
    122.182551f,
    124.200857f,
    128.242559f,
    132.289936f,
    136.341877f,
    140.397483f,
    144.456030f,
    148.516935f,
    151.817830f,
    157.152166f,
    163.251012f,
    169.605996f,
    177.996740f,
    184.354479f,
    196.054275f,
    206.992126f,
    217.167385f,
    244.132872f,
    712.224440f
};

} // end namespace pic

#endif /* PIC_METRICS_PU_ENCODE_DATA_HPP */

