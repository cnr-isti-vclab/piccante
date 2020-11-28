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

#ifndef PIC_HISTOGRAM_HPP
#define PIC_HISTOGRAM_HPP

#include "image.hpp"
#include "base.hpp"

#include "util/bbox.hpp"
#include "util/std_util.hpp"
#include "util/array.hpp"
#include "util/math.hpp"

namespace pic {

enum VALUE_SPACE {VS_LDR, VS_LIN, VS_LOG_2, VS_LOG_E, VS_LOG_10};

/**
 * @brief The Histogram class is a class for creating,
 * managing, loading, and saving histogram for an Image.
 */
class Histogram
{
protected:
    float *bin_c;
    float *bin_nor;
    int nBin;
    VALUE_SPACE type;
    float fMin, fMax;
    float deltaMaxMin, nBinf;
    float epsilon;

    /**
     * @brief projectDomain applies the histogram domain to x.
     * @param x is an input value.
     * @return x is converted into the histogram domain.
     */
    inline float projectDomain(float x)
    {
        switch(type) {
            case VS_LOG_2: {
                return logf(x + epsilon) * C_INV_LOG_NAT_2;
            }
            break;

            case VS_LOG_E: {
                return logf(x + epsilon);
            }
            break;

            case VS_LOG_10: {
                return log10f(x + epsilon);
            }
            break;
                
            default: {
                return x;
            } break;
        }

        return x;
    }

    /**
     * @brief unprojectDomain removes the histogram domain to x.
     * @param x is an input value.
     * @return x is converted back to its original domain.
     */
    inline float unprojectDomain(float x)
    {
        switch(type) {
            case VS_LOG_2: {
                return powf(2.0f, x) - epsilon;
            }
            break;

            case VS_LOG_E: {
                return expf(x) - epsilon;
            }
            break;

            case VS_LOG_10: {
                return powf(10.0f, x) - epsilon;
            }
            break;
                
            default: {
                return x;
            }
            break;
        }

        return x;
    }

    /**
     * @brief update
     * @param x
     */
    inline void update(float x)
    {
        float val = projectDomain(x);

        int indx = int(((val - fMin) * nBinf) / deltaMaxMin);

        #ifdef PIC_DEBUG
        if((indx >= nBin) || (indx < 0)) {
            printf("Error in Calculate %d.\n",indx);
        }
        #endif

        bin[CLAMP(indx, nBin)]++;
    }

public:
    uint *bin, *bin_work;

    /**
     * @brief Histogram is the basic constructor setting variables to defaults.
     */
    Histogram()
    {
        bin     = NULL;
        bin_nor = NULL;
        bin_c   = NULL;
        bin_work = NULL;

        nBin =  0;
        type =  VS_LIN;
        fMin = -FLT_MAX;
        fMax =  FLT_MAX;

        epsilon = 1e-6f;
    }

    /**
     * @brief Histogram is an extension of the basic constructor, where calculate
     * is called in order to populate the Histogram.
     * @param imgIn is an input image for which Histogram needs to be computed.
     * @param type is the space of computations (please see calculate()).
     * @param nBin is the number of bins of the Histogram.
     * @param channel is the color channel for which Histogram needs to be computed.
     */
    Histogram(Image *imgIn, VALUE_SPACE type, int nBin, int channel = 0)
    {
        bin     = NULL;
        bin_nor = NULL;
        bin_c   = NULL;
        bin_work = NULL;

        fMin = -FLT_MAX;
        fMax =  FLT_MAX;

        epsilon = 1e-6f;
        this->nBin = 0;

        calculate(imgIn, type, nBin, NULL, channel);
    }

    /**
    * @brief ~Histogram is the basic destructor which frees memory.
    */
    ~Histogram()
    {
        release();

        nBin = 0;
        type =  VS_LIN;
        fMin = -FLT_MAX;
        fMax =  FLT_MAX;
    }   

    /**
     * @brief release
     */
    void release()
    {
        bin = delete_vec_s(bin);
        bin_c = delete_vec_s(bin_c);
        bin_nor = delete_vec_s(bin_nor);
        bin_work = delete_vec_s(bin_work);
    }

    /**
     * @brief calculate computes the histogram of an input image. In the case
     * of LDR images, they are ssumed to be normalized; i.e. with values in [0, 1].
     * This function computes the histogram for a single color channel.
     * @param imgIn is the input image for which the histogram needs to be computed
     * @param type is the domain space for histogram computations.
     * Histogram can be computed as: VS_LDR (256 bins), VS_LIN (linear space),
     * VS_LOG_2 (logarithm 2 base), VS_LOG_E (natural logarithm space), and VS_LOG_10
     * (logarithm 10 base).
     * @param nBin is the number of bins of the Histogram to be computed. The default value
     * is 256.
     * @param box is the slice where to compute the histogram.
     * @param channel is the color channel for which the Histogram will be computed.
     */
    void calculate(Image *imgIn, VALUE_SPACE type = VS_LIN, int nBin = 256,
                   BBox *box = NULL, int channel = 0)
    {
        if((imgIn == NULL) || (channel < 0) ) {
            uniform(0.0f, 1.0f, 1, type, nBin);
            return;
        }

        if(!imgIn->isValid() || (channel >= imgIn->channels)) {
            uniform(0.0f, 1.0f, 1, type, nBin);
            return;
        }

        if(nBin < 1 || type == VS_LDR) {
            nBin = 256;
        }

        bool c1 = (nBin != this->nBin) && (bin != NULL);
        bool c2 = (bin == NULL);
        if(c1 || c2)  {
            release();

            bin = new uint[nBin];
            memset((void *)bin, 0, nBin * sizeof(uint));
        } else {
            memset((void *)bin, 0, nBin * sizeof(uint));
        }

        this->nBin = nBin;
        this->type = type;

        int size = imgIn->width * imgIn->height * imgIn->channels;
        int channels = imgIn->channels;

        //compute statistics
        fMin =  FLT_MAX;
        fMax = -FLT_MAX;

        if(box == NULL) {
            for(int i = channel; i < size; i += channels) {
                float val = imgIn->data[i];
                fMin = MIN(fMin, val);
                fMax = MAX(fMax, val);
            }
        } else {
            for(int k = box->z0; k < box->z1; k++) {
                for(int j = box->y0; j < box->y1; j++) {
                    for(int i = box->x0; i < box->x1; i++) {
                        float *tmp_data = (*imgIn)(i, j, k);
                        fMin = MIN(fMin, tmp_data[channel]);
                        fMax = MAX(fMax, tmp_data[channel]);
                    }
                }
            }
        }

        fMin = projectDomain(fMin);
        fMax = projectDomain(fMax);

        deltaMaxMin = (fMax - fMin);
        nBinf = float(nBin - 1);

        //compute the histogram
        if(box == NULL) {
            for(int i = channel; i < size; i += channels) {
                update(imgIn->data[i]);
            }
        } else {
            for(int k = box->z0; k < box->z1; k++) {
                for(int j = box->y0; j < box->y1; j++) {
                    for(int i = box->x0; i < box->x1; i++) {
                        float *tmp_data = (*imgIn)(i, j, k);
                        update(tmp_data[channel]);
                    }
                }
            }
        }
    }

    /**
     * @brief uniform
     * @param value
     * @param type
     * @param nBin
     */
    void uniform(float fMin, float fMax, uint value, VALUE_SPACE type, int nBin)
    {
        bool c1 = (nBin != this->nBin) && (bin != NULL);
        bool c2 = (bin == NULL);
        if(c1 || c2)  {
            release();

            bin = new uint[nBin];
        }

        this->nBin = nBin;
        this->type = type;

        memset((void *)bin, value, nBin * sizeof(uint));

        nBinf = float(nBin - 1);

        this->fMin = fMin;
        this->fMax = fMax;
        deltaMaxMin = (fMax - fMin);
    }

    /**
     * @brief update
     * @param fMin
     * @param fMax
     */
    void update(float fMin, float fMax)
    {        
        this->fMin = projectDomain(fMin);
        this->fMax = projectDomain(fMax);
        deltaMaxMin = (fMax - fMin);
    }

    /**
     * @brief project converts an input value in the histogram domain.
     * @param x is an input value.
     * @return x is projected in the histogram domain.
     */
    int project(float x)
    {
        if(deltaMaxMin > 0.0f) {
            float y = projectDomain(x);
            return int(((y - fMin) * nBinf) / deltaMaxMin);
        } else {
            return 0.5f;
        }
    }

    /**
     * @brief unproject converts a histogram value back to its original domain.
     * @param ind is a histogram value.
     * @return ind is converted back to its original domain.
     */
    float unproject(int ind)
    {
        float indf = float(ind);
        float y = ((indf * deltaMaxMin) / nBinf) + fMin;
        return unprojectDomain(y);
    }

    /**
     * @brief ceiling limits the maximum value of the histogram using Ward
     * algorithm.
     * @param k
     */
    void ceiling(float k)
    {
        float tolerance = float(Array<uint>::sum(bin, nBin)) * 0.025f;
        int   trimmings = 0;
        bool  bFlag = true;

        std::vector<bool> trimmed_vec;

        while((trimmings <= tolerance) && bFlag) {
            trimmings = 0;
            float T = float(Array<uint>::sum(bin, nBin));

            if(T < tolerance) {
                bFlag = false;
            } else {
                bool bTrimmed = false;
                uint ceiling = uint(T * k);

                for(int i = 0; i < nBin; i++) {
                    if(bin[i] > ceiling) {
                        trimmings += (bin[i] - ceiling);
                        bin[i] = ceiling;
                        bTrimmed = true;
                    }
                }

                trimmed_vec.push_back(bTrimmed);

            }

            int tvSize = int(trimmed_vec.size());
            if(tvSize >= 2) {
                bool b0 = !trimmed_vec[tvSize - 1];
                bool b1 = !trimmed_vec[tvSize - 2];
                if(b0 && b1) {
                    bFlag = false;
                }
            }
        }
    }

    /**
     * @brief clip clips the histogram to value.
     * @param value the maximum allowed value in the histogram.
     */
    void clip(uint value)
    {
        int redistrib = 0;
        for(int i = 0; i < nBin; i++) {
            if(bin[i] > value) {
                redistrib += bin[i] - value + 1;
                bin[i] = value;
            }
        }

        int nCount = redistrib / nBin;

        for(int i = 0; i < nCount; i++) {
            for(int j = 0; j < nBin; j++) {
                bin[j]++;
            }
        }

        int remainder = redistrib % nBin;

        for(int i =0; i < remainder; i++) {
            bin[rand() % nBin]++;
        }
    }

    /**
     * @brief cumulativef computes the cumulative Histogram.
     * @param bNormalized is a boolean value; if it is true values of
     * the Histogram will be normalized.
     * @return It returns the cumulative Histogram as a float pointer.
     */
    float *cumulativef(bool bNormalized)
    {
        getNormalized();
    
        bin_c = Arrayf::cumsum(bin_nor, nBin, bin_c);

        if(bNormalized) {
            Arrayf::div(bin_c, nBin, bin_c[nBin - 1]);
        }

        return bin_c;
    }

    /**
     * @brief getCumulativef this function returns the cumulative
     * Histogram. Note that cumulativef needs to be computed before otherwise
     * the function will return a NULL pointer.
     * @return It returns a float pointer to the cumulative Histogram.
     */
    float *getCumulativef()
    {
        return bin_c;
    }

    /**
     * @brief getfMin
     * @return
     */
    float getfMin()
    {
        return fMin;
    }


    /**
     * @brief getfMax
     * @return
     */
    float getfMax()
    {
        return fMax;
    }

    /**
     * @brief getNormalized normalizes the Histogram.
     * @return It returns the normalized Histogram as a float pointer.
     */
    float *getNormalized(bool bNor = true)
    {
        if(bin_nor == NULL) {
            bin_nor = new float[nBin];
        }

        int ind;
        float maxValf;
        if(bNor) {
            maxValf = float(Array<uint>::getMax(bin, nBin, ind));
        } else {
            maxValf = float(Array<uint>::sum(bin, nBin));
        }

        if(maxValf > 0.0f) {
            for(int i = 0; i < nBin; i++) {
                bin_nor[i] = float(bin[i]) / maxValf;
            }
        } else {
            Arrayf::assign(0.0f, bin_nor, nBin);
        }

        return bin_nor;
    }

    /**
     * @brief getOtsu
     * @return
     */
    float getOtsu()
    {
        float *pdf = getNormalized(false);

        float w0 = 0.0f;
        float w1 = 1.0f;
        float mu0 = 0.0f;
        float mu1 = 0.0f;
        int index = 0;
        for(int i = 0; i < nBin; i++) {
            mu1 += unproject(i) * pdf[i];
        }

        float sigma_b_max = 0.0f;
        for(int i = 1; i < nBin; i++) {

            w0 += pdf[i];
            w1 -= pdf[i];

            float tmp = unproject(i) * pdf[i];
            mu0 += tmp;
            mu1 -= tmp;

            if(w0 > 0.0f && w1 > 0.0f) {
                float delta = (mu0 / w0) - (mu1 / w1);
                float sigma_b = w0 * w1 * delta * delta;

                if(sigma_b > sigma_b_max) {
                    sigma_b_max = sigma_b;
                    index = i;
                }
            }
        }

        return unproject(index);
    }

    /**
     * @brief write saves the Histogram as an Image into a file.
     * @param name is the filename where to save the Histogram.
     * @param bNor is a boolean value for normalizing or not the Histogram.
     */
    void write(std::string name, bool bNor)
    {
        Image img(1, nBin, 1, 1);

        if(bNor) {
            getNormalized();
            memcpy(img.data, bin_nor, sizeof(float) * nBin);
        } else {
            for(int i = 0; i < nBin; i++) {
                img.data[i] = float(bin[i]);
            }
        }

        img.Write(name, LT_NONE);
    }

    /**
     * @brief exposureCovering computes the exposure values for fully covering
     * the dynamic range of the image. This function works only if the histogram
     * was compute usign VS_LOG_2.
     * @param nBits is the number of bit used for storing each output exposure image.
     * The default value is 8.
     * @param overlap is the value, in f-stops, of overlapping between two exposure images.
     * This value is set to 1 by default.
     * @return It returns an std::vector<float> which contains the exposure values
     * in f-stops for all required exposures for covering information.
     */
    std::vector< float > exposureCovering(int nBits = 8, float overlap = 1.0f)
    {
        std::vector< float > ret;

        if(type != VS_LOG_2) {
            #ifdef PIC_DEBUG
                printf("ERROR in ExposureCovering: this histogram has to be in log2!\n");
            #endif
            
            return ret;
        }

        float dMM = deltaMaxMin / nBinf;

        int removingBins = int(float(nBits) /dMM + overlap);

        if( bin_work == NULL) {
            bin_work = new uint [nBin];
        }

        memcpy(bin_work, bin, sizeof(uint) * nBin);

        int countIndex = 0;
        while(Array<uint>::sum(bin_work, nBin) > 0) {

            int count = -1;
            int index = 0;

            for(int i = 0; i < (nBin - removingBins); i++) {

                int tmpCount = Array<uint>::sum(&bin_work[i], removingBins);

                if(tmpCount > count) {
                    count = tmpCount;
                    index = i;
                }
            }

            if(index == 0) {
                countIndex++;
            }

            if(countIndex > 2) {
                break;
            }

            Array<uint>::assign(0, &bin_work[index], removingBins);

            float fstop = (float(index + removingBins ) * dMM) + fMin;

            ret.push_back(fstop);

        }

        return ret;
    }
};


} // end namespace pic

#endif /* PIC_HISTOGRAM_HPP */

