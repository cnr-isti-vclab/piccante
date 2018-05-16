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

#ifndef PIC_ALGORITHMS_LIVE_WIRE_HPP
#define PIC_ALGORITHMS_LIVE_WIRE_HPP

#include <functional>
#include <vector>

#include "../base.hpp"

#include "../image.hpp"
#include "../filtering/filter_luminance.hpp"
#include "../filtering/filter_gradient.hpp"
#include "../filtering/filter_log_2d.hpp"
#include "../filtering/filter_channel.hpp"
#include "../util/vec.hpp"

namespace pic {

class LiveWire
{
protected:
    float fG_min, fG_max;

    Image *img_L, *img_G, *fG, *fZ, *g;
    int *pointers;
    bool *e;

    /**
     * @brief getCost
     * @param x
     * @param y
     * @return
     */
    float getCost(Vec2i &p, Vec2i &q)
    {
        float out;
        float *tmp;

        //fZ cost
        tmp = (*fZ)(q[0], q[1]);
        out = 0.43f * tmp[0];

        //fG cost
        tmp = (*fG)(q[0], q[1]);
        float dist_qp = sqrtf(float(q.distanceSq(p)));
        out += 0.14f * tmp[0] / dist_qp;

        //fD cost

        //D_p
        tmp = (*img_G)(p[0], p[1]);
        Vec<2, float> D_p(tmp[1], -tmp[0]);
        float n_D_p_sq = D_p.lengthSq();
        if(n_D_p_sq > 0.0f) {
            D_p.div(sqrtf(n_D_p_sq));
        }

        //D_q
        tmp = (*img_G)(q[0], q[1]);
        Vec<2, float> D_q(tmp[1], -tmp[0]);
        float n_D_q_sq = D_q.lengthSq();
        if(n_D_q_sq > 0.0f) {
            D_q.div(sqrtf(n_D_q_sq));
        }

        //Delta_qp
        Vec<2, float> delta_qp(float(q[0] - p[0]), float(q[1] - p[1]));

        Vec<2, float> L;
        if(D_p.dot(delta_qp) >= 0.0f) {
            L = delta_qp;
        } else {
            L = delta_qp;
            L.neg();
        }

        float n_L_sq = L.lengthSq();
        if(n_L_sq > 0.0f) {
            L.div(sqrtf(n_L_sq));
        }

        float dp_pq = D_p.dot(L);
        float dq_pq = L.dot(D_q);

        float fD = (acosf(dp_pq) + acosf(dq_pq)) * 2.0f / (3.0f * C_PI);

        out +=  0.43f * fD;

        return out;
    }

    /**
     * @brief release
     */
    void release()
    {
        if(img_L != NULL) {
            delete img_L;
        }

        if(img_G != NULL) {
            delete img_G;
        }

        if(fZ != NULL) {
            delete fZ;
        }

        if(g != NULL) {
            delete g;
        }

        if(e != NULL) {
            delete e;
        }

        if(pointers != NULL) {
            delete pointers;
        }
    }

    /**
     * @brief f1minusx
     * @param x
     * @return
     */
    static float f1minusx(float x) {
        return 1.0f - x;
    }

public:

    LiveWire(Image *img)
    {
        img_L = NULL;
        img_G = NULL;
        fZ = NULL;
        fG = NULL;
        g = NULL;
        e = NULL;
        pointers = NULL;

        set(img);
    }

    ~LiveWire()
    {
        release();
    }

    /**
     * @brief set
     * @param img
     */
    void set(Image *img)
    {
        release();

        img_L = FilterLuminance::Execute(img, img_L);

        //compute fG
        img_G = FilterGradient::Execute(img_L, img_G);
        fG = FilterChannel::Execute(img_G, fG, 2);

        fG->getMinVal(NULL, &fG_min);
        *fG -= fG_min;
        fG->getMaxVal(NULL, &fG_max);
        *fG /= fG_max;
        fG->applyFunction(f1minusx);

        //compute fZ
        fZ = FilterLoG2D::Execute(img_L, fZ, 1.0f);
        fZ->applyFunction(f1minusx);

        //aux buffers
        g = img_L->allocateSimilarOne();
        e = new bool[img_L->nPixels()];

        pointers = new int[img_L->nPixels() * 2];
    }

    /**
     * @brief execute
     * @param pS
     * @param pE
     * @param out
     */
    void execute(Vec2i pS, Vec2i pE, std::vector< Vec2i > &out)
    {
        float *tmp;

        *g = FLT_MAX;

        e = Buffer<bool>::assign(e, img_L->nPixels(), false);
        pointers = Buffer<int>::assign(pointers, img_L->nPixels() * 2, 0);

        int width  = img_L->width;
        int height = img_L->height;

        int nx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        int ny[] = {1, 1, 1, 0, 0, -1, -1, -1};

        tmp = (*g)(pS[0], pS[1]);
        tmp[0] = 0.0f;

        std::vector< Vec2i > list;
        list.push_back(pS);

        while(!list.empty()) {
            //get the best
            std::vector< Vec2i >::iterator index;
            Vec2i q;

            float g_q = FLT_MAX;
            bool bCheck = false;
            for(auto it = list.begin(); it != list.end(); it++) {
                float g_it = (*g)((*it)[0], (*it)[1])[0];
                if(g_it < g_q) {
                    g_q = g_it;
                    index = it;
                    bCheck = true;
                }
            }

            if(!bCheck) {
                break;
            }

            q[0] = (*index)[0];
            q[1] = (*index)[1];

            list.erase(index);

            //update
            e[q[1] * width + q[0]] = true;

            for(int i = 0; i < 8; i++) {
                Vec2i r(  q[0] + nx[i],
                                q[1] + ny[i]);

                if(r[0] > -1 && r[0] < width &&
                   r[1] > -1 && r[1] < height) {

                    if(!e[r[1] * width + r[0]]) {
                        float g_tmp = g_q + getCost(q, r);

                        //check list
                        bool bFlag = false;
                        for(auto it = list.begin(); it != list.end(); it++) {
                            if(r.equal(*it)) {
                                index = it;
                                bFlag = true;
                            }
                        }

                        if(bFlag && (g_tmp < g_q)) {
                            list.erase(index);
                        }

                        if(!bFlag) {
                            tmp = (*g)(r[0], r[1]);
                            tmp[0] = g_tmp;

                            int index = (r[1] * width + r[0]) * 2;
                            pointers[index    ] = q[0];
                            pointers[index + 1] = q[1];
                            list.push_back(r);
                        }

                    }

                }

            }
        }

        //forward pass -- tracking
        out.clear();

        out.push_back(pE);
        Vec2i m = pE;
        Vec2i prev(-1, -1);

        int maxIter = (width + height) * 4;
        int i = 0;

        while(true) {
            prev = m;
            if(m.equal(pS)) {
                break;
            }

            int index = (m[1] * width + m[0]) * 2;
            Vec2i t(pointers[index], pointers[index + 1]);

            out.push_back(t);
            m = t;

            if(prev.equal(m)) {
                break;
            }

            i++;

            if(i > maxIter) {
                break;
            }
        }

    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_LIVE_WIRE_HPP */

