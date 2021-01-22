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

#ifndef PIC_COMPUTER_VISION_NELDER_MEAD_OPT_GORDON_LOWE_HPP
#define PIC_COMPUTER_VISION_NELDER_MEAD_OPT_GORDON_LOWE_HPP

#include "../util/matrix_3_x_3.hpp"
#include "../util/nelder_mead_opt_base.hpp"
#include "../computer_vision/camera_matrix.hpp"

#ifndef PIC_DISABLE_EIGEN
#ifndef PIC_EIGEN_NOT_BUNDLED
   #include "../externals/Eigen/Dense"
#else
    #include <Eigen/Dense>
#endif
#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

#define GL_PACKED_CAMERA_SIZE 11
#define GL_3D_POINT_SIZE 3

class NelderMeadOptGordonLowe: public NelderMeadOptBase<double>
{
public:
    std::vector< std::vector< Vec<2, float> > > m;

    /**
     * @brief NelderMeadOptGordonLowe
     * @param m0
     * @param m1
     */
    NelderMeadOptGordonLowe(std::vector< Vec<2, float> > m0, std::vector< Vec<2, float> > m1) : NelderMeadOptBase()
    {
        this->m.push_back(m0);
        this->m.push_back(m1);
    }    

    static Eigen::Matrix34d parseCameraMatrix(float *x, unsigned int index)
    {
        Eigen::Matrix3d K, R;
        Eigen::Vector3d t;

        //offset of the matrix
        unsigned int c = index * 11;

        Eigen::Quaternion<double> reg;
        reg.x() = x[c]; c++;
        reg.y() = x[c]; c++;
        reg.z() = x[c]; c++;
        reg.w() = x[c]; c++;
        R = reg.toRotationMatrix();

        t[0] = x[c]; c++;
        t[1] = x[c]; c++;
        t[2] = x[c]; c++;

        K.setZero();
        K(0, 0) = x[c]; c++;
        K(1, 1) = x[c]; c++;
        K(0, 2) = x[c]; c++;
        K(1, 2) = x[c];
        K(2, 2) = 1.0;

        return getCameraMatrix(K, R, t);
    }

    /**
     * @brief ProjectionError
     * @param x
     * @param index
     * @return
     */
    double ProjectionError(float *x, unsigned int index) {

       double err = 0.0;

       Eigen::Matrix34d P = parseCameraMatrix(x, index);

       //offset of vertices
       int c = GL_PACKED_CAMERA_SIZE * int(m.size());

       Eigen::Vector4d point;
       for(uint i = 0; i < m[index].size(); i++) {
           point = Eigen::Vector4d(x[c], x[c + 1], x[c + 2], 1.0);

           Eigen::Vector3d point_proj = P * point;
           point_proj /= point_proj[2];

           double dx = point_proj[0] - m[index][i][0];
           double dy = point_proj[1] - m[index][i][1];

           err += dx * dx + dy * dy;

           c += 3;
       }


       return err;
    }

    /**
     * @brief function
     * @param x
     * @param n
     * @return
     */
    float function(float *x, unsigned int n)
    {       
        int n2 = int(m.size() * m[0].size());
        double err = sqrt((ProjectionError(x, 0) + ProjectionError(x, 1)) / double(n2));

        return float(err);
    }


    /**
     * @brief init3DPoints
     * @param K
     * @param m
     * @param x
     * @param distance
     */
    static void init3DPoints(Eigen::Matrix3d K, std::vector< Vec<2, float> > &m, std::vector< Eigen::Vector3d > &x, float distance = 20.0f)
    {
        Eigen::Matrix3d K_inv = K.inverse();
    \
        printf("Points: %zd\n", m.size());

        for(unsigned int i = 0; i < m.size(); i++) {
            Eigen::Vector3d point = Eigen::Vector3d (m[i][0], m[i][1], 1.0);

            point = K_inv * point;

            point *= distance;

            x.push_back(point);
        }
    }

    /**
     * @brief prepareInputData
     * @param K
     * @param R
     * @param t
     * @param x
     * @param ret_size
     * @return
     */
    static double *prepareInputData(std::vector< Eigen::Matrix3d > &K, std::vector< Eigen::Matrix3d > &R, std::vector< Eigen::Vector3d > &t, std::vector< Eigen::Vector3d > &x, unsigned int &ret_size)
    {
        if(R.size() != t.size()) {
            return NULL;
        }

        if(x.empty()) {
            return NULL;
        }

        int n = int (R.size());
        ret_size = GL_PACKED_CAMERA_SIZE * n + GL_3D_POINT_SIZE * int(x.size());
        double *ret = new double[ret_size];

        int c = 0;
        for(int i = 0; i < n; i++) {

            Eigen::Quaternion<double> reg(R[i]);

            ret[c] = reg.x(); c++;
            ret[c] = reg.y(); c++;
            ret[c] = reg.z(); c++;
            ret[c] = reg.w(); c++;

            ret[c] = t[i][0]; c++;
            ret[c] = t[i][1]; c++;
            ret[c] = t[i][2]; c++;

            ret[c] = K[i](0, 0); c++;
            ret[c] = K[i](1, 1); c++;
            ret[c] = K[i](0, 2); c++;
            ret[c] = K[i](1, 2); c++;
        }

        for(size_t i = 0; i < x.size(); i++) {
            ret[c] = x[i][0]; c++;
            ret[c] = x[i][1]; c++;
            ret[c] = x[i][2]; c++;
        }

        return ret;
    }
};

#endif

}

#endif // PIC_COMPUTER_VISION_NELDER_MEAD_OPT_GORDON_LOWE_HPP
