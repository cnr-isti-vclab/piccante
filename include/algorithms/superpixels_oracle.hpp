/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_ALGORITHMS_SUPERPIXELS_ORACLE_HPP
#define PIC_ALGORITHMS_SUPERPIXELS_ORACLE_HPP

#include <vector>
#include <set>

#include "algorithms/quadtree.hpp"

namespace pic {

class SuperPixelsOracle
{
protected:

    int					*buffer;
    int					width, height;
    std::vector<int>	unique;

    Quadtree			*root;

    void Init()
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
                root->Insert(pos, buffer[ind + j], max_level);
            }
        }
    }

public:

    SuperPixelsOracle(int *buffer, int width, int height)
    {
        if((buffer == NULL) || (width < 1) || (height < 1)) {
            return;
        }

        this->buffer = buffer;
        this->width  = width;
        this->height = height;
        Init();
    }

    ~SuperPixelsOracle()
    {
        delete root;
    }

    void Query(float x, float y, float r, std::set<int> &out)
    {
        root->Find(x, y, r, out);
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_SUPERPIXELS_ORACLE_HPP */

