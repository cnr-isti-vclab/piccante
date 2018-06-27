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

#ifndef PIC_UTIL_POLYNOMIAL_HPP
#define PIC_UTIL_POLYNOMIAL_HPP

#include <vector>

#include "../base.hpp"

#ifndef PIC_DISABLE_EIGEN
    #ifndef PIC_EIGEN_NOT_BUNDLED
        #include "../externals/Eigen/QR"
    #elif
        #include <Eigen/QR>
    #endif
#endif

namespace pic {

/**
 * @brief polynomialVal
 * @param poly
 * @param x
 * @return
 */
PIC_INLINE float polynomialVal(std::vector< float > & poly, float x)
{
    float val = 0.f;
    float M = 1.f;
    for (const float &c : poly) {
        val += c * M;
        M *= x;
    }
    return val;
}

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief polynomialFit
 * @param x
 * @param y
 * @param n is the degree of the polynomial
 * @return
 */
PIC_INLINE std::vector<float> polynomialFit(std::vector<float> &x, std::vector<float> &y, int n)
{
    std::vector<float> poly;

    if(n < 1) {
        return poly;
    }

    if(x.size() != y.size()) {
        return poly;
    }

    int np1 = n + 1;

    int s = int(x.size());
    Eigen::MatrixXf A(s, np1);
    Eigen::VectorXf b(s);

    for(int i = 0; i < s; i++) {
        b(i) = y[i];
        A(i, n) = 1.0f;
    }

    for(int j = (n - 1); j >= 0; j--) {
        for(int i = 0; i < s; i++) {
            A(i, j) = x[i] * A(i, j + 1);
        }
    }

    Eigen::VectorXf _x = A.colPivHouseholderQr().solve(b);

    for(int i = n; i >= 0; i--) {
        poly.push_back(_x(i));
    }

    return poly;
}

/**
 * @brief polynomialTest
 */
PIC_INLINE void polynomialTest()
{
    std::vector<float> x, y;

    for(int i = 0; i < 10; i++) {
        float p_x = float(i);
        float p_y = 3.0f * p_x * p_x +
                    2.0f * p_x +
                    1.0f;

        x.push_back(p_x);
        y.push_back(p_y);
    }

    std::vector< float > tmp = polynomialFit(x, y, 2);

    printf("poly: ");
    for(int i = 0; i < 3; i++) {
        printf("%f ", tmp[i]);
    }

    float y_2 = polynomialVal(tmp, 4.0f);
    printf("\n p(4.0f) = %f\n", y_2);
}

#endif

} // end namespace pic

#endif //PIC_UTIL_POLYNOMIAL_HPP
