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
#include "../filtering/filter_log_2d_opt.hpp"
#include "../filtering/filter_channel.hpp"
#include "../filtering/filter_sampler_2d.hpp"
#include "../util/vec.hpp"
#include "../util/std_util.hpp"

namespace pic {

class LiveWire
{
protected:
    float fD_const;

    float *fG_min, *fG_max;

    Image *img_G, *fZ, *g;
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
        tmp = (*img_G)(q[0], q[1]);

        float fG = 1.0f - (tmp[2] - fG_min[2]) / fG_max[2];
        float dist_qp = sqrtf(float(q.distanceSq(p)));
        out += 0.14f * fG / dist_qp;

        //fD cost

        //D_p
        tmp = (*img_G)(p[0], p[1]);
        Vec2f D_p(tmp[1], -tmp[0]);
        float n_D_p_sq = D_p.lengthSq();
        if(n_D_p_sq > 0.0f) {
            D_p /= sqrtf(n_D_p_sq);
        }

        //D_q
        tmp = (*img_G)(q[0], q[1]);
        Vec2f D_q(tmp[1], -tmp[0]);
        float n_D_q_sq = D_q.lengthSq();
        if(n_D_q_sq > 0.0f) {
            D_q /= sqrtf(n_D_q_sq);
        }

        //Delta_qp
        Vec2f delta_qp(float(q[0] - p[0]), float(q[1] - p[1]));

        Vec2f L;
        if(D_p.dot(delta_qp) >= 0.0f) {
            L = delta_qp;
        } else {
            L = -delta_qp;
        }

        float n_L_sq = L.lengthSq();
        if(n_L_sq > 0.0f) {
            L /= sqrtf(n_L_sq);
        }

        float dp_pq = D_p.dot(L);
        float dq_pq = L.dot(D_q);

        float fD = (acosf(dp_pq) + acosf(dq_pq)) * fD_const;

        out +=  0.43f * fD;

        return out;
    }

    /**
     * @brief release
     */
    void release()
    {
        img_G = delete_s(img_G);
        fZ = delete_s(fZ);
        g = delete_s(g);
        e = delete_s(e);
        pointers = delete_s(pointers);
    }

    /**
     * @brief f1minusx
     * @param x
     * @return
     */
    static float f1minusx(float x)
    {
        return 1.0f - x;
    }

public:

    LiveWire(Image *img)
    {
        fD_const = 2.0f / (C_PI * 3.0f);

        img_G = NULL;
        fZ = NULL;
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

        Image *img_L = FilterLuminance::execute(img, NULL);

        //compute fG
        img_G = FilterGradient::execute(img_L, img_G);
        fG_min = img_G->getMinVal(NULL, NULL);
        fG_max = img_G->getMaxVal(NULL, NULL);

        //compute fZ
        fZ = FilterLoG2DOpt::execute(img_L, fZ, 1.0f);
        fZ->applyFunction(f1minusx);

        //aux buffers
        g = img_L;

        e = new bool[img_L->nPixels()];

        pointers = new int[img_L->nPixels()];
    }

    /**
     * @brief execute
     * @param pS
     * @param pE
     * @param out
     * @param bConstrained
     * @param bMultiple
     */
    void execute(Vec2i pS, Vec2i pE, std::vector< Vec2i > &out, bool bConstrained = false, bool bMultiple = false)
    {
        float *tmp;

        e = Buffer<bool>::assign(e, g->nPixels(), false);
        //*g = FLT_MAX;
        //pointers = Buffer<int>::assign(pointers, g->nPixels(), 0);

        int width  = g->width;
        int height = g->height;

        int nx[] = {-1, 0, 1, -1, 1, -1,  0, 1};
        int ny[] = { 1, 1, 1,  0, 0, -1, -1, -1};

        int bX[2], bY[2];

        if(!bConstrained) {
            bX[0] = -1;
            bX[1] = width;

            bY[0] = -1;
            bY[1] = height;
        } else {
            int boundSize = 11;

            bX[0] = MAX(MIN(pS[0], pE[0]) - boundSize, -1);
            bX[1] = MIN(MAX(pS[0], pE[0]) + boundSize, width);

            bY[0] = MAX(MIN(pS[1], pE[1]) - boundSize, -1);
            bY[1] = MIN(MAX(pS[1], pE[1]) + boundSize, height);
        }

        tmp = (*g)(pS[0], pS[1]);
        tmp[0] = 0.0f;

        std::vector< Vec2i > list;
        list.push_back(pS);

        while(!list.empty()) { //get the best
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
            int index_q = q[1] * width + q[0];
            e[index_q] = true;

            for(int i = 0; i < 8; i++) {
                Vec2i r(q[0] + nx[i], q[1] + ny[i]);

                if((r[0] > bX[0]) && (r[0] < bX[1]) &&
                   (r[1] > bY[0]) && (r[1] < bY[1])) {

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

                            int index = (r[1] * width + r[0]);
                            pointers[index] = index_q;
                            list.push_back(r);
                        }

                    }

                }

            }
        }

        //forward pass -- tracking
        if(!bMultiple) {
            out.clear();
        }

        out.push_back(pE);
        Vec2i m = pE;
        Vec2i prev(-1, -1);

        int maxIter = (width * height);
        int i = 0;

        while((!prev.equal(m)) && (i < maxIter)) {
            prev = m;

            if(m.equal(pS)) {
                break;
            }

            int index = (m[1] * width + m[0]);
            int t_x = pointers[index] % width;
            int t_y = pointers[index] / width;
            Vec2i t(t_x, t_y);

            out.push_back(t);
            m = t;

            i++;
        }

    }

    /**
     * @brief executeLiveWireSingle
     * @param in
     * @param pS
     * @param pE
     * @param out
     */
    static void executeLiveWireSingle(Image *in, Vec2i pS, Vec2i pE, std::vector< Vec2i > &out)
    {
        if(in != NULL) {
            pic::LiveWire *lw = new pic::LiveWire(in);

            lw->execute(pS, pE, out, true, false);

            delete lw;
        }
    }

    /**
     * @brief executeLiveWireMultiple
     * @param in
     * @param controlPoint
     * @param out
     */
    static void executeLiveWireMultiple(Image *in, std::vector< Vec2i > &controlPoints, std::vector< Vec2i > &out)
    {
        if(in != NULL) {
            pic::LiveWire *lw = new pic::LiveWire(in);

            for(uint i = 0; i < (controlPoints.size() - 1); i++) {
                lw->execute(controlPoints.at(i), controlPoints.at(i + 1), out, true, true);
            }

            delete lw;
        }
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_LIVE_WIRE_HPP */

