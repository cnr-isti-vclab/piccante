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

#ifndef PIC_NELDER_MEAD_OPT_TEST_FUNCTION_HPP
#define PIC_NELDER_MEAD_OPT_TEST_FUNCTION_HPP


#include <random>

#include "util/nelder_mead_opt_base.hpp"

namespace pic {

class NelderMeadOptTestFunction: public NelderMeadOptBase<float>
{
public:

    float a, b;

    NelderMeadOptTestFunction(float a, float b) : NelderMeadOptBase()
    {
        this->a = a;
        this->b = b;
    }

    float function(float *x, unsigned int n)
    {
        if(n != 2) {
            return FLT_MAX;
        }

        float a_x = (a - x[0]);
        float y_x2 = x[1] - x[0] * x[0];

        return a_x * a_x + b * y_x2 *y_x2;
    }

    static void test()
    {
        NelderMeadOptTestFunction test(1.0f, 100.0f);

        std::mt19937 rnd(0);
        float start[2];
        for(int i = 0; i < 1000; i++) {

            start[0] = float(rnd()%1000) * 0.003f;
            start[1] = float(rnd()%1000) * 0.002f;

            float *sol = test.run(start, 2, 1e-12f, 10000);
            printf("x: %f y: %f f: %f\n", sol[0], sol[1], test.function(sol, 2));
        }
    }
};

}

#endif // PIC_NELDER_MEAD_OPT_TEST_FUNCTION_HPP
