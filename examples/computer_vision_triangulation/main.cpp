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

#include "../common_code/image_qimage_interop.hpp"

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    
    printf("Reading an LDR images...");
    
    //estimating K matrix from camera
    double fx = pic::getFocalLengthPixels(18.0, 22.3, 2592.0);
    double fy = pic::getFocalLengthPixels(18.0, 14.9, 1728.0);
    Eigen::Matrix3d K = pic::getIntrinsicsMatrix(fx, fy, 2592.0 / 2.0, 1728.0 / 2.0);
    
    pic::Image img0, img1;
    ImageRead("../data/input/triangulation/campo_s_stefano_l.jpg", &img0, pic::LT_NOR);
    ImageRead("../data/input/triangulation/campo_s_stefano_r.jpg", &img1, pic::LT_NOR);
    
    printf("Ok\n");
    
    printf("Are they both valid? ");
    if(img0.isValid() && img1.isValid()) {
        printf("OK\n");
        
        //output corners
        std::vector< Eigen::Vector3f > corners_from_img0;
        std::vector< Eigen::Vector3f > corners_from_img1;
        
        //compute the luminance images
        pic::Image *L0 = pic::FilterLuminance::Execute(&img0, NULL, pic::LT_CIE_LUMINANCE);
        pic::Image *L1 = pic::FilterLuminance::Execute(&img1, NULL, pic::LT_CIE_LUMINANCE);
        
        //get corners
        printf("Extracting corners...\n");
        pic::HarrisCornerDetector hcd(2.5f, 5);
        hcd.execute(L0, &corners_from_img0);
        hcd.execute(L1, &corners_from_img1);
        
        //compute ORB descriptors for each corner and image
        //apply a gaussian filter to luminance images
        pic::Image *L0_flt = pic::FilterGaussian2D::Execute(L0, NULL, 2.5f);
        pic::Image *L1_flt = pic::FilterGaussian2D::Execute(L1, NULL, 2.5f);
        
        printf("Computing ORB descriptors...\n");
        
        pic::ORBDescriptor b_desc(31, 512);
        
        std::vector< unsigned int *> descs0;
        b_desc.getAll(L0_flt, corners_from_img0, descs0);

        std::vector< unsigned int *> descs1;
        b_desc.getAll(L1_flt, corners_from_img1, descs1);

        printf("Matching ORB descriptors...\n");
        std::vector< Eigen::Vector3i > matches;
        
        int n = b_desc.getDescriptorSize();
        
        printf("Descriptor size: %d\n", n);
        
        //pic::BinaryFeatureBruteForceMatcher bffm_bin(&descs1, n);
        pic::BinaryFeatureLSHMatcher bffm_bin(&descs1, n, 64);
        
        bffm_bin.getAllMatches(descs0, matches);
        
        printf("Matches:\n");
        std::vector< Eigen::Vector2f > m0, m1;
        pic::BinaryFeatureMatcher::filterMatches(corners_from_img0, corners_from_img1, matches, m0, m1);
        printf("\n Total matches: (%lu | %lu)\n", m0.size(), m1.size());
        
        printf("\nEstimating the fundamental matrix F from the matches...");
        
        std::vector< unsigned int > inliers;
        Eigen::Matrix3d F = pic::estimateFundamentalWithNonLinearRefinement(m0, m1, inliers, 1000000, 0.5, 1, 10000, 1e-4f);
                
        printf("Ok.\n");
        
        printf("\nFoundamental matrix: \n");
        pic::MatrixConvert(F).print();
        
        //compute essential matrix decomposition
        Eigen::Matrix3d E = pic::computeEssentialMatrix(F, K);
                
        //decompose E into R and t
        std::vector< Eigen::Vector2f > m0f, m1f;
        pic::filterInliers(m0, inliers, m0f);
        pic::filterInliers(m1, inliers, m1f);

        Eigen::Matrix3d R;
        Eigen::Vector3d t;
        pic::decomposeEssentialMatrixWithConfiguration(E, K, K, m0f, m1f, R, t);
        
        //triangulation        
        std::vector<Eigen::Vector3d> points_3d;
        
        Eigen::Matrix34d M0 = pic::getCameraMatrixIdentity(K);
        Eigen::Matrix34d M1 = pic::getCameraMatrix(K, R, t);
        
        pic::printfMat34d(M0);
        pic::printfMat34d(M1);
        
        pic::NelderMeadOptTriangulation nmTri(M0, M1);
        for(unsigned int i = 0; i < m0f.size(); i++) {
            //normalized coordinates
            Eigen::Vector3d p0 = Eigen::Vector3d(m0f[i][0], m0f[i][1], 1.0);
            Eigen::Vector3d p1 = Eigen::Vector3d(m1f[i][0], m1f[i][1], 1.0);
            
            //triangulation
            Eigen::Vector4d point = pic::triangulationHartleySturm(p0, p1, M0, M1);
            
            //non-linear refinement
            nmTri.update(m0f[i], m1f[i]);
            float tmpp[] = {float(point[0]), float(point[1]), float(point[2])};
            float out[3];
            nmTri.run(tmpp, 3, 1e-9f, 10000, &out[0]);
            
            points_3d.push_back(Eigen::Vector3d(out[0], out[1], out[2]));
        }
        
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

        double cx = 2592.0 / 2.0;
        double cy = 1728.0 / 2.0;
        for(unsigned int i = 0; i < m0f.size(); i++) {
            //first image
            Eigen::Vector2i proj0 = pic::cameraMatrixProjection(M0, points_3d[i], cx, cy, fx, fy, lambda_out);
            float *tmp;
            
            tmp = imgOut0(int(m0f[i][0]), int(m0f[i][1]));
            tmp[1] = 1.0f;
            
            tmp = imgOut0(proj0[0], proj0[1]);
            tmp[0] = 1.0f;
            
            //second image
            Eigen::Vector2i proj1 = pic::cameraMatrixProjection(M1, points_3d[i], cx, cy, fx, fy, lambda_out);
            
            tmp = imgOut1(int(m1f[i][0]), int(m1f[i][1]));
            tmp[1] = 1.0f;
            
            tmp = imgOut1(proj1[0], proj1[1]);
            tmp[0] = 1.0f;
        }
        
        //write reprojection images
        ImageWrite(&imgOut0, "../data/output/triangulation_reprojection_l.png");
        ImageWrite(&imgOut1, "../data/output/triangulation_reprojection_r.png");
        
        //write a PLY file
        FILE *file = fopen("../data/output/triangulation_mesh.ply","w");
        
        if (file == NULL) {
            return 0;
        }
        
        fprintf(file,"ply\n");
        fprintf(file,"format ascii 1.0\n");
        fprintf(file,"element vertex %d\n", int(m0f.size()));
        
        fprintf(file,"property float x\n");
        fprintf(file,"property float y\n");
        fprintf(file,"property float z\n");
        
        fprintf(file,"property uchar red\n");
        fprintf(file,"property uchar green\n");
        fprintf(file,"property uchar blue\n");
        fprintf(file,"property uchar alpha\n");
        fprintf(file,"end_header\n");
        
        for(unsigned int i = 0; i < m0f.size(); i++) {
            fprintf(file, "%3.4f %3.4f %3.4f ", points_3d[i][0], points_3d[i][1], points_3d[i][2]);
            
            //write color information
            unsigned char r, g, b;
            float *color = img0(int(m0f[i][0]), int(m0f[i][1]));
            r = int(color[0] * 255.0f);
            g = int(color[1] * 255.0f);
            b = int(color[2] * 255.0f);
            fprintf(file, " %d %d %d 255\n", r, g, b);
        }
        
        fclose(file);
    } else {
        printf("No there is at least an invalid file!\n");
    }
    
    return 0;
}
