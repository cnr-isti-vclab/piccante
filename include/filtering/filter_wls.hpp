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

#ifndef PIC_FILTERING_FILTER_WLS_HPP
#define PIC_FILTERING_FILTER_WLS_HPP

#include "../filtering/filter.hpp"

#ifndef PIC_DISABLE_EIGEN

#ifndef PIC_EIGEN_NOT_BUNDLED
    #include "../externals/Eigen/Sparse"
    #include "../externals/Eigen/src/SparseCore/SparseMatrix.h"
#else
    #include <Eigen/Sparse>
    #include <Eigen/src/SparseCore/SparseMatrix.h>
#endif

#endif

namespace pic {

#ifndef PIC_DISABLE_EIGEN

class FilterWLS: public Filter
{
protected:
    /**
     * @brief singleChannel applies WLS smoothing filter for gray-scale images.
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *singleChannel(ImageVec imgIn, Image *imgOut)
    {
        Image *L = imgIn[0];

        int width  = L->width;
        int height = L->height;
        int tot    = height * width;

        Eigen::VectorXd b, x;
        b = Eigen::VectorXd::Zero(tot);

        #ifdef PIC_DEBUG
            printf("Init matrix...");
        #endif

        std::vector< Eigen::Triplet< double > > tL;

        for(int i = 0; i < height; i++) {
            int tmpInd = i * width;

            for(int j = 0; j < width; j++) {

                float Ltmp, tmp;
                int indJ;
                int indI = tmpInd + j;
                float Lref = L->data[indI];

                b[indI] = Lref;

                float sum = 0.0f;

                if((i - 1) >= 0) {
                    indJ = indI - width;
                    Ltmp = L->data[indJ];
                    tmp  = -lambda / (powf(fabsf(Ltmp - Lref), alpha) + epsilon);
                    tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                    sum += tmp;
                }

                if((i + 1) < height) {
                    indJ = indI + width;
                    Ltmp = L->data[indJ];
                    tmp  = -lambda / (powf(fabsf(Ltmp - Lref), alpha) + epsilon);
                    tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                    sum += tmp;
                }

                if((j - 1) >= 0) {
                    indJ = indI - 1;
                    Ltmp = L->data[indJ];
                    tmp  = -lambda / (powf(fabsf(Ltmp - Lref), alpha) + epsilon);
                    tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                    sum += tmp;
                }

                if((j + 1) < width) {
                    indJ = indI + 1;
                    Ltmp = L->data[indJ];
                    tmp  = -lambda / (powf(fabsf(Ltmp - Lref), alpha) + epsilon);
                    tL.push_back(Eigen::Triplet< double > (indI, indJ, tmp));
                    sum += tmp;
                }

                tL.push_back(Eigen::Triplet< double > (indI, indI, 1.0f - sum));
            }
        }

        #ifdef PIC_DEBUG
            printf("Ok\n");
        #endif

        Eigen::SparseMatrix<double> A = Eigen::SparseMatrix<double>(tot, tot);
        A.setFromTriplets(tL.begin(), tL.end());

        Eigen::SimplicialCholesky<Eigen::SparseMatrix<double> > solver(A);
        x = solver.solve(b);

        if(solver.info() != Eigen::Success) {
            #ifdef PIC_DEBUG
                printf("SOLVER FAILED!\n");
            #endif
            return NULL;
        }

        #ifdef PIC_DEBUG
            printf("SOLVER SUCCESS!\n");
        #endif

        #pragma omp parallel for

        for(int i = 0; i < tot; i++) {
            imgOut->data[i] = float(x(i));
        }

        return imgOut;
    }

    /**
     * @brief multiChannel applies WLS filter for color images.
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *multiChannel(ImageVec imgIn, Image *imgOut)
    {
        Image *img = imgIn[0];

        int width  = img->width;
        int height = img->height;
        int tot    = height * width;

        alpha /= 2.0f;

        int stridex = width * img->channels;

        #ifdef PIC_DEBUG
            printf("Init matrix...");
        #endif

        std::vector< Eigen::Triplet< double > > tL;

        for(int i = 0; i < height; i++) {
            int tmpInd = i * width;

            for(int j = 0; j < width; j++) {

                float sum = 0.0f;
                float tmp;
                int indJ;
                int indI = tmpInd + j;
                int indImg = indI * img->channels;

                if((i - 1) >= 0) {
                    indJ = indImg - stridex;
                    float diff = 0.0f;

                    for(int p = 0; p < img->channels; p++) {
                        float tmpDiff = img->data[indJ + p] - img->data[indImg + p];
                        diff += tmpDiff * tmpDiff;
                    }

                    tmp  = -lambda / (powf(diff, alpha) + epsilon);

                    tL.push_back(Eigen::Triplet< double > (indI, indI - width , tmp));

                    sum += tmp;
                }

                if((i + 1) < height) {
                    indJ = indImg + stridex;
                    float diff = 0.0f;

                    for(int p = 0; p < img->channels; p++) {
                        float tmpDiff = img->data[indJ + p] - img->data[indImg + p];
                        diff += tmpDiff * tmpDiff;
                    }

                    tmp  = -lambda / (powf(diff, alpha) + epsilon);
                    tL.push_back(Eigen::Triplet< double > (indI, indI + width , tmp));
                    sum += tmp;
                }

                if((j - 1) >= 0) {
                    indJ = indImg - img->channels;
                    float diff = 0.0f;

                    for(int p = 0; p < img->channels; p++) {
                        float tmpDiff = img->data[indJ + p] - img->data[indImg + p];
                        diff += tmpDiff * tmpDiff;
                    }

                    tmp  = -lambda / (powf(diff, alpha) + epsilon);
                    tL.push_back(Eigen::Triplet< double > (indI, indI - 1 , tmp));
                    sum += tmp;
                }

                if((j + 1) < width) {
                    indJ = indImg + img->channels;
                    float diff = 0.0f;

                    for(int p = 0; p < img->channels; p++) {
                        float tmpDiff = img->data[indJ + p] - img->data[indImg + p];
                        diff += tmpDiff * tmpDiff;
                    }

                    tmp  = -lambda / (powf(diff, alpha) + epsilon);

                    tL.push_back(Eigen::Triplet< double > (indI, indI + 1 , tmp));
                    sum += tmp;
                }

                tL.push_back(Eigen::Triplet< double > (indI, indI, 1.0f - sum));
            }
        }

        #ifdef PIC_DEBUG
            printf("Ok\n");
        #endif

        Eigen::SparseMatrix<double> A = Eigen::SparseMatrix<double>(tot, tot);

        A.setFromTriplets(tL.begin(), tL.end());

        Eigen::SimplicialCholesky< Eigen::SparseMatrix< double > > solver(A);

        for(int i = 0; i < imgOut->channels; i++) {
            Eigen::VectorXd b, x;

            b = Eigen::VectorXd::Zero(tot);
            #pragma omp parallel for

            for(int j = 0; j < tot; j++) {
                b[j] = img->data[j * img->channels + i];
            }

            x = solver.solve(b);

            if(solver.info() == Eigen::Success) {

                #ifdef PIC_DEBUG
                    printf("SOLVER SUCCESS!\n");
                #endif

                #pragma omp parallel for

                for(int j = 0; j < tot; j++) {
                    imgOut->data[j * imgOut->channels + i] = float(x(j));
                }
            } else {
                #ifdef PIC_DEBUG
                    printf("SOLVER FAILED!\n");
                #endif
            }

        }

        return imgOut;
    }

    float alpha, lambda, epsilon;

public:

    /**
     * @brief FilterWLS
     */
    FilterWLS() : Filter()
    {
        update(1.2f, 1.0f);
    }

    /**
     * @brief FilterWLS
     * @param alpha
     * @param lambda
     */
    FilterWLS(float alpha, float lambda) : Filter()
    {
        update(alpha, lambda);
    }

    /**
     * @brief update
     * @param alpha
     * @param lambda
     */
    void update(float alpha, float lambda)
    {
        epsilon = 0.0001f;

        if(alpha <= 0.0f) {
            alpha = 1.2f;
        }

        if(lambda <= 0.0f) {
            lambda = 1.0f;
        }

        this->alpha = alpha;
        this->lambda = lambda;
    }

    /**
     * @brief Process
     * @param imgIn
     * @param imgOut
     * @return
     */
    Image *Process(ImageVec imgIn, Image *imgOut)
    {
        if(imgIn.empty()){
            return imgOut;
        }

        if(imgIn[0] == NULL) {
            return imgOut;
        }

        imgOut = setupAux(imgIn, imgOut);

        if(imgOut == NULL) {
            return imgOut;
        }

        //convolution
        if(imgIn[0]->channels == 1) {
            return singleChannel(imgIn, imgOut);
        } else {
            return multiChannel(imgIn, imgOut);
        }
    }

    /**
     * @brief main
     * @param argc
     * @param argv
     * @return
     */
    static int main(int argc, char* argv[])
    {
        if(argc < 4) {
            printf("Usage: name_input alpha lambad\n");
            return 0;
        }

        std::string nameIn = argv[1];
        std::string name = removeExtension(nameIn);
        std::string ext = getExtension(nameIn);

        float alpha = float(atof(argv[2]));
        float lambda = float(atof(argv[3]));

        std::string nameOut = name + "_wls." + ext; 

        Image img(nameIn);

        FilterWLS *filter = new FilterWLS(alpha, lambda);

        filter->Process(Single(&img), NULL)->Write(nameOut);

        return 0;
    }
};
#endif

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_WLS_HPP */

