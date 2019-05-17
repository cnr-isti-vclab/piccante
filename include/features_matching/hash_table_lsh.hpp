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

#ifndef PIC_FEATURES_MATCHING_HASH_TABLE_LSH_HPP
#define PIC_FEATURES_MATCHING_HASH_TABLE_LSH_HPP

#include <vector>
#include <math.h>
#include <set>

#include "../features_matching/brief_descriptor.hpp"

namespace pic {

#ifndef PIC_DISABLE_EIGEN

/**
 * @brief The Hash class
 */
class HashTableLSH
{
public:
    unsigned int *g_f;

    std::vector< unsigned int *> *descs;
    std::vector< unsigned int > *table;
    unsigned int nTable;
    unsigned int hash_size, desc_size, size_ui;

    HashTableLSH(unsigned int hash_size, unsigned int *g_f, std::vector< unsigned int *> *descs, unsigned int desc_size)
    {
        if(hash_size == 0) {
            hash_size = 8;
        }

        this->hash_size = hash_size;

        nTable = 1 << hash_size;
        table = new std::vector< unsigned int >[nTable];

        //hash function
        this->g_f = g_f;

        //insert descriptors
        this->descs = descs;
        this->desc_size = desc_size;
        size_ui = sizeof(unsigned int) * 8;

        for(unsigned int i = 0; i < descs->size();  i++) {
            unsigned int address = getAddress(descs->at(i));
            table[address].push_back(i);
        }
    }

    /**
     * @brief getAddress
     * @param point
     * @return
     */
    unsigned int getAddress(unsigned int *desc)
    {
        unsigned int address = 0;
        for(unsigned int i=0; i<hash_size; i++) {
            unsigned int pos = g_f[i];

            unsigned int block = pos / size_ui;
            unsigned int pos_block = pos % size_ui;

            unsigned int bit = (desc[block] >> pos_block) & 0x1;

            if(bit == 1) {
                address += (1 << i);
            }
        }

        return address;
    }

    /**
     * @brief getNearest
     * @param desc
     * @param matched_j
     * @param dist_1
     * @param dist_2
     */
    void getNearest(unsigned int * desc, int &matched_j, unsigned int &dist_1, unsigned int &dist_2)
    {
        unsigned int address = getAddress(desc);

        for(unsigned int i=0; i<table[address].size(); i++) {
            unsigned int j = table[address].at(i);
            unsigned int dist = BRIEFDescriptor::match(desc, descs->at(j), desc_size);

            if(dist > dist_1) {
                dist_2 = dist_1;
                dist_1 = dist;
                matched_j = j;
             } else {
                if(dist > dist_2) {
                    dist_2 = dist;
                }
            }
        }
    }
};

#endif

} // end namespace pic

#endif /* PIC_FEATURES_MATCHING_HASH_TABLE_LSH_HPP */

