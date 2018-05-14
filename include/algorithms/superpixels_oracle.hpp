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

#ifndef PIC_ALGORITHMS_SUPERPIXELS_ORACLE_HPP
#define PIC_ALGORITHMS_SUPERPIXELS_ORACLE_HPP

#include <vector>
#include <set>

#include "../algorithms/quadtree.hpp"

namespace pic {

/**
 * @brief The SuperPixelsOracle class
 */
class SuperPixelsOracle
{
protected:

    int					*buffer;
    int					width, height;
    std::vector<int>	unique;

    Quadtree			*root;

    /**
     * @brief init
     */
    void init()
    {
        int size = width * height;

        std::set<int> keys;

        for(int i = 0; i < size; i++) {
            keys.insert(buffer[i]);
        }

        unique.insert(unique.begin(), keys.begin(), keys.end());

        int bmin[2], bmax[2];
        bmin[0] = 0;
        bmin[1] = 0;

        bmax[0] = width;
        bmax[1] = height;

        int tmp_max_level = width > height ? height : width;
        int max_level = int(ceilf(logf(float(tmp_max_level)) / logf(2.0f)));

        printf("Max Level: %d\n", max_level);
        root = new Quadtree(bmax, bmin);

        for(int i = 0; i < height; i++) {
            int pos[2];
            int ind = i * width;
            pos[1] = i;

            for(int j = 0; j < width; j++) {
                pos[0] = j;
                root->insert(pos, buffer[ind + j], max_level);
            }
        }
    }

public:

    /**
     * @brief SuperPixelsOracle
     * @param buffer
     * @param width
     * @param height
     */
    SuperPixelsOracle(int *buffer, int width, int height)
    {
        if((buffer == NULL) || (width < 1) || (height < 1)) {
            return;
        }

        this->buffer = buffer;
        this->width  = width;
        this->height = height;
        init();
    }

    ~SuperPixelsOracle()
    {
        delete root;
    }

    /**
     * @brief query
     * @param x
     * @param y
     * @param r
     * @param out
     */
    void query(float x, float y, float r, std::set<int> &out)
    {
        root->find(x, y, r, out);
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_SUPERPIXELS_ORACLE_HPP */

