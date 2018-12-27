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

#ifndef PIC_UTIL_coeffNOMIAL_HPP
#define PIC_UTIL_coeffNOMIAL_HPP

#include <vector>

#include "../base.hpp"

#ifndef PIC_DISABLE_EIGEN
    #ifndef PIC_EIGEN_NOT_BUNDLED
        #include "../externals/Eigen/QR"
        #include "../externals/Eigen/Eigenvalues"
    #else
        #include <Eigen/QR>
    #endif
#endif

namespace pic {


class Polynomial
{
public:
    std::vector<float> coeff;
    bool  all_coeff_positive;

    /**
     * @brief Polynomial
     */
    Polynomial()
    {
        all_coeff_positive = true;
    }

    /**
     * @brief Polynomial
     * @param nCoeff
     */
    Polynomial(int nCoeff)
    {
        for(int i = 0; i < nCoeff; i++) {
            coeff.push_back(0.0f);
        }
        all_coeff_positive = true;
    }

    /**
     * @brief coeffnomial
     * @param coeff
     * @param nCoeff
     */
    Polynomial(float *coeff, int nCoeff)
    {
        if(nCoeff < 1 || coeff == NULL) {
            return;
        }

        this->coeff.assign(coeff, coeff + nCoeff);

        computeAllCoeffPositive();
    }

    ~Polynomial()
    {

    }

    /**
     * @brief computeAllCoeffPositive
     */
    void computeAllCoeffPositive()
    {
        int counter = 0;
        for (const float &c : coeff) {
            if(c < 0.0f) {
                counter++;
            }
        }

        all_coeff_positive = counter < 1;
    }

    /**
     * @brief print
     */
    void print()
    {
        printf("%s\n", toString().c_str());
    }

    std::string toString()
    {
        std::string ret = "";
        if(coeff.empty()) {
            return ret;
        }
        int nCoeff = coeff.size();

        if(nCoeff > 1) {

            std::string sep = "+ ";
            if(coeff[1] < 0.0f) {
                sep = " ";
            }

            ret = fromNumberToString(coeff[0]) + sep;

            for(int i = 1; i < (nCoeff - 1); i++) {

                if(coeff[i + 1] > 0.0f) {
                    sep = "+ ";
                } else {
                    sep = " ";
                }

                ret += fromNumberToString(coeff[i]) + " * x^" + fromNumberToString(i) + sep;
            }

            ret += fromNumberToString(coeff[nCoeff - 1]) + " * x^" + fromNumberToString(nCoeff - 1);
        } else {
            ret = fromNumberToString(coeff[0]);
        }
        return ret;
    }

    /**
     * @brief getArray
     * @return
     */
    float *getArray(float *ret)
    {
        if(ret == NULL) {
            ret = new float[coeff.size()];
        }

        for(int i = 0; i < coeff.size(); i++) {
            ret[i] = coeff[i];
        }

        return ret;
    }

    /**
     * @brief eval
     * @param x
     * @return
     */
    float eval(float x)
    {
        float val = 0.f;
        float M = 1.f;
        for (const float &c : coeff) {
            val += c * M;
            M *= x;
        }
        return val;
    }

    /**
     * @brief dEval
     * @param t
     * @return
     */
    float dEval(float x)
    {
        int nCoeff = coeff.size();

        if(nCoeff < 2) {
            return 0.0f;
        }

        float ret = coeff[nCoeff - 1] * float(nCoeff - 1);

        for(int i = (nCoeff - 2); i > 0 ; i--) {
            ret = (ret * x) + coeff[i] * float(i);
        }
        return ret;
    }

    /**
     * @brief fit
     * @param x
     * @param y
     * @param n
     */
    void fit(std::vector<float> &x, std::vector<float> &y, int n)
    {
#ifndef PIC_DISABLE_EIGEN
        if(n < 1 || (x.size() != y.size())) {
            return;
        }

        coeff.clear();

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
            coeff.push_back(_x(i));
        }
#endif
    }

    /**
     * @brief normalForm
     */
    void normalForm()
    {
        int last = coeff.size() - 1;

        if(fabsf(coeff[last]) > 0.0f) {

            for(int i = 0; i < last; i++) {
                coeff[i] /= coeff[last];
            }

            coeff[last] = 1.0f;
        }
    }

    /**
     * @brief horner
     * @param d
     * @param remainder
     * @return
     */
    Polynomial horner(float d, float &remainder)
    {
        int nCoeff = coeff.size();
        Polynomial p(nCoeff - 1);

        p.coeff[nCoeff - 2] = coeff[nCoeff - 1];

        for(int i = (nCoeff - 3); i >= 0 ; i--) {
            p.coeff[i] = (p.coeff[i + 1] * d + coeff[i + 1]);
        }

        p.computeAllCoeffPositive();

        remainder = coeff[0] + p.coeff[0] * d;

        return p;
    }

    /**
     * @brief getRoots
     * @param x
     * @return
     */
    bool getRoots(float *x)
    {
        int nCoeff = coeff.size();

        if(nCoeff < 2) {
            return false;
        }

        if(nCoeff == 2) {
            //this coefficient may be not positive
            if(coeff[1] > 0.0f) {
                x[0] = -coeff[0] / coeff[1];
                return true;
            } else {
                return false;
            }
        }

        if(nCoeff == 3) {
            //these coefficients may be not positive
            return getSecondOrderRoots(coeff[2], coeff[1], coeff[0], &x[0], &x[1]);
        }

        if(all_coeff_positive) {
            return false;
        }

        float max_coeff0 = -1.0f;

        for(int i = 1; i < coeff.size(); i++) {
            float tmp = fabsf(coeff[i]);
            if(tmp > max_coeff0) {
                max_coeff0 = tmp;
            }
        }

        float tmp = fabsf(coeff[0]);
        float max_coeff;
        if(max_coeff0 < tmp) {
            max_coeff = tmp;
        } else {
            max_coeff = max_coeff0;
        }

        float lower_bound = fabsf(coeff[0]) / (fabsf(coeff[0]) + max_coeff0);

#ifdef PIC_DEBUG
        float upper_bound = 1.0f + max_coeff / fabsf(coeff[0]);
        printf("Upper bound: %f\n", upper_bound);
        printf("Lower bound: %f\n", lower_bound);
#endif

        float x_p = lower_bound;
        float x_n;
        bool notConverged = true;
        int counter = 0;
        float E_x_p;
        while(notConverged) {
            E_x_p = eval(x_p);
            x_n = x_p - E_x_p / dEval(x_p);
            x_p = x_n;
            counter++;

            notConverged = (fabsf(E_x_p) > 1e-4f) && (counter < 200);
        }

        if(counter >= 200) {
            return false;
        } else {
            *x = x_n;
            return true;
        }
    }

    /**
     * @brief getAllRoots
     * @param x
     * @return
     */
    bool getAllRoots(float *x)
    {
        for(int i = 0; i < coeff.size() - 1; i++) {
            x[i] = FLT_MAX;
        }

        bool bOut = getRoots(&x[0]);

        if(!bOut) {
            return false;
        }

        float r;
        Polynomial p = horner(x[0], r);
        p.normalForm();

        int nCoeff = coeff.size();
        for(int i = 1; i < (nCoeff - 2); i++) {
            bool bOut = p.getRoots(&x[i]);

            if(!bOut) {
                return true;
            }

            p = p.horner(x[i], r);
            p.normalForm();
        }

        return true;
    }

    /**
     * @brief getQuarticRoots --> MANDATORY p[5] == 1.0
     * @param p
     * @param x
     * @return
     */
    static bool getQuarticRoots(float *p, float *x)
    {
#ifndef PIC_DISABLE_EIGEN
        Eigen::Matrix4f m;

        m << -p[3], -p[2], -p[1], -p[0],
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f;

        Eigen::EigenSolver<Eigen::Matrix4f> es(m);
        Eigen::Vector4cf e = es.eigenvalues();

        bool bOut = false;
        if(fabsf(e(0).imag()) < 1e-6f) {
            x[0] = e(0).real();
            bOut = true;
        }

        if(fabsf(e(1).imag()) < 1e-6f) {
            x[1] = e(1).real();
            bOut = true;
        }

        if(fabsf(e(2).imag()) < 1e-6f) {
            x[2] = e(2).real();
            bOut = true;
        }

        if(fabsf(e(3).imag()) < 1e-6f) {
            x[3] = e(3).real();
            bOut = true;
        }

        return bOut;
#else
        return true;
#endif
    }

    /**
     * @brief getSecondOrderRoots solves second order equations, ax^2 + b x + c = 0
     * @param a is the a coefficient
     * @param b is the b coefficient
     * @param c is the c coefficient
     * @param x0 is the first zero
     * @param x1 is the second zero
     * @return It returns true, if x0 and x1 have a real value, false otherwise
     */
    static bool getSecondOrderRoots(float a, float b, float c, float *x0, float *x1)
    {
        float delta = b * b - 4.0f * a * c;

        if(delta >= 0.0f) {
            float dnum = 2.0f * a;
            delta = sqrtf(delta);
            *x0 = (-b + delta) / dnum;
            *x1 = (-b - delta) / dnum;
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief getSecondOrderRootsS solves second order equations, ax^2 + 2 b x + c = 0; i.e., 2 b is even!
     * @param a is the a coefficient
     * @param b is the b coefficient
     * @param c is the c coefficient
     * @param x0 is the first zero
     * @param x1 is the second zero
     * @return It returns true, if x0 and x1 have a real value, false otherwise
     */
    static bool getSecondOrderRootsS(float a, float b, float c, float *x0, float *x1)
    {
        float delta = b * b - a * c;
        if(delta >= 0.0f) {
            delta = sqrtf(delta);
            *x0 = (-b + delta) / a;
            *x1 = (-b - delta) / a;
            return true;
        } else {
            return false;
        }
    }


};

} // end namespace pic

#endif //PIC_UTIL_coeffNOMIAL_HPP
