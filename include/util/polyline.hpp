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

#ifndef PIC_UTIL_POLYLINE_HPP
#define PIC_UTIL_POLYLINE_HPP

#include <vector>

#include "../base.hpp"

#include "../util/vec.hpp"

#ifndef PIC_DISABLE_EIGEN
#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif
#endif

namespace pic {

/**
 * @brief The Polyline class
 */
template<unsigned int N, class T>
class Polyline
{
public:

    std::vector< Vec<N, T> > points;

    Polyline()
    {
        points.clear();
    }

    Polyline(std::vector< Vec<N, T> > &points)
    {
        this->points.assign(points.begin(), points.end());
    }

    /**
     * @brief add
     * @param point
     */
    void add(Vec<N, T> &point)
    {
        points.push_back(point);
    }

    /**
     * @brief simplify
     * @param target_n_points
     */
    void simplify(int target_n_points)
    {
        int n = int(points.size());

        if(n <= target_n_points) {
            return;
        }

        while(n > target_n_points) {
            float area_min = FLT_MAX;
            int index = -1;
            float a, b, c, area_tmp;
            for(int i = 0; i < (n - 2); i++) {

                auto p0 = points.at(i    );
                auto p1 = points.at(i + 1);
                auto p2 = points.at(i + 2);

                a = sqrtf(float(p0.distanceSq(p1)));
                b = sqrtf(float(p1.distanceSq(p2)));
                c = sqrtf(float(p2.distanceSq(p0)));

                float s = (a + b + c) / 2.0f ;

                float area_sq = s * (s - a) * (s - b) * (s - c);
                area_tmp = sqrtf(area_sq);

                if(area_tmp < area_min) {
                    area_min = area_tmp;
                    index = i;
                }
            }

            if(index > -1) {
                points.erase(points.begin() + index + 1);
            } else {

                break;
            }

            n = int(points.size());
        }
    }
};

/**
 * @brief Polyline2i
 */
typedef Polyline<2, int> Polyline2i;

#ifndef PIC_DISABLE_EIGEN
/**
 * @brief convertFromEigenToPolyLine
 * @param in
 * @param out
 */
PIC_INLINE void convertFromEigenToPolyLine(std::vector< Eigen::Vector2i > &in, Polyline2i &out)
{
    for(unsigned int i = 0; i < in.size(); i++) {
        auto tmp = Vec2i(in[i][0], in[i][1]);
        out.add(tmp);
    }
}
#endif

} // end namespace pic

#endif //PIC_UTIL_POLYLINE_HPP
