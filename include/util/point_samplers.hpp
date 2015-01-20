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

#ifndef PIC_UTIL_POINT_SAMPLERS_HPP
#define PIC_UTIL_POINT_SAMPLERS_HPP

namespace pic {

/**
 * @brief randU computes a random number in [0, 1[ using the classic rand().
 * @return It returns a random value in [0, 1[ using the classic rand().
 */
inline float randU()
{
    return float(rand() % RAND_MAX) / float(RAND_MAX);
}

const float POISSON_RHO = 0.75f;

/**
 * @brief PoissonRadius estimates the radius of a Poisson-disk like distribution
 * using nSmaples.
 * @param nSamples is the number of samples to have.
 * @return It returns the estimation of the radius.
 */
inline float PoissonRadius(int nSamples)
{
    return (2.0f * POISSON_RHO) / sqrtf(2.0f * sqrtf(3.0f) * float(nSamples));
}

/*Sampler type:
	-ST_POISSON: poisson sampling
	-ST_POISSON_M: multiple poisson sampling
	-ST_MONTECARLO: classic montecarlo
	-ST_MONTECARLO_S: stratifield montecarlo*/

enum SAMPLER_TYPE {ST_BRIDSON, ST_DARTTHROWING, ST_PATTERN, ST_MONTECARLO, ST_MONTECARLO_S};

} // end namespace pic

#endif /* PIC_UTIL_POINT_SAMPLERS_HPP */

