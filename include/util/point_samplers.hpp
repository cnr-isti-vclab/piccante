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

#ifndef PIC_UTIL_POINT_SAMPLERS_HPP
#define PIC_UTIL_POINT_SAMPLERS_HPP

namespace pic {

/**randU: random number in [0,1[*/

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

