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

#ifndef PIC_ALGORITHMS_SUB_SAMPLE_STACK_HPP
#define PIC_ALGORITHMS_SUB_SAMPLE_STACK_HPP

#include "../util/math.hpp"
#include "../util/std_util.hpp"

#include "../image.hpp"
#include "../image_vec.hpp"
#include "../point_samplers/sampler_random.hpp"
#include "../histogram.hpp"

namespace pic {

/**
 * @brief The SubSampleStack class
 */
class SubSampleStack
{
protected:

    /**
    * \brief sampleGrossberg creates a low resolution version of the stack using Grossberg and Nayar sampling.
    * \param stack is a stack of Image* at different exposures
    */
    void sampleGrossberg(ImageVec &stack)
    {
        #ifdef PIC_DEBUG
            printf("Computing histograms...");
        #endif

        Histogram *h = new Histogram[exposures * channels];

        int c = 0;
        for(int j = 0; j < channels; j++) {
            for(int i = 0; i < exposures; i++) {
                h[c].calculate(stack[i], VS_LDR, 256, NULL, j);
                h[c].cumulativef(true);
                c++;
            }
        }

        #ifdef PIC_DEBUG
            printf("Ok\n");
        #endif

        total = this->nSamples * this->channels * this->exposures;
        samples = new int[total];

        #ifdef PIC_DEBUG
            printf("Sampling...");
        #endif

        float div = float(nSamples - 1);
        c = 0;
        for(int k = 0; k < channels; k++) {
            for(int i = 0; i < nSamples; i++) {

                float u = float(i) / div;

                for(int j = 0; j < exposures; j++) {

                    int ind = k * exposures + j;

                    float *bin_c = h[ind].getCumulativef();

                    float *ptr = std::upper_bound(&bin_c[0], &bin_c[0]+256, u);

                    samples[c] = CLAMPi((int)(ptr - bin_c), 0, 255);
                    c++;
                }
            }
        }

        #ifdef PIC_DEBUG
            printf("Ok\n");
        #endif

        delete[] h;
    }

    /**
     * @brief sampleSpatial creates a low resolution version of the stack.
     * @param stack is a stack of Image* at different exposures
     * @param sub_type
     */
    void sampleSpatial(ImageVec &stack, SAMPLER_TYPE sub_type = ST_MONTECARLO_S)
    {
        int width    = stack[0]->width;
        int height   = stack[0]->height;

        Vec<2, int> vec(width, height);

        RandomSampler<2> *sampler = new RandomSampler<2>(sub_type, vec, nSamples, 1, 0);

        #ifdef PIC_DEBUG
            int oldNSamples = nSamples;
        #endif

        this->nSamples = sampler->getSamplesPerLevel(0);

        total = this->nSamples * this->channels * this->exposures;
        samples = new int[total];

        #ifdef PIC_DEBUG
            printf("--subSample samples: %d \t \t old samples: %d\n", nSamples, oldNSamples);
        #endif

        int c = 0;

        for(int k = 0; k < channels; k++) {
            for(int i = 0; i < nSamples; i++) {

                int x, y;
                sampler->getSampleAt(0, i, x, y);

                for(int j = 0; j < exposures; j++) {
                    float fetched = (*stack[j])(x, y)[k];
                    float tmp = lround(fetched * 255.0f);
                    samples[c] = CLAMPi(int(tmp), 0, 255);
                    c++;
                }
            }
        }

        delete sampler;
    }

    int exposures;
    int channels;
    int nSamples;
    int total;
    int *samples;

public:
    
    /**
     * @brief SubSampleStack
     */
    SubSampleStack()
    {
        total = 0;
        exposures = 0;
        channels = 0;
        nSamples = 0;
        samples = NULL;
    }

    ~SubSampleStack()
    {
        release();
    }

    /**
     * @brief release
     */
    void release()
    {
        exposures = 0;
        channels = 0;
        nSamples = 0;
        total = 0;

        samples = delete_s(samples);
    }

    /**
     * @brief execute
     * @param stack
     * @param nSamples output number of samples
     * @param bSpatial
     * @param sub_type
     */
    void execute(ImageVec &stack, int nSamples, float alpha = 0.f, bool bSpatial = false, SAMPLER_TYPE sub_type = ST_MONTECARLO_S)
    {
        release();

        if(!((stack.size() > 1 && (nSamples > 1)))) {
            return;
        }

        this->nSamples = nSamples;
        this->channels  = stack[0]->channels;
        this->exposures = int(stack.size());

        if(bSpatial) {
            sampleSpatial(stack, sub_type);
        } else {
            sampleGrossberg(stack);
        }

        if (alpha < 0.f || alpha > 1.f)
            alpha = 0.f;
        else if (alpha > 0.5f)
            alpha = 1.f - alpha;

        if(alpha > 0.f && alpha <= 0.5f) {
            float t_min_f = alpha;
            float t_max_f = 1.0f - t_min_f;

            int t_min = int(t_min_f * 255.0f);
            int t_max = int(t_max_f * 255.0f);

            for(int i = 0; i < total; i++) {
                if(samples[i] < t_min || samples[i] > t_max) {
                    samples[i] = -1;
                }
            }
        }
    }

    /**
     * @brief get
     * @return
     */
    int *get()
    {
        return samples;
    }

    /**
     * @brief getNSamples
     * @return
     */
    int getNSamples() const
    {
        return nSamples;
    }

    /**
     * @brief print
     */
    void print()
    {
        for(int i = 0; i < total; i++) {
           printf("%d\n", samples[i]);
        }

    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_SUB_SAMPLE_STACK_HPP */

