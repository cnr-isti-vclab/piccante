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
};

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_POSITIVE_POLYNOMIAL_HPP
