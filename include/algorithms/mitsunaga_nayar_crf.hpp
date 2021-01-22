/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014-2016
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
Library author: Francesco Banterle
This file author: Giorgio Marcias

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_ALGORITHMS_MITSUNAGA_NAYAR_CRF_HPP
#define PIC_ALGORITHMS_MITSUNAGA_NAYAR_CRF_HPP

#include<algorithm>
#include<limits>
#include<vector>

#include "../base.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/LU"
#else
    #include <Eigen/LU>
#endif

#endif

namespace pic {

/**
 * @brief MitsunagaNayarClassic computes the inverse CRF of a camera as a polynomial function.
 * @param samples           Sample array of size nSamples x #exposures.
 * @param nSamples          Number of samples, for each exposure.
 * @param exposures         Array of exposure timings (size: #exposures = 'Q' as in the Mitsunaga & Nayar paper).
 * @param coefficients      The output coefficients ('c' in the paper) resulting from the computation.
 * @param computeRatios     false if exact exposures are passed, true to approximate exposure ratios as in the paper.
 * @param R                 The output estimated exposure ratios, i.e. R[q1][q2] = 'R_{q1,q2}' as in the book.
 * @param eps               Threshold for stopping the approximation process.
 * @param max_iterations    Maximum number of iterations.
 * @return The error as in the paper.
 */
PIC_INLINE float MitsunagaNayarClassic(int *samples, const std::size_t nSamples, const std::vector<float> &exposures,
                                   std::vector<float> &coefficients, const bool computeRatios, std::vector<float> &R,
                                   const float eps, const std::size_t max_iterations)
{
#ifndef PIC_DISABLE_EIGEN
    float eval, val, tmp1, tmp2;
    const std::size_t Q = exposures.size();
    const std::size_t N = coefficients.size() - 1;

    const float Mmax = 1.f;

    for (float &_c : coefficients) {
        _c = 0.f;
    }

    if (!samples || Q < 2 || coefficients.size() < 2) {
        return std::numeric_limits<float>::infinity();
    }

    R.assign(Q < 2 ? 0 : Q - 1, 1.f);
    for (unsigned int q = 0; q < R.size(); ++q) {
        R[q] = exposures[q] / exposures[q+1];
    }

    //Check valid samples
    std::vector<std::vector<float>> g(nSamples, std::vector<float>(Q, 0.f));
    std::size_t P = 0;
    for (std::size_t p = 0; p < nSamples; ++p) {
        bool valid = false;
        for (std::size_t q = 0; q < Q-1; ++q) {
            if (samples[p * Q + q] >= 0 && samples[p * Q + q+1] >= 0) {
                valid = true;
                break;
            }
        }
        if (valid) {
            for (std::size_t q = 0; q < Q; ++q) {
                if (samples[p * Q + q] >= 0) {
                    g[P][q] = samples[p * Q + q] / 255.f;
                } else {
                    g[P][q] = -1.f;
                }
            }
            ++P;
        }
    }
    g.resize(P);

    if (g.empty()) {
        return std::numeric_limits<float>::infinity();
    }

    //Precompute test with exponentials
    std::vector<Eigen::VectorXf> test;
    if (computeRatios) {
        test.assign(256, Eigen::VectorXf::Zero(N+1));
        for (std::size_t i = 0; i < 256; ++i) {
            test[i][0] = 1.f;
            for (std::size_t n = 1; n <= N; ++n) {
                test[i][n] = (i / 255.f) * test[i][n-1];
            }
        }
    }

    //Precompute M with exponentials
    std::vector<std::vector<std::vector<float>>> M(P,
                                                   std::vector<std::vector<float>>(Q,
                                                                                   std::vector<float>(N+1, 0.f)));
    for (std::size_t p = 0; p < P; ++p) {
        for (std::size_t q = 0; q < Q; ++q) {
            M[p][q][0] = 1.f;
            for (std::size_t n = 1; n <= N; ++n) {
                M[p][q][n] = g[p][q] * M[p][q][n-1];
            }
        }
    }

    std::vector<std::vector<std::vector<float>>> d(P,
                                                   std::vector<std::vector<float>>(Q-1,
                                                                                   std::vector<float>(N+1, 1.f)));
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(N, N);
    Eigen::VectorXf x, b = Eigen::VectorXf::Zero(N);
    Eigen::VectorXf c(N+1), prev_c = Eigen::VectorXf::Zero(N+1);

    std::size_t iter = 0;

    do {
        //Compute d
        for (std::size_t p = 0; p < P; ++p) {
            for (std::size_t q = 0; q < Q-1; ++q) {
                if (g[p][q] >= 0.f && g[p][q+1] >= 0.f) {
                    for (std::size_t n = 0; n <= N; ++n) {
                        d[p][q][n] = M[p][q][n] - R[q] * M[p][q+1][n];
                    }
                } else {
                    d[p][q].assign(N+1, 0.f);
                }
            }
        }

        //Build the matrix A of the linear system
        A.setZero(N, N);
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < N; ++j) {
                for (std::size_t p = 0; p < P; ++p) {
                    for (std::size_t q = 0; q < Q - 1; ++q) {
                        A(i, j) += d[p][q][i] * (d[p][q][j] - d[p][q][N]);
                    }
                }
            }
        }

        //Build the vector of knowns b
        b.setZero(N);
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t p = 0; p < P; ++p) {
                for (std::size_t q = 0; q < Q - 1; ++q) {
                    b(i) -= Mmax * d[p][q][i] * d[p][q][N];
                }
            }
        }

        //Solve the linear system
        x = A.partialPivLu().solve(b);
        c << x, Mmax - x.sum();

        if (computeRatios) {
            //Evaluate approximation increment
            eval = std::numeric_limits<float>::lowest();
            for (const Eigen::VectorXf &_M : test) {
                val = std::abs((c - prev_c).dot(_M));
                if (val > eval) {
                    eval = val;
                }
            }

            //Update R
            for (std::size_t q = 0; q < Q-1; ++q) {
                R[q] = 0.f;
                tmp1 = 0.f;
                tmp2 = 0.f;
                for (std::size_t p = 0; p < P; ++p) {
                    if (g[p][q] >= 0.f && g[p][q+1] >= 0.f) {
                        for (std::size_t n = 0; n <= N; ++n) {
                            tmp1 += c[n] * M[p][q][n];
                            tmp2 += c[n] * M[p][q+1][n];
                        }
                    }
                }
                R[q] += tmp1 / tmp2;
            }

            ++iter;
        }
    } while (computeRatios && eval > eps && iter < max_iterations);

    for (std::size_t n = 0; n <= N; ++n) {
        coefficients[n] = c[n];
    }

    //Evaluate error
    eval = 0.f;
    for (std::size_t q = 0; q < Q-1; ++q) {
        for (std::size_t p = 0; p < P; ++p) {
            if (g[p][q] >= 0.f && g[p][q+1] >= 0.f) {
                val = 0.f;
                for (std::size_t n = 0; n <= N; ++n) {
                    val += coefficients[n] * (M[p][q][n] - R[q] * M[p][q+1][n]);
                }
                eval += val * val;
            }
        }
    }

    return eval;
#else
    return -1.0f;
#endif
}

/**
 * @brief MitsunagaNayarFull computes the inverse CRF of a camera as a polynomial function, using all exposure ratios.
 * @param samples           Sample array of size nSamples x #exposures.
 * @param nSamples          Number of samples, for each exposure.
 * @param exposures         Array of exposure timings (size: #exposures = 'Q' as in the Mitsunaga & Nayar paper).
 * @param coefficients      The output coefficients ('c' in the paper) resulting from the computation.
 * @param computeRatios     false if exact exposures are passed, true to approximate exposure ratios as in the paper.
 * @param R                 The output estimated exposure ratios, i.e. R[q1][q2] = 'R_{q1,q2}' as in the book.
 * @param eps               Threshold for stopping the approximation process.
 * @param max_iterations    Maximum number of iterations.
 * @return The error as in the paper.
 */
PIC_INLINE float MitsunagaNayarFull(int *samples, const std::size_t nSamples, const std::vector<float> &exposures,
                                std::vector<float> &coefficients, bool computeRatios, std::vector<std::vector<float>> &R,
                                const float eps, const std::size_t max_iterations)
{
#ifndef PIC_DISABLE_EIGEN
    float eval, val, tmp1, tmp2;
    const std::size_t Q = exposures.size();
    const std::size_t N = coefficients.size() - 1;

    const float Mmax = 1.f;

    for (float &_c : coefficients) {
        _c = 0.f;
    }
    R.assign(Q < 2 ? 0 : Q, std::vector<float>(Q < 2 ? 0 : Q, 1.f));
    for (uint q1 = 0; q1 < R.size(); ++q1) {
        for (uint q2 = 0; q2 < R[q1].size(); ++q2) {
            if (q2 == q1) {
                R[q1][q2] = 1.f;
            } else {
                R[q1][q2] = exposures[q1] / exposures[q2];
            }
        }
    }
    if (!samples || Q < 2 || coefficients.size() < 2) {
        return std::numeric_limits<float>::infinity();
    }

    //Check valid samples
    std::vector<std::vector<float>> g(nSamples, std::vector<float>(Q, 0.f));
    std::size_t P = 0;
    for (std::size_t p = 0; p < nSamples; ++p) {
        std::size_t valid = 0;
        for (std::size_t q = 0; q < Q; ++q) {
            if (samples[p * Q + q] >= 0) {
                ++valid;
            }
        }
        if (valid > 1) {
            for (std::size_t q = 0; q < Q; ++q) {
                if (samples[p * Q + q] >= 0) {
                    g[P][q] = samples[p * Q + q] / 255.f;
                } else {
                    g[P][q] = -1.f;
                }
            }
            ++P;
        }
    }
    g.resize(P);

    if (g.empty()) {
        return std::numeric_limits<float>::infinity();
    }

    //Precompute test with exponentials
    std::vector<Eigen::VectorXf> test;
    if (computeRatios) {
        test.assign(256, Eigen::VectorXf::Zero(N+1));
        for (std::size_t i = 0; i < 256; ++i) {
            test[i][0] = 1.f;
            for (std::size_t n = 1; n <= N; ++n) {
                test[i][n] = (i / 255.f) * test[i][n-1];
            }
        }
    }

    //Precompute M with exponentials
    std::vector<std::vector<std::vector<float>>> M(P,
                                                   std::vector<std::vector<float>>(Q,
                                                                                   std::vector<float>(N+1, 0.f)));
    for (std::size_t p = 0; p < P; ++p) {
        for (std::size_t q = 0; q < Q; ++q) {
            M[p][q][0] = 1.f;
            for (std::size_t n = 1; n <= N; ++n) {
                M[p][q][n] = g[p][q] * M[p][q][n-1];
            }
        }
    }

    std::vector<std::vector<std::vector<std::vector<float>>>> d(P,
                                                                std::vector<std::vector<std::vector<float>>>(Q,
                                                                            std::vector<std::vector<float>>(Q,
                                                                                        std::vector<float>(N+1, 1.f))));
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(N, N);
    Eigen::VectorXf x, b = Eigen::VectorXf::Zero(N);
    Eigen::VectorXf c(N+1), prev_c = Eigen::VectorXf::Zero(N+1);

    std::size_t iter = 0;

    do {
        //Compute d
        for (std::size_t p = 0; p < P; ++p) {
            for (std::size_t q1 = 0; q1 < Q; ++q1) {
                for (std::size_t q2 = 0; q2 < Q; ++q2) {
                    d[p][q1][q2].assign(N+1, 0.f);
                    if (q2 != q1) {
                        for (std::size_t n = 0; n <= N; ++n) {
                            if (g[p][q1] >= 0.f && g[p][q2] >= 0.f) {
                                d[p][q1][q2][n] = M[p][q1][n] - R[q1][q2] * M[p][q2][n];
                            } else {
                                d[p][q1][q2][n] = 0.f;
                            }
                        }
                    }
                }
            }
        }

        //Build the matrix A of the linear system
        A.setZero(N, N);
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < N; ++j) {
                for (std::size_t p = 0; p < P; ++p) {
                    for (std::size_t q1 = 0; q1 < Q; ++q1) {
                        for (std::size_t q2 = 0; q2 < Q; ++q2) {
                            if (q2 != q1) {
                                A(i, j) += d[p][q1][q2][i] * (d[p][q1][q2][j] - d[p][q1][q2][N]);
                            }
                        }
                    }
                }
            }
        }

        //Build the vector of knowns b
        b.setZero(N);
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t p = 0; p < P; ++p) {
                for (std::size_t q1 = 0; q1 < Q; ++q1) {
                    for (std::size_t q2 = 0; q2 < Q; ++q2) {
                        if (q2 != q1) {
                            b(i) -= Mmax * d[p][q1][q2][i] * d[p][q1][q2][N];
                        }
                    }
                }
            }
        }

        //Solve the linear system
        x = A.partialPivLu().solve(b);
        c << x, Mmax - x.sum();

        if (computeRatios) {
            //Evaluate approximation increment
            eval = std::numeric_limits<float>::lowest();
            for (const Eigen::VectorXf &_M : test) {
                val = std::abs((c - prev_c).dot(_M));
                if (val > eval) {
                    eval = val;
                }
            }

            //Update R
            for (std::size_t q1 = 0; q1 < Q; ++q1) {
                for (std::size_t q2 = 0; q2 < Q; ++q2) {
                    R[q1][q2] = 0.f;
                    tmp1 = 0.f;
                    tmp2 = 0.f;
                    for (std::size_t p = 0; p < P; ++p) {
                        if (g[p][q1] >= 0.f && g[p][q2] >= 0.f) {
                            for (std::size_t n = 0; n <= N; ++n) {
                                tmp1 += c[n] * M[p][q1][n];
                                tmp2 += c[n] * M[p][q2][n];
                            }
                        }
                    }
                    R[q1][q2] += tmp1 / tmp2;
                }
            }

            ++iter;
        }
    } while (computeRatios && eval > eps && iter < max_iterations);

    for (std::size_t n = 0; n <= N; ++n) {
        coefficients[n] = c[n];
    }

    //Evaluate error
    eval = 0.f;
    for (std::size_t q1 = 0; q1 < Q; ++q1) {
        for (std::size_t q2 = 0; q2 < Q; ++q2) {
            if (q2 != q1) {
                for (std::size_t p = 0; p < P; ++p) {
                    if (g[p][q1] >= 0.f && g[p][q2] >= 0.f) {
                        val = 0.f;
                        for (std::size_t n = 0; n <= N; ++n) {
                            val += coefficients[n] * (M[p][q1][n] - R[q1][q2] * M[p][q2][n]);
                        }
                        eval += val * val;
                    }
                }
            }
        }
    }

    return eval;
#else
    return -1.0f;
#endif
}

}

#endif // PIC_ALGORITHMS_MITSUNAGA_NAYAR_CRF_HPP
