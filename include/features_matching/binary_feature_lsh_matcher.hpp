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

#ifndef PIC_FEATURES_MATCHING_BINARY_FEATURE_LSH_MATCHER_HPP
#define PIC_FEATURES_MATCHING_BINARY_FEATURE_LSH_MATCHER_HPP

#include <vector>

#include "../base.hpp"
#include "../features_matching/hash_table_lsh.hpp"
#include "../features_matching/feature_matcher.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The LSH class
 */
class BinaryFeatureLSHMatcher: public FeatureMatcher<uint>
{
protected:
    std::vector< HashTableLSH* > tables;
    uint R;

public:

    /**
     * @brief LSH
     */
    BinaryFeatureLSHMatcher(std::vector< uint *> *descs, uint desc_size, uint nTables = 32, uint hash_size = 8) : FeatureMatcher<uint>(descs, desc_size)
    {
        this->R = ((desc_size * sizeof(uint) * 8) * 90) / 100;

        std::mt19937 m_rnd(1);

        for(uint i=0; i < nTables; i++) {
            uint n = desc_size * sizeof(uint) * 8;
            uint *g_f = getHash(m_rnd, n, hash_size);
            HashTableLSH *tmp = new HashTableLSH(hash_size, g_f, descs, desc_size);
            tables.push_back(tmp);
        }
    }

    /**
     * @brief getHash
     * @param dim
     * @param hash_size
     * @param seed
     * @return
     */
    static uint *getHash(std::mt19937 &m, uint dim, uint hash_size = 0)
    {
        if(hash_size == 0) {
            hash_size = 8;
        }

        uint *out = new uint[hash_size];

        std::set<uint> tmp;

        int c = 0;
        while (tmp.size() < hash_size) {
            uint val = m() % dim;
            auto result = tmp.insert(val);

            if(result.second) {
                out[c] = val;
                c++;
            }
        }

        return out;
    }

    /**
     * @brief getMatch
     * @param desc0
     * @param matched_j
     * @param dist_1
     * @return
     */
    bool getMatch(uint *desc, int &matched_j, uint &dist_1)
    {
        uint dist_2 = 0;

        dist_1 = R;
        matched_j = -1;

        for(uint i = 0; i < tables.size(); i++) {
            tables[i]->getNearest(desc, matched_j, dist_1, dist_2);
        }

        return (matched_j != -1);// && (dist_1 * 100 > dist_2 * 105);
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_BINARY_FEATURE_LSH_MATCHER_HPP */

