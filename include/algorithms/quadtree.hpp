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

#ifndef PIC_ALGORITHMS_QUADTREE_HPP
#define PIC_ALGORITHMS_QUADTREE_HPP

#include <set>

namespace pic {

/**
 * @brief The Quadtree class
 */
class Quadtree
{
protected:
    bool leaf;
    std::set<int> list;
    Quadtree *children[4];

    //bounding box
    int bmax[2], bmin[2];

    /**
     * @brief findAux
     * @param pos
     * @param radius2
     * @param out
     */
    void findAux(int *pos, int radius2, std::set<int> &out)
    {
        if(leaf) {
            out.insert(list.begin(), list.end());
        } else {
            for(int i = 0; i < 4; i++) {
                if(children[i] != NULL) {
                    if(checkCircleBBox(children[i]->bmax, children[i]->bmin, pos, radius2)) {
                        children[i]->findAux(pos, radius2, out);
                    }
                }
            }
        }
    }

public:

    /**
     * @brief Quadtree
     * @param bmax
     * @param bmin
     */
    Quadtree(int *bmax, int *bmin)
    {
        for(int i = 0; i < 2; i++) {
            this->bmax[i] = bmax[i];
            this->bmin[i] = bmin[i];
        }

        leaf = false;

        for(int i = 0; i < 4; i++) {
            children[i] = NULL;
        }
    }

    ~Quadtree()
    {
        for(int i = 0; i < 4; i++)
            if(children[i] != NULL) {
                delete children[i];
            }
    }

    /**
     * @brief checkPointBBox
     * @param p
     * @param bmin
     * @param bmax
     * @return
     */
    static bool checkPointBBox(int *p, int *bmin, int *bmax)
    {
        return((p[0] >= bmin[0])
               && (p[1] >= bmin[1])
               && (p[0] < bmax[0])
               && (p[1] < bmax[1]));
    }

    /**
     * @brief checkCircleBBox
     * @param bmax
     * @param bmin
     * @param center
     * @param radius2
     * @return
     */
    static bool checkCircleBBox(int *bmax, int *bmin, int *center, int radius2)
    {
        int dmin = 0;

        for(int i = 0; i < 2; i++) {
            if(center[i] < bmin[i]) {
                int tmp = center[i] - bmin[i];
                dmin += tmp * tmp;
            } else {
                if(center[i] > bmax[i]) {
                    int tmp = center[i] - bmax[i];
                    dmin += tmp * tmp;
                }
            }
        }

        return (dmin <= radius2);
    }

    /**
     * @brief getQuadrant
     * @param bmax
     * @param bmin
     * @param pMax
     * @param pMin
     * @param i
     */
    static void getQuadrant(int *bmax, int *bmin, int *pMax, int *pMin, int i)
    {
        int half[2];

        for(int j = 0; j < 2; j++) {
            half[j] = (bmax[j] + bmin[j]);

            if((half[j] % 2) == 0) {
                half[j] = half[j] >> 1;
            } else {
                half[j] = (half[j] >> 1) + 1;
            }
        }

        switch(i) {
        case 0: {
            pMin[0] = bmin[0];
            pMin[1] = bmin[1];

            pMax[0] = half[0];
            pMax[1] = half[1];
        }
        break;

        case 1: {
            pMin[0] = half[0];
            pMin[1] = bmin[1];

            pMax[0] = bmax[0];
            pMax[1] = half[1];
        }
        break;

        case 2: {
            pMin[0] = bmin[0];
            pMin[1] = half[1];

            pMax[0] = half[0];
            pMax[1] = bmax[1];
        }
        break;

        case 3: {
            pMin[0] = half[0];
            pMin[1] = half[1];

            pMax[0] = bmax[0];
            pMax[1] = bmax[1];
        }
        break;
        }
    }

    /**
     * @brief insert
     * @param pos
     * @param value
     * @param MAX_OCTREE_LEVEL
     * @param level
     */
    void insert(int *pos, int value, int MAX_OCTREE_LEVEL, int level = 0)
    {
        if(level == MAX_OCTREE_LEVEL) {
            list.insert(value);
            leaf = true;
        } else {
            int pMax[2], pMin[2];

            for(int i = 0; i < 4; i++) {
                getQuadrant(bmax, bmin, pMax, pMin, i);

                if(checkPointBBox(pos, pMin, pMax)) {
                    if(children[i] == NULL) {
                        children[i] = new Quadtree(pMax, pMin);
                    }

                    children[i]->insert(pos, value, MAX_OCTREE_LEVEL, level + 1);
                    break;
                }
            }
        }
    }

    /**
     * @brief find
     * @param x
     * @param y
     * @param radius
     * @param out
     */
    void find(float x, float y, float radius, std::set<int> &out)
    {

        int pos[2];
        pos[0] = int(x);
        pos[1] = int(y);
        int radius2 = int(ceilf(radius * radius));

        if(checkPointBBox(pos, bmin, bmax)) {
            findAux(pos, radius2, out);
        }
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_QUADTREE_HPP */

