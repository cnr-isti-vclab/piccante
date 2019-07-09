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
    float           *bin_c;
    float           *bin_nor;
    int             nBin;
    VALUE_SPACE     type;
    float           fMin, fMax;
    float           deltaMaxMin, nBinf;
    float           epsilon;

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

public:
    unsigned int *bin, *bin_work;

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

        calculate(imgIn, type, nBin, channel);
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
        if(bin != NULL) {
            delete [] bin;
            bin = NULL;
        }

        if(bin_c != NULL) {
            delete [] bin_c;
            bin_c = NULL;
        }

        if(bin_nor != NULL) {
            delete [] bin;
            bin_nor = NULL;
        }

        if(bin_work != NULL) {
            delete[] bin_work;
            bin_work = NULL;
        }
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
     * @param channel is the color channel for which the Histogram will be computed.
     */
    void calculate(Image *imgIn, VALUE_SPACE type, int nBin,
                              int channel = 0)
    {
        if((imgIn == NULL) || (channel < 0) ) {
            return;
        }

        if(!imgIn->isValid() || (channel >= imgIn->channels)) {
            return;
        }

        if(nBin < 1 || type == VS_LDR) {
            nBin = 256;
        }

        bool c1 = (nBin != this->nBin) && (bin != NULL);
        bool c2 = (bin == NULL);
        if(c1 || c2)  {
            release();

            bin = new unsigned int[nBin];
            memset((void *)bin, 0, nBin * sizeof(unsigned int));
        }

        this->nBin = nBin;
        this->type = type;

        int size = imgIn->width * imgIn->height * imgIn->channels;
        int channels = imgIn->channels;

        //compute statistics
        fMin =  FLT_MAX;
        fMax = -FLT_MAX;

        for(int i = channel; i < size; i += channels) {
            float val = imgIn->data[i];
            fMin = MIN(fMin, val);
            fMax = MAX(fMax, val);
        }

        fMin = projectDomain(fMin);
        fMax = projectDomain(fMax);

        deltaMaxMin = (fMax - fMin);
        nBinf = float(nBin - 1);

        //compute the histogram
        for(int i = channel; i < size; i += channels) {           
            float val = projectDomain(imgIn->data[i]);

            int indx = int(((val - fMin) * nBinf) / deltaMaxMin);

            #ifdef PIC_DEBUG
            if((indx >= nBin) || (indx < 0)) {
                printf("Error in Calculate %d.\n",indx);
            }
            #endif

            bin[CLAMP(indx, nBin)]++;
        }
    }

    /**
     * @brief project converts an input value in the histogram domain.
     * @param x is an input value.
     * @return x is projected in the histogram domain.
     */
    int project(float x)
    {
        float y = projectDomain(x);
        return int(((y - fMin) * nBinf) / deltaMaxMin);
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
     */
    void ceiling(float k)
    {
        float tolerance = float(Array<unsigned int>::sum(bin, nBin)) * 0.025f;
        int   trimmings = 0;
        bool  bFlag = true;

        std::vector<bool> trimmed_vec;

        while((trimmings <= tolerance) && bFlag) {
            trimmings = 0;
            float T = float(Array<unsigned int>::sum(bin, nBin));

            if(T < tolerance) {
                bFlag = false;
            } else {
                bool bTrimmed = false;
                unsigned int ceiling = int(T * k);

                for(int i = 0; i < nBin; i++) {
                    if(bin[i] > ceiling) {
                        trimmings += (bin[i] - ceiling);
                        bin[i] = ceiling;
                        bTrimmed = true;
                    }
                }

                trimmed_vec.push_back(bTrimmed);

            }

            int tvSize = trimmed_vec.size();
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
     * @brief cumulativef computes the cumulative Histogram.
     * @param bNormalized is a boolean value; if it is true values of
     * the Histogram will be normalized.
     * @return It returns the cumulative Histogram as a float pointer.
     */
    float *cumulativef(bool bNormalized)
    {
        getNormalized();
    
        bin_c = Array<float>::cumsum(bin_nor, nBin, bin_c);

        if(bNormalized) {
            for(int i=0; i<nBin; i++) {
                bin_c[i] /= bin_c[nBin - 1];
            }
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
     * @brief getNormalized normalizes the Histogram.
     * @return It returns the normalized Histogram as a float pointer.
     */
    float *getNormalized()
    {
        if(bin_nor == NULL) {
            bin_nor = new float[nBin];
        }

        int ind;
        float maxValf = float(Array<unsigned int>::getMax(bin, nBin, ind));

        for(int i = 0; i < nBin; i++) {
            bin_nor[i] = float(bin[i]) / maxValf;
        }

        return bin_nor;
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
            memcpy(img.data, bin_nor, sizeof(float)*nBin);
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

        //NOTE: this code assumes to extract 8-bit images
        float halfnBits = float( nBits >> 1 );

        float dMM = deltaMaxMin / nBinf;

        int removingBins = MAX(int(lround((halfnBits - overlap) / dMM)), 1);

        if( bin_work == NULL) {
            bin_work = new unsigned int [nBin];
        }

        memcpy(bin_work, bin, sizeof(unsigned int) * nBin);

        while(Array<unsigned int>::sum(bin_work, nBin) > 0) {
            int ind;
            Array<unsigned int>::getMax(bin_work, nBin, ind);

            int indMin = MAX(ind - removingBins, 0);
            int indMax = MIN(ind + removingBins, nBin);

            for(int i = indMin; i < indMax; i++) {
                bin_work[i] = 0;
            }

            float fstop_ind = float(ind) * dMM + fMin;
            float fstop = -fstop_ind - 1.0f;

            ret.push_back(fstop);
        }

        return ret;
    }

    /**
     * @brief getBestExposure computes the best interval center.
     * @param nBits is the number of bits in the budget for the output image.
     * @return It returns the exposure, in f-stops, for setting the image
     * with the best exposure at given dynamic range.
     */
    float getBestExposure(int nBits, float overlap = 0.5f)
    {
        if(overlap < 0.0f) {
            overlap = 0.5f;
        }

        float nBits_f = float(nBits);
        if((type != VS_LOG_2) && (nBits_f > deltaMaxMin) && (nBits < 1)){
            return 0.0f;
        }

        float n_values_f = float(1 << nBits);
        float dMM = deltaMaxMin / nBinf;
        int range_size_hist = int(float(nBits) / (1.0f / (n_values_f * dMM)) + overlap);

        range_size_hist = range_size_hist < 1 ? 2 : range_size_hist;

        #ifdef PIC_DEBUG
            printf("Histogram [%f %f] %d\n", fMin, fMax, range_size_hist);
        #endif

        int count = 0;
        int index = 0;

        for(int i = 0; i < (nBin - range_size_hist); i++) {
            int tmpCount = 0;

            for(int j = i; j < (i + range_size_hist); j++) {
                tmpCount += bin[j];
            }

            if(tmpCount > count) {
                count = tmpCount;
                index = i;
            }
        }

        int mid = range_size_hist >> 1;

        float fstop_index = (float(index + mid) * dMM) + fMin;
        return -fstop_index - 1.0f;
    }
};


} // end namespace pic

#endif /* PIC_HISTOGRAM_HPP */

