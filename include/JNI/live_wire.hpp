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

#ifndef PIC_JNI_LIVE_WIRE_HPP
#define PIC_JNI_LIVE_WIRE_HPP

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
#include "../util/polyline.hpp"

#include "../algorithms/live_wire.hpp"

namespace pic {

/**
 * @brief executeLiveWireMultipleJNI
 * @param imageInPath
 * @param controPoints
 * @param bDownsample
 * @return
 */
PIC_INLINE std::vector< int > executeLiveWireMultipleJNI(std::string imageInPath, std::vector< int > controlPoints, bool bDownsample)
{
    std::vector< int > out;

    Image in;
    bool bRead = in.Read(imageInPath, LT_NOR_GAMMA);

    if(bRead) {
        LiveWire *lw ;
        Image *in_sub = NULL;

        if(bDownsample) {
            ImageSamplerBilinear isb;
            in_sub = FilterSampler2D::execute(&in, NULL, 0.25f, &isb);
            lw = new LiveWire(in_sub);
        } else {
            lw = new LiveWire(&in);
        }

        std::vector< Vec2i > out_tmp;

        int n = int(controlPoints.size()) >> 1;
        for(auto i = 0; i < (n - 1); i++) {
            int indexS = i << 1;
            int indexE = (i + 1) << 1;
            Vec2i pS(controlPoints[indexS], controlPoints[indexS + 1]);
            Vec2i pE(controlPoints[indexE], controlPoints[indexE + 1]);

            if(bDownsample) {
                pS[0] = pS[0] >> 2;
                pS[1] = pS[1] >> 2;

                pE[0] = pE[0] >> 2;
                pE[1] = pE[1] >> 2;
            }

            lw->execute(pS, pE, out_tmp, true, true);
        }


        Polyline2i pl(out_tmp);
        pl.simplify(32);

        for(uint i = 0; i < pl.points.size(); i++) {
            auto point = pl.points.at(i);

            if(bDownsample) {
                point[0] = point[0] << 2;
                point[1] = point[1] << 2;
            }

            out.push_back(point[0]);
            out.push_back(point[1]);
        }

        delete lw;
    }

    return out;
}

} // end namespace pic

#endif /* PIC_JNI_LIVE_WIRE_HPP */

