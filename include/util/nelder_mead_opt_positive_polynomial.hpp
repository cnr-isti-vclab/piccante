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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_POSITIVE_POLYNOMIAL_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_POSITIVE_POLYNOMIAL_HPP

#include <random>

#include "../util/nelder_mead_opt_base.hpp"
#include "../util/math.hpp"
#include "../util/polynomial.hpp"

namespace pic {

class NelderMeadOptPositivePolynomial: public NelderMeadOptBase<float>
{
protected:
    std::vector< float > px, py;

public:
    NelderMeadOptPositivePolynomial(std::vector< float > &px, std::vector< float > &py) : NelderMeadOptBase()
    {
        if(px.size() == py.size()) {
            this->px.assign(px.begin(), px.end());
            this->py.assign(py.begin(), py.end());
        }
    }

    float function(float *x, unsigned int n)
    {
        std::vector< float > poly;
        poly.assign(x, x + n);

        float err = 0.0f;
        for(auto i = 0; i < px.size(); i++) {
            float py_i = polynomialVal(poly, px[i]);
            if(py_i > 0.0f) {
                float delta_y = py_i - py[i];
                err += (delta_y * delta_y);
            } else {
                err += 1e6f;
            }
        }

        return err;
    }

#ifndef PIC_DISABLE_EIGEN

    static void test()
    {
        std::mt19937 m(1);

        std::vector< float > x, y;

        for(int i = 0; i < 100; i++) {
            float tx = float(i) / 100.0f;
            float ty = tx + (Random(m()) * 0.1f - 0.5f);
            x.push_back(tx);
            y.push_back(ty);
        }

        NelderMeadOptPositivePolynomial test(x, y);

        std::vector<float> poly = polynomialFit(x, y, 2);

        float *in = new float[poly.size()];
        for(int i = 0; i < poly.size(); i++) {
            in[i] = poly[i];
        }

        float *out = test.run(in, 3, 1e-12f, 10000);

        printf("Init: %f %f %f\nOut: %f %f %f\n", in[0], in[1], in[2], out[0], out[1], out[2]);
    }

#endif
};

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_POSITIVE_POLYNOMIAL_HPP
