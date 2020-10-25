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
#include <vector>

#include "../base.hpp"
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
        Polynomial poly(x, n);

        float err = 0.0f;
        for(uint i = 0; i < px.size(); i++) {
            float py_i = poly.eval(px[i]);
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
            float ty = tx + (getRandom(m()) * 0.01f - 0.05f); //noise
            float ty_sq = ty * ty;
            x.push_back(tx);
            y.push_back(ty_sq);
        }

        NelderMeadOptPositivePolynomial test(x, y);

        Polynomial poly;
        poly.fit(x, y, 2);

        float *in = poly.getArray(NULL);

        float *out = test.run(in, 3, 1e-12f, 100000);

        printf("In: [%f %f %f]\nOut: [%f %f %f]\n", in[2], in[0], in[1], out[2], out[1], out[0]);
    }

#endif
};

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_POSITIVE_POLYNOMIAL_HPP
