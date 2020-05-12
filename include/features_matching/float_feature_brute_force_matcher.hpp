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

#ifndef PIC_FEATURES_MATCHING_FLOAT_FEATURE_BRUTE_FORCE_MATCHER
#define PIC_FEATURES_MATCHING_FLOAT_FEATURE_BRUTE_FORCE_MATCHER

#include <vector>

#include "../features_matching/feature_matcher.hpp"
#include "../features_matching/sift_descriptor.hpp"

namespace pic{

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The FloatFeatureBruteForceMatcher class
 */
class FloatFeatureBruteForceMatcher : public FeatureMatcher<float>
{
public:

    /**
     * @brief FloatFeatureBruteForceMatcher
     * @param descs
     * @param n
     */
    FloatFeatureBruteForceMatcher(std::vector<unsigned int *> *descs, unsigned int desc_size) : FeatureMatcher<float>(descs, desc_size)
    {
    }

    /**
     * @brief getMatch
     * @param desc0
     * @param matched_j
     * @param dist_1
     * @return
     */
    bool getMatch(float *desc, int &matched_j, float &dist_1)
    {
        float dist_2 = 1e32f;

        dist_1 = 1e32f;

        matched_j = -1;

        for(unsigned int j = 0; j < descs->size(); j++) {
            float dist = SIFTDescriptor::match(desc, descs->at(j), desc_size);

            if(dist < dist_1) {
                dist_2 = dist_1;
                dist_1 = dist;
                matched_j = j;
             } else {
                if(dist < dist_2) {
                    dist_2 = dist;
                }
            }
        }

        return ((dist_1 > dist_2 * 1.2f) && matched_j != -1);
    }
};

#endif

}

#endif // PIC_FEATURES_MATCHING_FLOAT_FEATURE_BRUTE_FORCE_MATCHER
