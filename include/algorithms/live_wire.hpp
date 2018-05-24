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
#include "../filtering/filter_sampler_2d.hpp"
#include "../util/vec.hpp"

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
            D_p.div(sqrtf(n_D_p_sq));
        }

        //D_q
        tmp = (*img_G)(q[0], q[1]);
        Vec2f D_q(tmp[1], -tmp[0]);
        float n_D_q_sq = D_q.lengthSq();
        if(n_D_q_sq > 0.0f) {
            D_q.div(sqrtf(n_D_q_sq));
        }

        //Delta_qp
        Vec2f delta_qp(float(q[0] - p[0]), float(q[1] - p[1]));

        Vec2f L;
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

        float fD = (acosf(dp_pq) + acosf(dq_pq)) * fD_const;

        out +=  0.43f * fD;

        return out;
    }

    /**
     * @brief release
     */
    void release()
    {
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

        Image *img_L = FilterLuminance::Execute(img, NULL);

        //compute fG
        img_G = FilterGradient::Execute(img_L, img_G);
        fG_min = img_G->getMinVal(NULL, NULL);
        fG_max = img_G->getMaxVal(NULL, NULL);

        //compute fZ
        fZ = FilterLoG2D::Execute(img_L, fZ, 1.0f);
        fZ->applyFunction(f1minusx);

        //aux buffers
        g = img_L;

        e = new bool[img_L->nPixels()];

        pointers = new int[img_L->nPixels() << 1];
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

        *g = FLT_MAX;

        e = Buffer<bool>::assign(e, g->nPixels(), false);
        pointers = Buffer<int>::assign(pointers, g->nPixels() << 1, 0);

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
            e[q[1] * width + q[0]] = true;

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

                            int index = (r[1] * width + r[0]) << 1;
                            pointers[index    ] = q[0];
                            pointers[index + 1] = q[1];
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

        while(true) {
            prev = m;

            printf("point(%d, %d)\n", prev[0], prev[1]);
            if(m.equal(pS)) {
                break;
            }

            int index = (m[1] * width + m[0]) << 1;
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
    static void executeLiveWireMultiple(Image *in, std::vector< Vec2i > &controlPoint, std::vector< Vec2i > &out)
    {
        if(in != NULL) {
            pic::LiveWire *lw = new pic::LiveWire(in);

            for(auto i = 0; i < (controlPoint.size() - 1); i++) {
                lw->execute(controlPoint.at(i), controlPoint.at(i + 1), out, true, true);
            }

            delete lw;
        }
    }
};

/**
 * @brief executeLiveWireSingleJNI
 * @param imageInPath
 * @param pS
 * @param pE
 * @param bDownsample
 * @return
 */
PIC_INLINE std::vector< int > executeLiveWireSingleJNI(std::string imageInPath, int x0, int y0, int x1, int y1, bool bDownsample)
{
    std::vector< int > out;

    Image in;
    bool bRead = in.Read(imageInPath, LT_NOR_GAMMA);

    if(bRead) {
        pic::LiveWire *lw ;
        Image *in_sub = NULL;
        Vec2i pS(x0, y0);
        Vec2i pE(x1, y1);

        if(bDownsample) {
            ImageSamplerBilinear isb;
            in_sub = FilterSampler2D::Execute(&in, NULL, 0.25f, &isb);
            lw = new pic::LiveWire(in_sub);

            pS[0] = pS[0] >> 2;
            pS[1] = pS[1] >> 2;

            pE[0] = pE[0] >> 2;
            pE[1] = pE[1] >> 2;

        } else {
            lw = new pic::LiveWire(&in);
        }

        std::vector< Vec2i > out_tmp;

        lw->execute(pS, pE, out_tmp, true, false);

        for(auto i = 0; i < out_tmp.size(); i++) {
            auto point = out_tmp.at(i);

            int x = point[0];
            int y = point[1];

            if(bDownsample) {
                out.push_back(x << 2);
                out.push_back(y << 2);
            } else {
                out.push_back(x);
                out.push_back(y);
            }
        }

        delete lw;
    }

    return out;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_LIVE_WIRE_HPP */

