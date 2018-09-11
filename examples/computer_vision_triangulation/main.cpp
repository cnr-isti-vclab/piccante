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
    std::string name0, name1;
    double focal_length;
    double sensor_x_mm, sensor_y_mm;
    
    //estimating K matrix from camera
    if(argc == 3) {
        name0 = argv[1];
        name1 = argv[2];
        focal_length = atof(argv[3]);
        sensor_x_mm = atof(argv[4]);
        sensor_y_mm = atof(argv[5]);
    } else {
        name0 = "../data/input/triangulation/campo_s_stefano_l.jpg";
        name1 = "../data/input/triangulation/campo_s_stefano_r.jpg";
        focal_length = 18.0;
        sensor_x_mm = 22.3;
        sensor_y_mm = 14.9;
    }

    printf("Reading an LDR images...");
    pic::Image img0, img1;
    img0.Read(name0, pic::LT_NOR);
    img1.Read(name1, pic::LT_NOR);
    
    printf("Ok\n");
    
    printf("Are they both valid? ");
    if(img0.isValid() && img1.isValid()) {        
        printf("OK\n");

        auto fx0 = pic::getFocalLengthPixels(focal_length, sensor_x_mm, img0.widthf);
        auto fy0 = pic::getFocalLengthPixels(focal_length, sensor_y_mm, img0.heightf);
        Eigen::Matrix3d K0 = pic::getIntrinsicsMatrix(fx0, fy0, img0.widthf / 2.0, img0.heightf / 2.0);

        auto fx1 = pic::getFocalLengthPixels(focal_length, sensor_x_mm, img1.widthf);
        auto fy1 = pic::getFocalLengthPixels(focal_length, sensor_y_mm, img1.heightf);
        Eigen::Matrix3d K1 = pic::getIntrinsicsMatrix(fx1, fy1, img1.widthf / 2.0, img1.heightf / 2.0);

        //compute fundamental matrix
        std::vector< Eigen::Vector2f > m0, m1;
        std::vector< unsigned int > inliers;
        auto F = pic::estimateFundamentalFromImages(&img0, &img1, m0, m1, inliers);
        
        printf("\nFoundamental matrix: \n");
        pic::MatrixConvert(F).print();
        
        //compute essential matrix decomposition
        Eigen::Matrix3d E = pic::computeEssentialMatrix(F, K0, K1);
                
        //decompose E into R and t
        std::vector< Eigen::Vector2f > m0f, m1f;
        pic::filterInliers(m0, inliers, m0f);
        pic::filterInliers(m1, inliers, m1f);

        Eigen::Matrix3d R;
        Eigen::Vector3d t;
        pic::decomposeEssentialMatrixWithConfiguration(E, K0, K1, m0f, m1f, R, t);
        
        //triangulation        
        std::vector<Eigen::Vector3d> points_3d;
        
        Eigen::Matrix34d M0 = pic::getCameraMatrixIdentity(K0);
        Eigen::Matrix34d M1 = pic::getCameraMatrix(K1, R, t);
        
        printf("Camera Matrix0:\n");
        pic::printfMat34d(M0);

        printf("Camera Matrix1:\n");
        pic::printfMat34d(M1);
        printf("\n");

        std::vector< unsigned char> colors;
        pic::triangulationPoints(M0, M1, m0f, m1f, points_3d, colors, &img0, &img1, true);

        pic::writeSimplePLY("../data/output/triangulation.ply", points_3d, colors);

        //compute distortion parameters
        pic::NelderMeadOptRadialDistortion nmRD(M0, M1, &m0f, &m1f, &points_3d);
        
        float lambda = 0.0f;
        float lambda_out;
        nmRD.run(&lambda, 1, 1e-12f, 100000, &lambda_out);
        printf("Radial distortion lambda: %f\n", lambda_out);
        
        //error images
        pic::Image imgOut0(1, img0.width, img0.height, 3);
        imgOut0.setZero();

        pic::Image imgOut1(1, img1.width, img1.height, 3);
        imgOut1.setZero();

        double cx0 = img0.widthf / 2.0;
        double cy0 = img0.heightf / 2.0;

        double cx1 = img1.widthf / 2.0;
        double cy1 = img1.heightf / 2.0;

        for(unsigned int i = 0; i < m0f.size(); i++) {
            //first image
            Eigen::Vector2i proj0 = pic::cameraMatrixProjection(M0, points_3d[i], cx0, cy0, fx0, fy0, lambda_out);
            float *tmp;
            
            tmp = imgOut0(int(m0f[i][0]), int(m0f[i][1]));
            tmp[1] = 1.0f;
            
            tmp = imgOut0(proj0[0], proj0[1]);
            tmp[0] = 1.0f;
            
            //second image
            Eigen::Vector2i proj1 = pic::cameraMatrixProjection(M1, points_3d[i], cx1, cy1, fx1, fy1, lambda_out);
            
            tmp = imgOut1(int(m1f[i][0]), int(m1f[i][1]));
            tmp[1] = 1.0f;
            
            tmp = imgOut1(proj1[0], proj1[1]);
            tmp[0] = 1.0f;
        }
        
        //write reprojection images
        imgOut0.Write("../data/output/triangulation_reprojection_l.png", pic::LT_NOR);
        imgOut1.Write("../data/output/triangulation_reprojection_r.png", pic::LT_NOR);

    } else {
        printf("No there is at least an invalid file!\n");
    }
    
    return 0;
}
