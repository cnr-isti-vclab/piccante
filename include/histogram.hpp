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
    float			*bin_nor;
    int				nBin;
    VALUE_SPACE		type;
    float			fMin, fMax;

public:
    unsigned int	*bin, *bin_work;

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
    }

    /**
     * @brief Histogram is an extension of the basic constructor, where Calculate
     * is called in order to populate the Histogram.
     * @param imgIn is an input image for which Histogram needs to be computed.
     * @param type is the space of computations (please see Calculate()).
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

        Calculate(imgIn, type, nBin, channel);
    }

    /**
    * @brief ~Histogram is the basic destructor which frees memory.
    */
    ~Histogram()
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

        nBin = 0;
        type =  VS_LIN;
        fMin = -FLT_MAX;
        fMax =  FLT_MAX;
    }

    /**
     * @brief Calculate computes the histogram of an input image. In the case
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
    void Calculate(Image *imgIn, VALUE_SPACE type, int nBin,
                              int channel = 0)
    {
        if(imgIn == NULL) {
            return;
        }

        if((channel < 0) || (channel >= imgIn->channels)) {
            return;
        }

        if(nBin < 1) {
            nBin = 256;
        }

        if(type == VS_LDR) {
            nBin = 256;
        }

        bin = new unsigned int[nBin];
        memset((void *)bin, 0, nBin * sizeof(unsigned int));

        this->nBin = nBin;
        this->type = type;

        int size = imgIn->width * imgIn->height * imgIn->channels;
        int channels = imgIn->channels;

        //Statistics
        float epsilon = 1e-6f;
        fMin =  FLT_MAX;
        fMax = -FLT_MAX;
        float log2f = logf(2.0f);

        for(int i = channel; i < size; i += channels) {
            float val = imgIn->data[i];

            switch(type) {
                case VS_LOG_2: {
                    val = logf(imgIn->data[i] + epsilon) / log2f;
                }
                break;

                case VS_LOG_E: {
                    val = logf(imgIn->data[i] + epsilon);
                }
                break;

                case VS_LOG_10: {
                    val = log10f(imgIn->data[i] + epsilon);
                }
                break;
            }

            fMin = MIN(fMin, val);
            fMax = MAX(fMax, val);
        }

        if(type == VS_LDR) {
            fMin = 0.0f;
            fMax = 1.0f;
        }
    
        float deltaMaxMin = (fMax - fMin);
        float nBinf = float(nBin - 1);

        //Histogram calculation
        for(int i = channel; i < size; i += channels) {
            float val = imgIn->data[i];
            switch(type) {
                case VS_LOG_2: {
                    val = logf(imgIn->data[i] + epsilon) / log2f;
                }
                break;

                case VS_LOG_E: {
                    val = logf(imgIn->data[i] + epsilon);
                }
                break;

                case VS_LOG_10: {
                    val = log10f(imgIn->data[i] + epsilon);
                }
                break;
            }

            int indx = int(((val - fMin) * nBinf) / deltaMaxMin);

            #ifdef PIC_DEBUG
            if((indx >= nBin) || (indx<0)) {
                printf("Error in Calculate %d.\n",indx);
            }
            #endif

            bin[ CLAMP(indx, nBin) ]++;
        }
    }

    /**
     * @brief Ceiling limits the maximum value of the histogram using Ward
     * algorithm.
     */
    void Ceiling()
    {
        float k = 1.0f / float(nBin - 1);
        float tolerance = float(Array<unsigned int>::sum(bin, nBin)) * 0.025f;
        int   trimmings = 0;
        bool  bFlag     = true;

        while((trimmings <= tolerance) && bFlag) {
            trimmings = 0;
            float T = float(Array<unsigned int>::sum(bin, nBin));

            if(T < tolerance) {
                bFlag = false;
            } else {
                unsigned int ceiling = int(T * k);

                for(int i = 0; i < nBin; i++) {
                    if(bin[i] > ceiling) {
                        trimmings = trimmings + bin[i] - ceiling;
                        bin[i] = ceiling;
                    }
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
    
        bin_c = Array<float>::cumsum(bin_nor, bin_c, nBin);

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
        float maxValf = float(Array<unsigned int>::max(bin, nBin, ind));

        for(int i = 0; i < nBin; i++) {
            bin_nor[i] = float(bin[i]) / maxValf;
        }

        return bin_nor;
    }

    /**
     * @brief Write saves the Histogram as an Image into a file.
     * @param name is the filename where to save the Histogram.
     * @param bNor is a boolean value for normalizing or not the Histogram.
     */
    void Write(std::string name, bool bNor)
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

        img.Write(name);
    }

    /**
     * @brief ExposureCovering computes the exposure values for fully covering
     * the dynamic range of the image. This function works only if the histogram
     * was compute usign VS_LOG_2.
     * @param bits is the number of bit used for storing each output exposure image.
     * The default value is 8.
     * @param overlap is the value, in f-stops, of overlapping between two exposure images.
     * This value is set to 1 by default.
     * @return It returns an std::vector<float> which contains the exposure values
     * in f-stops for all required exposures for covering information.
     */
    std::vector< float > ExposureCovering(int bits = 8, float overlap = 1.0f)
    {
        std::vector< float > ret;

        if(type != VS_LOG_2) {
            #ifdef PIC_DEBUG
                printf("ERROR in ExposureCovering: this histogram has to be in log2!\n");
            #endif
            
            return ret;
        }

        //assuming 8-bit images
        float halfBits = float( bits >> 1 );

        float dMM = (fMax - fMin) / float(nBin);

        int removingBins = MAX(int(lround((halfBits - overlap) / dMM)), 1);

        if( bin_work == NULL) {
            bin_work = new unsigned int [nBin];
        }

        memcpy(bin_work, bin, sizeof(unsigned int) * nBin);

        while(Array<unsigned int>::sum(bin_work, nBin) > 0) {
            int ind;
            Array<unsigned int>::max(bin_work, nBin, ind);

            int indMin = MAX(ind - removingBins, 0);
            int indMax = MIN(ind + removingBins, nBin);

            for(int i=indMin; i<indMax; i++) {
                bin_work[i] = 0;
            }

            float fstop = - (float(ind) * dMM + fMin) - 1.0f;

            ret.push_back(fstop);
        }

        return ret;
    }

    /**
     * @brief BestInterval computes the best
     * @param intSize is the budget dynamic range for the output image in f-stops
     * @return It returns the exposure, in f-stops, for setting the image
     * with the best exposure at given dynamic range.
     */
    float BestInterval(float intSize)
    {
        if((type != VS_LOG_2) && (intSize > (fMax - fMin))){
            return 0.0f;
        }

        float iDelta = float(nBin + 1) / (fMax - fMin);
        int iS = int(intSize * iDelta + 0.5);

        #ifdef PIC_DEBUG
            printf("Histogram [%f %f] %d\n", fMin, fMax, iS);
        #endif

        int count = 0;
        int index = 0;

        for(int i = 0; i < (nBin - iS); i++) {

            int tmpCount = 0;

            for(int j = i; j < (iS + i); j++) {
                tmpCount += bin[j];
            }

            if(tmpCount > count) {
                count = tmpCount;
                index = i;
            }
        }

        return (float(index + iS) / iDelta) + fMin;
    }

    /**
     * @brief FindBestExposure computes the best exposure given
     * a budget dynamig range.
     * @param range is the input dynamic range.
     * @return It returns the exposure value in f-stops.
     */
    float FindBestExposure(float range = 8.0f)
    {
        return -BestInterval(range);
    }
};


} // end namespace pic

#endif /* PIC_HISTOGRAM_HPP */

