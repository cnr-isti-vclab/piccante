/*

PICCANTE Examples
The hottest examples of Piccante:
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3.0 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See the GNU Lesser General Public License
    ( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.
*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#define EIGEN_DONT_VECTORIZE

#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include "piccante.hpp"

int main(int argc, char *argv[])
{   
    printf("Reading an LDR images...");

    double focal_length;
    double sensor_size_x_mm, sensor_size_y_mm;
    double sensor_pixel_resolution_x, sensor_pixel_resolution_y;
    std::string name0, name1;

    if(argc == 8) {
        name0 = argv[1];
        name1 = argv[2];
        focal_length = atof(argv[3]);
        sensor_size_x_mm = atof(argv[4]);
        sensor_size_y_mm = atof(argv[5]);
        sensor_pixel_resolution_x = atof(argv[6]);
        sensor_pixel_resolution_y = atof(argv[7]);
    } else {
        name0 = "../data/input/triangulation/campo_s_stefano_l.jpg";
        name1 = "../data/input/triangulation/campo_s_stefano_r.jpg";
        focal_length = 18.0;
        sensor_size_x_mm = 22.3;
        sensor_size_y_mm = 14.9;
        sensor_pixel_resolution_x = 2592.0;
        sensor_pixel_resolution_y = 1728.0;
    }
    
    //estimate K matrix from camera manufacturer's data
    double fx = pic::getFocalLengthPixels(focal_length, sensor_size_x_mm, sensor_pixel_resolution_x);
    double fy = pic::getFocalLengthPixels(focal_length, sensor_size_y_mm, sensor_pixel_resolution_y);
    Eigen::Matrix3d K = pic::getIntrinsicsMatrix(fx, fy,
                                                 sensor_pixel_resolution_x / 2.0,
                                                 sensor_pixel_resolution_y / 2.0);
    
    pic::Image img0, img1;
    img0.Read(name0, pic::LT_NOR);
    img1.Read(name1, pic::LT_NOR);
    
    printf("Ok\n");
    
    printf("Are they both valid? ");
    if(img0.isValid() && img1.isValid()) {
        printf("OK\n");
        
        std::vector< Eigen::Vector2f > m0, m1;
        std::vector< unsigned int > inliers;

        auto F = pic::estimateFundamentalFromImages(&img0, &img1, m0, m1, inliers);
                
        //decompose E into R and t
        std::vector< Eigen::Vector2f > m0f, m1f;
        pic::filterInliers(m0, inliers, m0f);
        pic::filterInliers(m1, inliers, m1f);

        //compute essential matrix decomposition
        Eigen::Matrix3d E = pic::computeEssentialMatrix(F, K);

        Eigen::Matrix3d R;
        Eigen::Vector3d t;
        pic::decomposeEssentialMatrixWithConfiguration(E, K, K, m0f, m1f, R, t);
        
        //triangulation                
        Eigen::Matrix34d M0 = pic::getCameraMatrixIdentity(K);
        Eigen::Matrix34d M1 = pic::getCameraMatrix(K, R, t);
        
        printf("Camera Matrix0:\n");
        pic::printfMat34d(M0);

        printf("Camera Matrix1:\n");
        pic::printfMat34d(M1);
    } else {
        printf("No there is at least an invalid file!\n");
    }
    
    return 0;
}
