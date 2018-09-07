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

#ifndef PIC_COMPUTER_VISION_SIMPLE_PLY_HPP
#define PIC_COMPUTER_VISION_SIMPLE_PLY_HPP

#include <vector>
#include <random>
#include <stdlib.h>

#include "../base.hpp"

#include "../util/math.hpp"

#include "../util/eigen_util.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Dense"
    #include "../externals/Eigen/SVD"
    #include "../externals/Eigen/Geometry"
#else
    #include <Eigen/Dense>
    #include <Eigen/SVD>
    #include <Eigen/Geometry>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

PIC_INLINE bool writeSimplePLY(std::string name,
                               std::vector< Eigen::Vector3d > &points_3d,
                               std::vector< unsigned char > &colors)
{
    //write a PLY file
    FILE *file = fopen(name.c_str(), "w");

    if (file == NULL) {
        return false;
    }

    bool bColor = colors.size() == (points_3d.size() * 3);

    fprintf(file,"ply\n");
    fprintf(file,"format ascii 1.0\n");
    fprintf(file,"element vertex %d\n", int(points_3d.size()));

    fprintf(file,"property float x\n");
    fprintf(file,"property float y\n");
    fprintf(file,"property float z\n");

    if(bColor) {
        fprintf(file,"property uchar red\n");
        fprintf(file,"property uchar green\n");
        fprintf(file,"property uchar blue\n");
        fprintf(file,"property uchar alpha\n");
    }
    fprintf(file,"end_header\n");

    for(unsigned int i = 0; i < points_3d.size(); i++) {
        //write position information
        fprintf(file, "%3.4f %3.4f %3.4f ", points_3d[i][0], points_3d[i][1], points_3d[i][2]);

        //write color information
        if(bColor) {
            auto k = i * 3;
            fprintf(file, " %d %d %d 255\n", colors[k], colors[k + 1], colors[k + 2]);
        }
    }

    fclose(file);
    return true;
}

#endif // PIC_DISABLE_EIGEN

} // end namespace pic

#endif // PIC_COMPUTER_VISION_SIMPLE_PLY_HPP
