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

#ifndef PIC_NELDER_MEAD_OPT_BASE_HPP
#define PIC_NELDER_MEAD_OPT_BASE_HPP

#include <vector>
#include <utility>
#include <algorithm>

#include "../base.hpp"

namespace pic{

/**
 * @brief The NelderMeadOptBase class
 */
template <class Scalar>
class NelderMeadOptBase
{
protected:
    bool bStopMean;
    Scalar delta, delta_zero;
    Scalar alpha, gamma, lambda, sigma;

    //simlex vertices
    std::vector< std::pair<Scalar, Scalar *> > simplex;

    /**
     * @brief InitSimplex
     * @param x0
     * @param n
     */
    void InitSimplex(Scalar *x0, uint n)
    {
        simplex.clear();

        //first point of simplex is x0
        Scalar *vertex_0 = new Scalar[n];
        Scalar function_vertex_0;
        memcpy(vertex_0, x0, sizeof(Scalar) * n);
        function_vertex_0 = function(vertex_0, n);

        simplex.push_back(std::make_pair(function_vertex_0, vertex_0));

        //compute the other vertices of the simplex
        for(uint i = 0; i < n; i++) {
            Scalar *vertex = new Scalar[n];
            memcpy(vertex, x0, sizeof(Scalar) * n);

            if(vertex[i] != Scalar(0)) {
                vertex[i] += x0[i] * delta;
            } else {
                vertex[i] = delta_zero;
            }

            Scalar function_vertex = function(vertex, n);

            simplex.push_back(std::make_pair(function_vertex, vertex));
        }

        std::sort(simplex.begin(), simplex.end());
    }

    /**
     * @brief ComputeMean
     * @param x_mean
     * @param n
     */
    void ComputeMean(Scalar *x_mean, uint n)
    {
        Scalar n_f = Scalar(n);

        //computing the mean point in the simplex
        for(uint i = 0; i < n; i++) {
            x_mean[i] = Scalar(0);
        }

        for(uint j = 0; j < n; j++) {
            Scalar *vertex = simplex[j].second;

            for(uint i = 0; i < n; i++) {
                x_mean[i] += vertex[i];
            }
        }

        for(uint i = 0; i < n; i++) {
            x_mean[i] /= n_f;
        }
    }

    /**
     * @brief ComputeReflected
     * @param x_r
     * @param x_mean
     * @param n
     * @return
     */
    Scalar ComputeReflected(Scalar *x_r, Scalar *x_mean, uint n)
    {
        Scalar *x_n = simplex[n].second;

        for(uint i = 0; i < n; i++) {
            x_r[i] = x_mean[i] + alpha * (x_mean[i] - x_n[i]);
        }

        return function(x_r, n);
    }

    /**
     * @brief ComputeExpansion
     * @param x_e
     * @param x_mean
     * @param n
     * @return
     */
    Scalar ComputeExpansion(Scalar *x_e, Scalar *x_mean, uint n)
    {
        Scalar *x_n = simplex[n].second;

        for(uint i = 0; i < n; i++) {
            x_e[i] = x_mean[i] + gamma * (x_mean[i] - x_n[i]);
        }

        return function(x_e, n);
    }

    /**
     * @brief ComputeContractionInside
     * @param x_c
     * @param x_mean
     * @param n
     * @return
     */
    Scalar ComputeContractionInside(Scalar *x_c, Scalar *x_mean, uint n)
    {
        Scalar *x_n = simplex[n].second;

        for(uint i = 0; i < n; i++) {
            x_c[i] = x_mean[i] + lambda * (x_mean[i] - x_n[i]);
        }

        return function(x_c, n);
    }

    /**
     * @brief ComputeReduction
     * @param n
     */
    void ComputeReduction(uint n)
    {
        Scalar *x_0 = simplex[0].second;

        for(uint i = 1; i < (n + 1); i++) {

            Scalar *x_i = simplex[i].second;

            for(uint j = 0; j < n ; j++) {
                x_i[j] = x_0[j] + sigma * (x_i[j] - x_0[j]);
            }

            simplex[i].first = function(x_i, n);
        }
    }

    /**
     * @brief run_aux
     * @param x_start
     * @param n
     * @param epsilon
     * @param x
     * @return
     */
    Scalar *run_aux(Scalar *x_start, uint n, Scalar epsilon, int max_iterations = 1000, Scalar *x = NULL)
    {
        InitSimplex(x_start, n);

        Scalar *x_mean = new Scalar[n];
        Scalar *x_r = new Scalar[n];
        Scalar *x_e = new Scalar[n];
        Scalar *x_c = new Scalar[n];

        int i = 0;

        size_t n_size = sizeof(Scalar) * n;

        while(i < max_iterations) {

            std::sort(simplex.begin(), simplex.end());

            ComputeMean(x_mean, n);

            //Stopping check
            if(bStopMean) {

                Scalar function_vertex_x_mean = function(x_mean, n);

                Scalar err = Scalar(0);
                for(uint j = 0; j < n; j++) {
                    Scalar tmp = simplex[j].first - function_vertex_x_mean;
                    err += tmp * tmp;
                }

                err = sqrt( err / Scalar(n));

                if(err <= epsilon) {
                    break;
                }

            } else {

                Scalar err_f = Scalar(0);
                Scalar err_v = Scalar(0);

                for(uint j = 1; j < (n + 1); j++) {
                    err_f = MAX(err_f, fabs(simplex[j].first - simplex[0].first));

                    for(uint i = 0; i < n; i++) {
                        err_v = MAX(err_v, fabs(simplex[j].second[i] - simplex[0].second[i]));
                    }
                }

                if((err_f <= epsilon) && (err_v <= epsilon)) {
                    break;
                }
            }

            //main algorithm
            Scalar function_vertex_r = ComputeReflected(x_r, x_mean, n);

            if((simplex[0].first <= function_vertex_r) &&
               (function_vertex_r < simplex[n - 1].first)) {//reflection

                simplex[n].first = function_vertex_r;
                memcpy(simplex[n].second, x_r, n_size);
            } else {
                if(function_vertex_r < simplex[0].first) {//expansion
                    Scalar function_vertex_e = ComputeExpansion(x_e, x_mean, n);

                    if(function_vertex_e < function_vertex_r) {
                        simplex[n].first = function_vertex_e;
                        memcpy(simplex[n].second, x_e, n_size);

                    } else {
                        simplex[n].first = function_vertex_r;
                        memcpy(simplex[n].second, x_r, n_size);
                    }
                } else {//contraction function_vertex_r > function_vertex_(n - 1)

                    Scalar function_vertex_c = ComputeContractionInside(x_c, x_mean, n);

                    if(function_vertex_c < simplex[n].first) {
                        simplex[n].first = function_vertex_c;
                        memcpy(simplex[n].second, x_c, n_size);

                    } else {//reduction
                        ComputeReduction(n);
                    }
                }
            }

            i++;
        }

        #ifdef PIC_DEBUG
            printf("\nNelder-Mead iterations: %d Err: %f\n", i, simplex[0].first);
        #endif

        output_error = simplex[0].first;

        memcpy(x, simplex[0].second, n_size);

        delete[] x_mean;
        delete[] x_r;
        delete[] x_e;
        delete[] x_c;

        return x;
    }

public:

    int max_iterations;
    Scalar output_error;

    /**
     * @brief NelderMeadOptBase
     */
    NelderMeadOptBase()
    {
        GlobalSettings();
    }

    /**
     * @brief GlobalSettings
     */
    void GlobalSettings()
    {
        //initialization
        delta = Scalar(0.05);
        delta_zero = Scalar(0.00025);

        bStopMean = false;
        
        //other parts
        alpha = Scalar(1.0);
        gamma = Scalar(2.0);
        lambda = Scalar(-0.5);
        sigma = Scalar(0.5);
    }

    /**
     * @brief function
     * @return
     */
    virtual Scalar function(Scalar *x, uint n)
    {
        return FLT_MAX;
    }

    virtual Scalar *run(Scalar *x_start, uint n, Scalar epsilon = 1e-4f, int max_iterations = 1000, Scalar *x = NULL)
    {
        if(x == NULL) {
            x = new Scalar[n];
        }

        return run_aux(x_start, n, epsilon, max_iterations, x);
    }

};

}

#endif // PIC_NELDER_MEAD_OPT_BASE_HPP
