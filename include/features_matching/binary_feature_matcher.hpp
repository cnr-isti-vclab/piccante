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

#ifndef PIC_FEATURES_MATCHING_BINARY_FEATURE_MATCHER
#define PIC_FEATURES_MATCHING_BINARY_FEATURE_MATCHER

#include <vector>

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#elif
    #include <Eigen/Dense>
#endif

#endif

namespace pic{

/**
 * @brief The BinaryFeatureMatcher class
 */
class BinaryFeatureMatcher
{
protected:
    std::vector<unsigned int *> *descs;
    unsigned int desc_size;
    unsigned int R;

public:

    /**
     * @brief BinaryFeatureMatcher
     * @param descs
     * @param n
     */
    BinaryFeatureMatcher(std::vector<unsigned int *> *descs, unsigned int desc_size)
    {
        this->desc_size = desc_size;
        this->descs = descs;
        this->R = ((desc_size * sizeof(unsigned int) * 8) * 90) / 100;
    }

    /**
     * @brief getMatch
     * @param desc0
     * @param matched_j
     * @param dist_1
     * @return
     */
    virtual bool getMatch(unsigned int *desc, int &matched_j, unsigned int &dist_1)
    {
        return false;
    }

#ifndef PIC_DISABLE_EIGEN
    void getAllMatches(std::vector<unsigned int *> &descs0, std::vector< Eigen::Vector3i > &matches)
    {
        matches.clear();

        for(unsigned int i = 0; i< descs0.size(); i++) {
            int matched_j;
            unsigned int dist_1;

            if(getMatch(descs0.at(i), matched_j, dist_1)) {
                matches.push_back(Eigen::Vector3i(i, matched_j, dist_1));
            }
        }
    }

    /**
     * @brief filterMatches
     * @param c0
     * @param c1
     * @param matches
     * @param p0
     * @param p1
     */
    static void filterMatches(  std::vector< Eigen::Vector2f > &c0,
                                std::vector< Eigen::Vector2f > &c1,
                                std::vector< Eigen::Vector3i > &matches,
                                std::vector< Eigen::Vector2f > &p0,
                                std::vector< Eigen::Vector2f > &p1)
    {
        p0.clear();
        p1.clear();

        for(size_t i = 0; i < matches.size(); i++) {
            int I0 = matches[i][0];
            int I1 = matches[i][1];

            Eigen::Vector2f x = c0.at(I0);
            Eigen::Vector2f y = c1.at(I1);

            p0.push_back(x);
            p1.push_back(y);

            #ifdef PIC_DEBUG
            printf("I1: %d (%d %d) -- I2: %d (%d %d) -- Score: %d\n",
                   I0, int(x[0]), int(x[1]), I1, int(y[0]), int(y[1]), matches[i][2]);
            #endif
        }
    }

#endif
};

}

#endif // PIC_FEATURES_MATCHING_BINARY_FEATURE_MATCHER
