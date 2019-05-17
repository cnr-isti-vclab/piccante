/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014-2016
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_ALGORITHMS_CAMERA_RESPONSE_FUNCTION_HPP
#define PIC_ALGORITHMS_CAMERA_RESPONSE_FUNCTION_HPP

#include <algorithm>

#include "../image.hpp"
#include "../point_samplers/sampler_random.hpp"
#include "../histogram.hpp"
#include "../filtering/filter_mean.hpp"
#include "../util/polynomial.hpp"

#include "../algorithms/sub_sample_stack.hpp"
#include "../algorithms/weight_function.hpp"
#include "../algorithms/mitsunaga_nayar_crf.hpp"

#ifndef PIC_DISABLE_EIGEN
    #ifndef PIC_EIGEN_NOT_BUNDLED
        #include "../externals/Eigen/SVD"
    #else
        #include <Eigen/SVD>
    #endif
#endif

namespace pic {

enum IMG_LIN {IL_LIN, IL_2_2, IL_LUT_8_BIT, IL_POLYNOMIAL};

/**
 * @brief The CameraResponseFunction class
 */
class CameraResponseFunction
{
protected:

    /**
    * \brief gsolve computes the inverse CRF of a camera.
    */
    float *gsolve(int *samples, std::vector< float > &log_exposure, float lambda, int nSamples)
    {
        #ifndef PIC_DISABLE_EIGEN

        int nExposure = int(log_exposure.size());

        int n = 256;
        int rows = nSamples * nExposure + n + 1;
        int cols = n + nSamples;

        #ifdef PIC_DEBUG
            printf("Matrix size: (%d, %d)\n", rows, cols);
        #endif

        Eigen::MatrixXf A = Eigen::MatrixXf::Zero(rows, cols);
        Eigen::VectorXf b = Eigen::VectorXf::Zero(rows);

        int k = 0;

        for(int i = 0; i < nSamples; i++) {
            for(int j = 0; j < nExposure; j++) {
                int tmp = samples[i * nExposure + j];

                float w_ij = w[tmp];

                A.coeffRef(k, tmp) =  w_ij;
                A.coeffRef(k, n + i) = -w_ij;
                
                b[k] =  w_ij * log_exposure[j];

                k++;
            }
        }

        A.coeffRef(k, 128) = 1.0f;
        k++;

        //smoothness term
        for(int i = 0; i < (n - 2); i++) {
            float w_l = lambda * w[i + 1];
            A.coeffRef(k, i)     =         w_l;
            A.coeffRef(k, i + 1) = -2.0f * w_l;
            A.coeffRef(k, i + 2) =         w_l;
            k++;
        }

        //solve the linear system
        Eigen::JacobiSVD< Eigen::MatrixXf > svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);

        Eigen::VectorXf x = svd.solve(b);

        float *ret = new float[n];

        for(int i = 0; i < n; i++) {
            ret[i] = expf(x[i]);
        }

        #else
            float *ret = NULL;
        #endif

        return ret;
    }

    /**
     * @brief release frees memory.
     */
    void release()
    {
        stackOut.release();

        for(unsigned int i = 0; i < icrf.size(); i++) {
            if(icrf[i] != NULL) {
                delete[] icrf[i];
                icrf[i] = NULL;
            }
        }

        for(unsigned int i = 0; i < crf.size(); i++) {
            if(crf[i] != NULL) {
                delete[] crf[i];
                crf[i] = NULL;
            }
        }

        icrf.clear();
        crf.clear();
        poly.clear();
    }

    /**
     * @brief createTabledICRF
     */
    void createTabledICRF()
    {
        if(type_linearization != IL_POLYNOMIAL) {
            return;
        }

        for(unsigned int i = 0; i < icrf.size(); i++) {
            if(icrf[i] != NULL) {
                delete[] icrf[i];
            }
        }

        icrf.clear();

        for(unsigned int i = 0; i < poly.size(); i++) {
            float *tmp = new float[256];

            for(int j = 0; j < 256; j++) {
                float x = float(j) / 255.0f;

                tmp[j] = poly[i].eval(x);
            }

            crf.push_back(tmp);
        }
    }

    SubSampleStack stackOut;
    IMG_LIN type_linearization;
    float w[256];

public:

    std::vector<float *> icrf;
    std::vector<float *> crf;

    std::vector< Polynomial > poly;
    
    /**
     * @brief CameraResponseFunction
     */
    CameraResponseFunction()
    {
        type_linearization = IL_LIN;
    }

    ~CameraResponseFunction()
    {
        release();
    }

    /**
     * @brief remove linearizes a camera value using the inverse CRF.
     * @param x is an intensity value in [0,1].
     * @param channel
     * @return It returns x in the linear domain.
     */
    inline float remove(float x, int channel)
    {
        switch(type_linearization) {
            case IL_LIN: {
                return x;
            }
            break;

            case IL_LUT_8_BIT: {
                int index =  CLAMP(int(round(x * 255.0f)), 256);
                return icrf.at(channel)[index];
            }
            break;

            case IL_2_2: {
                return powf(x, 2.2f);
            }
            break;

            case IL_POLYNOMIAL: {
                return poly[channel].eval(x);
            }
            break;

            default:
                break;
        }

        return x;
    }

    /**
     * @brief apply
     * @param x a value in [0, 1]
     * @param channel
     * @return
     */
    inline float apply(float x, int channel)
    {
        switch(type_linearization) {
            case IL_LIN: {
                return x;
            }
            break;

            case IL_LUT_8_BIT: {
                float *ptr = std::lower_bound(&icrf[channel][0], &icrf[channel][255], x);
                int offset = CLAMPi((int)(ptr - icrf[channel]), 0, 255);

                return float(offset) / 255.0f;
            }
            break;

            case IL_2_2: {
               #ifdef PIC_WIN32
                  float inv_gamma = 1.0f / 2.2f;
               #else
                  constexpr float inv_gamma = 1.0f / 2.2f;
               #endif

               return powf(x, inv_gamma);
            }
            break;

            case IL_POLYNOMIAL: {
                float *ptr = std::lower_bound(&icrf[channel][0], &icrf[channel][255], x);
                int offset = CLAMPi((int)(ptr - icrf[channel]), 0, 255);

                return float(offset) / 255.0f;
            }
            break;

            default:
                break;
        }

        return x;
    }

    /**
     * @brief setCRFtoGamma2_2
     */
    void setCRFtoGamma2_2()
    {
        type_linearization = IL_2_2;
    }

    /**
     * @brief setCRFtoLinear
     */
    void setCRFtoLinear()
    {
        type_linearization = IL_LIN;
    }

    /**
     * @brief FromRAWJPEG computes the CRF by exploiting the couple RAW/JPEG from cameras.
     * @param img_raw is a RAW image.
     * @param img_jpg is a JPEG compressed image.
     * @param filteringSize
     */
    void fromRAWJPEG(Image *img_raw, Image *img_jpg, int filteringSize = 11)
    {
        if((img_raw == NULL) || (img_jpg == NULL))
            return;

        if(!img_raw->isSimilarType(img_jpg))
            return;
        
        icrf.clear();

        int width    = img_raw->width;
        int height   = img_raw->height;
        int channels = img_raw->channels;

        int crf_size = 256 * 256 * channels;
        unsigned int *crf = new unsigned int[crf_size];

        for(int i=0;i<crf_size;i++) {
            crf[i] = 0;
        }
               
        for(int i=0; i<height; i++) {
            for(int j=0; j<width; j++) {

                float *data_raw = (*img_raw)(j, i);
                float *data_jpg = (*img_jpg)(j, i);               

                for(int k=0;k<channels;k++) {
                    int i_raw = CLAMPi(int(255.0f * data_raw[k]), 0, 255);
                    int i_jpg = CLAMPi(int(255.0f * data_jpg[k]), 0, 255);

                    int addr = (i_raw * 256 + i_jpg ) * channels;

                    crf[addr + k ]++;
                }
            }
        }
       
        //compute the result
        std::vector< int > coords;

        for(int k=0;k<channels;k++) {

            float *ret_c = new float[256];

            for(int j=0;j<256;j++) {
                coords.clear();

                for(int i=0;i<256;i++) {

                    int addr = (i * 256 + j ) * channels + k;

                    if(crf[addr] > 0) {
                        coords.push_back(i);                        
                    }

                }

                if(!coords.empty()) {//get the median value
                    std::sort (coords.begin(), coords.end());  
                    ret_c[j] = float(coords[coords.size() >> 1]) / 255.0f;
                }
            }
            
            if(filteringSize > 0) {
                Image toBeFiltered(1, 256, 1, 1, ret_c);

                Image *filtered = FilterMean::execute(&toBeFiltered, NULL, filteringSize);
                
                icrf.push_back(filtered->data);

            } else {
                icrf.push_back(ret_c);
            }
        }
    }

    /**
     * @brief DebevecMalik computes the CRF of a camera using multiple exposures value following Debevec and Malik
    1997's method.
     * @param stack
     * @param exposure
     * @param type
     * @param nSamples
     * @param lambda
     */
    void DebevecMalik(ImageVec stack, CRF_WEIGHT type = CW_DEB97, int nSamples = 256, float lambda = 20.0f)
    {
        release();

        if(!ImageVecCheckSimilarType(stack)) {
            return;
        }

        if(nSamples < 1) {
            nSamples = 256;
        }

        this->type_linearization = IL_LUT_8_BIT;

        //subsample the image stack
        stackOut.execute(stack, nSamples);

        int *samples = stackOut.get();
        nSamples = stackOut.getNSamples();
        
        //compute CRF using Debevec and Malik
        int channels = stack[0]->channels;

        //pre-compute the weight function
        for(int i = 0; i < 256; i++) {
            w[i] = weightFunction(float(i) / 255.0f, type);
        }

        int nExposure = int(stack.size());

        //log domain exposure time        
        std::vector< float > log_exposures;
        ImaveVecGetExposureTimesAsArray(stack, log_exposures, true);

        #ifdef PIC_DEBUG
            printf("nSamples: %d\n", nSamples);
        #endif

        int stride = nSamples * nExposure;
        for(int i = 0; i < channels; i++) {
            float *icrf_channel = gsolve(&samples[i * stride], log_exposures, lambda, nSamples);

            icrf.push_back(icrf_channel);
        }
    }

    /**
     * @brief MitsunagaNayar computes the inverse CRF of a camera as a polynomial function.
     * @param stack             Array of images with associated exposure. Note that this array will be sorted with increasing exposure.
     * @param polynomial_degree Degree of the polynomial. If negative, the best degree will be selected in [1, -polynomial_degree] for each channel.
     * @param nSamples          Number of samples to extract from each image.
     * @param full              true for computing all exposure ratios (as in book "High Dynamic Range Imaging", second edition, Reinhard et al.),
     *                          false as in the original paper (only among successive exposures).
     * @param alpha             Threshold for removing samples with values not in [alpha, 1-alpha].
     * @param computeRatios     false if exact exposures are passed, true to approximate exposure ratios as in the paper.
     * @param eps               Threshold on the difference among successive approximations for stopping the computation.
     * @param max_iterations    Stop the computation after this number of iterations.
     * @return true if successfully computed, false otherwise.
     */
    bool MitsunagaNayar(ImageVec &stack, int polynomial_degree = -3, int nSamples = 256, const bool full = false,
                        const float alpha = 0.04f, const bool computeRatios = false, const float eps = 0.0001f,
                        const std::size_t max_iterations = 100)
    {
        release();

        if(!ImageVecCheckSimilarType(stack)) {
            return false;
        }

        if(nSamples < 1) {
            nSamples = 256;
        }

        type_linearization = IL_POLYNOMIAL;

        //sort the array by exposure
        ImaveVecSortByExposureTime(stack);

        //subsample the image stack
        stackOut.execute(stack, nSamples, alpha);
        int *samples = stackOut.get();
        nSamples = stackOut.getNSamples();

        if (nSamples < 1) {
            return false;
        }

        //compute the CRF using Mitsunaga and Nayar
        int channels = stack[0]->channels;

        std::size_t nExposures = stack.size();

        std::vector< float > exposures;
        ImaveVecGetExposureTimesAsArray(stack, exposures, false);

        int stride = nSamples * int(nExposures);

        float error = std::numeric_limits<float>::infinity();
        std::vector<float> R(nExposures - 1);
        std::vector<std::vector<float>> RR(nExposures - 1, std::vector<float>(nExposures - 1));

        poly.resize(channels);

        if (polynomial_degree > 0) {
            error = 0.f;
            for (int i = 0; i < channels; ++i) {
                poly[i].coeff.assign(polynomial_degree + 1, 0.f);
                if (full) {
                    error += MitsunagaNayarFull(&samples[i * stride], nSamples, exposures, poly[i].coeff, computeRatios, RR, eps, max_iterations);
                } else {
                    error += MitsunagaNayarClassic(&samples[i * stride], nSamples, exposures, poly[i].coeff, computeRatios, R, eps, max_iterations);
                }
            }
        } else if (polynomial_degree < 0) {
            error = std::numeric_limits<float>::infinity();
            std::vector<Polynomial> tmpCoefficients(channels);
            for (int degree = 1; degree <= -polynomial_degree; ++degree) {
                float tmpError = 0.f;
                for (int i = 0; i < channels; ++i) {
                    tmpCoefficients[i].coeff.resize(degree + 1);
                    if (full) {
                        tmpError += MitsunagaNayarFull(&samples[i * stride], nSamples, exposures, tmpCoefficients[i].coeff, computeRatios, RR, eps, max_iterations);
                    } else {
                        tmpError += MitsunagaNayarClassic(&samples[i * stride], nSamples, exposures, tmpCoefficients[i].coeff, computeRatios, R, eps, max_iterations);
                    }
                }

                if (tmpError < error) {
                    error = tmpError;
                    poly = std::move(tmpCoefficients);
                    tmpCoefficients.resize(channels);
                }
            }
        }

        bool bOk = error < std::numeric_limits<float>::infinity();

        if(bOk) {
            createTabledICRF();
        }

        return bOk;
    }

    /**
     * @brief Robertson computes the CRF of a camera using all multiple exposures value Robertson et al
       1999's method (Dynamic range improvement through multiple exposures).
     * @param stack
     * @param maxIterations
     */
    void Robertson(ImageVec &stack, const size_t maxIterations = 50)
    {
        release();

        if(!ImageVecCheckSimilarType(stack)) {
            return;
        }

        this->type_linearization = IL_LUT_8_BIT;

        const int channels   = stack[0]->channels;
        const int pixelcount = stack[0]->nPixels();

        // precompute robertson weighting function
        for (size_t i=0; i<256; i++) {
            this->w[i] = weightFunction(float(i) / 255.0f, CW_ROBERTSON);
        }

        // avoid saturation
        int minM = 0;
        int maxM = 255;
        for (int m = 0; m < 256; m++) {
            if (this->w[m] > 0) {
                minM = m;
                break;
            }
        }

        for (int m=255; m>=0; m--) {
            if (this->w[m] > 0) {
                maxM = m;
                break;
            }
        }

        // avoid ghosting (for each exposure get the index for the immediately higher and lower exposure)
        int *lower = new int [stack.size()];
        int *higher = new int[stack.size()];

        for (size_t i=0; i<stack.size(); i++) {
            lower[i]  = -1;
            higher[i] = -1;
            float t = stack[i]->exposure;
            float tHigh = stack[0]->exposure;
            float tLow  = tHigh;

            for (size_t j=0; j<stack.size(); j++) {
                if (i != j) {
                    float tj = stack[j]->exposure;

                    if (tj > t && tj < tHigh) {
                        tHigh = tj;
                        higher[i] = int(j);
                    }
                    if (tj < t && tj > tLow) {
                        tLow = tj;
                        lower[i] = int(j);
                    }
                }
            }

            if (lower[i]  == -1) {
                lower[i]  = int(i);
            }

            if (higher[i] == -1) {
                higher[i] = int(i);
            }
        }

        // create initial inv response function
        {
            float * lin = new float[256];
            for (int i=0; i<256; i++) {
                lin[i] = float(2.0 * i / 255.0);
            }
            this->icrf.push_back(lin);

            for (int i=1; i<channels; i++) {
                float * col = new float[256];
                Buffer<float>::assign(col, lin, 256);
                this->icrf.push_back(col);
            }
        }

        // create quantized stack
        std::vector<unsigned char *> qstack;
        for (Image * slice : stack) {
            assert(slice->frames == 1);
            unsigned char * q = convertHDR2LDR(slice->data, NULL, slice->size(), LT_NOR);
            qstack.push_back(q);
        }

        // iterative gauss-seidel
        for (int ch=0; ch<channels; ch++) {
            float * fun = this->icrf[ch];
            float funPrev[256];
            Buffer<float>::assign(funPrev, fun, 256);

            std::vector<float> x(pixelcount);

            float prevDelta = 0.0f;
            for (size_t iter=0; iter<maxIterations; iter++) {
                // Normalize inv crf to midpoint
                {
                    // find min max
                    size_t minIdx, maxIdx;
                    for (minIdx = 0   ; minIdx < 255 && fun[minIdx]==0 ; minIdx++);
                    for (maxIdx = 255 ; maxIdx > 0   && fun[maxIdx]==0 ; maxIdx--);

                    size_t midIdx = minIdx+(maxIdx-minIdx)/2;
                    float  mid = fun[midIdx];

                    if (mid == 0.0f) {
                        // find first non-zero middle response
                        while (midIdx < maxIdx && fun[midIdx] == 0.0f) {
                            midIdx++;
                        }
                        mid = fun[midIdx];
                    }

                    if (mid != 0.0f) {
                        Buffer<float>::div(fun, 256, mid);
                    }
                }

                // Update x
                for (int i=0; i<pixelcount; i++) {
                    float sum     = 0.0f;
                    float divisor = 0.0f;

                    float maxt = -1.0f;
                    float mint = FLT_MAX;

                    int ind = i * channels + ch;

                    for (size_t s=0; s<qstack.size(); s++) {
                        unsigned char * qslice = qstack[s];
                        const float     t      = stack[s]->exposure;

                        int m = qslice[ind];

                        // compute max/min time for under/over exposed pixels
                        if (m > maxM) {
                            mint = std::min(mint, t);
                        }

                        if (m < minM) {
                            maxt = std::max(maxt, t);
                        }

                        // to avoid ghosting
                        int mLow  = qstack[lower [s]][ind];
                        int mHigh = qstack[higher[s]][ind];
                        if (mLow > m || mHigh < m) {
                            continue;
                        }

                        const float wm = this->w[m];

                        sum     += wm * t * fun[m];
                        divisor += wm * t * t;
                    }

                    if (divisor == 0.0f) {
                        // avoid saturation
                        if (maxt > -1.0f) {
                            x[i] = fun[minM] / maxt;
                        }

                        if (mint < FLT_MAX) {
                            x[i] = fun[maxM] / mint;
                        }
                    } else if (divisor < 1e-4f) {
                        x[i] = -1.0f;
                    } else {
                        x[i] = sum / divisor;
                    }
                }

                // Update inv crf
                {
                    size_t cardEm[256] = { 0 };
                    float  sum[256]    = { 0.0f };
                    float minSatTime = FLT_MAX;
                    for (size_t s=0; s<qstack.size(); s++) {
                        unsigned char * qslice = qstack[s];
                        const float     t      = stack[s]->exposure;

                        for (int i=0; i<pixelcount; i++) {
                            if (x[i] < 0.0f) {
                                continue;
                            }

                            const int m = int(qslice[i*channels+ch]);

                            if (m == 255) {
                                if (t < minSatTime) {
                                    minSatTime = t;
                                    sum[m] = t * x[i];
                                    cardEm[m] = 1;
                                }
                                else if (t == minSatTime)
                                {
                                    sum[m] = std::min(sum[m], t * x[i]);
                                }
                            } else {
                                sum[m] += t * x[i];
                                cardEm[m]++;
                            }
                        }
                    }

                    // compute average and fill undefined values with previous one
                    float prev = 0.0f;
                    for (int m=0; m<256; m++) {
                        if (cardEm[m] != 0) {
                            fun[m] = prev = sum[m] / cardEm[m];
                        } else {
                            fun[m] = prev;
                        }
                    }
                }

                // check residuals
                {
                    static const float MaxDelta = 1e-7f;

                    float delta = 0.0f;
                    int count   = 0;
                    for (int m=0; m<256; m++) {
                        if( fun[m] != 0.0f ) {
                            float diff = fun[m] - funPrev[m];
                            delta += diff * diff;
                            funPrev[m] = fun[m];
                            count++;
                        }
                    }
                    delta /= count;

                    if (delta < MaxDelta) {
                        break;
                    }

                    prevDelta = delta;
                }
            }
        }
        // estimation complete!

        // normalize response function keeping relative scale between colors
        float maxV = -1.0f;
        for (int ch=0; ch<channels; ch++) {
            int ind;
            maxV = std::max(Arrayf::getMax(this->icrf[ch], 256, ind), maxV);
        }

        for (int ch=0; ch<channels; ch++) {
            Buffer<float>::div(this->icrf[ch], 256, maxV);
            this->icrf[ch][255] = 1.0f;
        }

        // clean quantized stack
        for (unsigned char * qslice : qstack) {
            delete qslice;
        }

        delete[] lower;
        delete[] higher;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_CAMERA_RESPONSE_FUNCTION_HPP */

