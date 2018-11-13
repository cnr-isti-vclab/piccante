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
    //check fit
    printf("Polynomial fit test:\n");
    std::vector<float> x, y;

    for(int i = 0; i < 10; i++) {
        float p_x = float(i);
        float p_y = 3.0f * p_x * p_x + 2.0f * p_x + 1.0f;
        x.push_back(p_x);
        y.push_back(p_y);
    }

    pic::Polynomial poly;
    poly.fit(x, y, 2);

    poly.print();

    printf("p(4.0f) = %f\n", poly.eval(4.0f));

    float roots[2];
    bool bReal = poly.getAllRoots(roots);

    if(bReal) {
        printf("Roots: %f %f\n", roots[0], roots[1]);
    } else {
        printf("No Real roots!\n");
    }

    printf("\n\n");

    printf("Second order polynomial test:\n");
    float tmp[] = {1.0f, -3.0f, 0.5f};
    pic::Polynomial poly2(tmp, 3);
    poly2.print();

    printf("p(0.0f) = %f\n", poly2.eval(0.0f));
    printf("dp(1.0f) = %f\n", poly2.dEval(1.0f));

    bReal = poly2.getRoots(roots);

    if(bReal) {
        printf("Roots: %f %f\n", roots[0], roots[1]);
    } else {
        printf("No Real roots!\n");
    }

    printf("\n\n");

    printf("Third order polynomial test:\n");
    float tmp3[] = {-6.0f, 11.0f, -6.0f, 1.0f};
    pic::Polynomial poly3(tmp3, 4);
    poly3.print();

    float r;
    auto poly3_2 = poly3.horner(3.0f, r);    
    printf("H: %s R: %f\n", poly3_2.toString().c_str(), r);;

    printf("p(1.0f) = %f\n", poly3.eval(1.0f));
    printf("dp(1.0f) = %f\n", poly3.dEval(1.0f));

    float roots3[3];
    bReal = poly3.getRoots(roots3);

    if(bReal) {
        printf("Roots: %f %f %f\n", roots3[0], roots3[1], roots3[2]);
    } else {
        printf("No Real roots!\n");
    }

    return 0;
}
