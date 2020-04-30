 /*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    //RGB primaries in xyY color space
    float r[] = {0.64f, 0.33f, 0.216f};
    float g[] = {0.3f, 0.6f, 0.7152f};
    float b[] = {0.15f, 0.06f, 0.0722f};

    //conversion of the primaries from xyY to XYZ
    float rXYZ[3];
    float gXYZ[3];
    float bXYZ[3];
    pic::ColorConvXYZtoxyY conv;
    conv.inverse(r, rXYZ);
    conv.inverse(g, gXYZ);
    conv.inverse(b, bXYZ);

    //compute the conversion matrix from XYZ to RGB
    float *mtx = pic::createMatrixFromPrimaries(rXYZ, gXYZ, bXYZ, NULL, NULL);

    printf("XYZ to RGB matrix:\n");
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            printf("%f ", mtx[i * 3 + j]);
        }
        printf("\n");
    }

    return 0;
}
