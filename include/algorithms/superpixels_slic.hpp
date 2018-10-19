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

#ifndef PIC_ALGORITHMS_SUPERPIXELS_SLIC_HPP
#define PIC_ALGORITHMS_SUPERPIXELS_SLIC_HPP

#include "../image.hpp"
#include "../filtering/filter_laplacian.hpp"

namespace pic {

/**
 * @brief The SlicoCenter struct
 */
struct SlicoCenter {
    float			*value;
    unsigned int	x, y;
};

/**
 * @brief The Slic class
 */
class Slic
{
protected:

    int             nSuperPixels;
    Image           *labels_distance, *lap_img;
    SlicoCenter     *centers;
    unsigned int    *prevX, *prevY, *counter;
    float           *col_values, *mPixel;
    int             width, height, channels;

    /**
     * @brief distanceC
     * @param a1
     * @param a2
     * @param channels
     * @return
     */
    inline float distanceC(float *a1, float *a2, int channels)
    {
        float acc = 0.0f;

        for(int i = 0; i < channels; i++) {
            float tmp = (a1[i] - a2[i]);
            acc += tmp * tmp;
        }

        return acc;
    }

    /**
     * @brief distanceS
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return
     */
    inline int distanceS(int x1, int y1, int x2, int y2)
    {
        int tx = x1 - x2;
        int ty = y1 - y2;
        return tx * tx + ty * ty;
    }

    /**
     * @brief pass
     * @param img
     * @param S
     * @return
     */
    bool pass(Image *img, int S)
    {
        float Sf = float(S);
        float Sf2 = Sf * Sf;

        for(int i = 0; i < nSuperPixels; i++) {
            prevX[i] = centers[i].x;
            prevY[i] = centers[i].y;
        }

        //for each cluster
        for(int i = 0; i < nSuperPixels; i++) {
            float i_f = float(i);

            //Search in Sx2 radius
            for(int j = -S; j < S; j++) {
                int vY = centers[i].y + j;

                for(int k = -S; k < S; k++) {
                    int vX = centers[i].x + k;

                    float *pixel = (*img)(vX, vY);
                    float *l_d   = (*labels_distance)(vX, vY);

                    float dS = float(distanceS(vX, vY, centers[i].x, centers[i].y)) / Sf2;
                    float dC = distanceC(pixel, centers[i].value, img->channels) / mPixel[i];
                    float D = dC + dS;

                    if(D < l_d[1]) {
                        l_d[0] = i_f;
                        l_d[1] = D;
                        l_d[2] = dC;
                    }
                }
            }
        }

        //update mPixel
        for(int i = 0; i < labels_distance->size(); i += labels_distance->channels) {
            int label = int(labels_distance->data[i]);

            if(label < 0) {
                continue;
            }

            if(mPixel[label] < labels_distance->data[i + 2]) {
                mPixel[label] = labels_distance->data[i + 2];
            }
        }

        //update clusters
        for(int i = 0; i < nSuperPixels; i++) {
            centers[i].x = 0;
            centers[i].y = 0;
            counter[i]   = 0;

            int ind = i * img->channels;

            for(int j = 0; j < img->channels; j++) {
                col_values[ind + j] = 0.0f;
            }
        }

        for(int j = 0; j < img->height; j++) {
            for(int k = 0; k < img->width; k++) {
                float *l_d = (*labels_distance)(k, j);
                int label = int(l_d[0]);

                if(label > -1) {
                    centers[label].x += k;
                    centers[label].y += j;
                    counter[label]++;

                    int ind = label * img->channels;
                    float *col = (*img)(k, j);

                    for(int p = 0; p < img->channels; p++) {
                        col_values[ind + p] += col[p];
                    }
                }
            }
        }

        float E = 0.0f;

        for(int i = 0; i < nSuperPixels; i++) {
            if(counter[i] <= 0) {
                continue;
            }

            centers[i].x /= counter[i];
            centers[i].y /= counter[i];
            int ind = i * img->channels;

            for(int j = 0; j < img->channels; j++) {
                centers[i].value[j] = col_values[ind + j] / float(counter[i]);
            }

            //Error
            int tx = prevX[i] - centers[i].x;
            int ty = prevY[i] - centers[i].y;
            float tmpErr = sqrtf(float(tx * tx + ty * ty));
            E += tmpErr;
        }

        return (E > (0.0001f * float(nSuperPixels)));
    }



    /**
     * @brief allocate
     * @param nSuperPixels
     * @param channels
     */
    void allocate(int nSuperPixels, int channels)
    {

        this->nSuperPixels = nSuperPixels;

        release();

        centers		= new SlicoCenter[nSuperPixels];
        prevX		= new unsigned int [nSuperPixels];
        prevY		= new unsigned int [nSuperPixels];
        counter		= new unsigned int [nSuperPixels];
        mPixel		= new float [nSuperPixels];
        col_values	= new float [nSuperPixels * channels];
    }

    /**
     * @brief release
     */
    void release()
    {
        if(lap_img != NULL) {
            delete lap_img;
        }

        if(labels_distance != NULL) {
            delete labels_distance;
        }

        if(centers != NULL) {
            delete[] centers;
        }

        if(prevX != NULL) {
            delete[] prevX;
        }

        if(prevY != NULL) {
            delete[] prevY;
        }

        if(counter != NULL) {
            delete[] counter;
        }

        if(col_values != NULL) {
            delete[] col_values;
        }

        if(mPixel != NULL) {
            delete[] mPixel;
        }
    }

public:

    /**
     * @brief Slic
     */
    Slic()
    {
        lap_img = NULL;
        labels_distance = NULL;
        centers = NULL;
        prevX = NULL;
        prevY = NULL;
        counter = NULL;
        col_values = NULL;
        mPixel = NULL;
    }

    /**
     * @brief Slic
     * @param img
     * @param nSuperPixels
     */
    Slic(Image *img, int nSuperPixels = 64)
    {
        lap_img = NULL;
        labels_distance = NULL;
        centers = NULL;
        prevX = NULL;
        prevY = NULL;
        counter = NULL;
        col_values = NULL;
        mPixel = NULL;

        execute(img, nSuperPixels);
    }

    ~Slic()
    {
        release();
    }
    
    /**
     * @brief execute
     * @param img
     * @param nSuperPixels
     */
    void execute(Image *img, int nSuperPixels = 64)
    {
        if(img == NULL) {
            return;
        }

        //Init
        int S = int(sqrtf(img->widthf * img->heightf) / float(nSuperPixels));

        if(S < 1) {
            return;
        }

        nSuperPixels = (img->width / S) * (img->height / S);

        allocate(nSuperPixels, img->channels);

        if(labels_distance == NULL) {
            labels_distance = new Image(1, img->width, img->height, 3);
        }

        for(int i = 0; i < labels_distance->size(); i += labels_distance->channels) {
            labels_distance->data[i    ] = -1.0f;
            labels_distance->data[i + 1] = FLT_MAX;
            labels_distance->data[i + 2] = FLT_MAX;
        }

        width = img->width;
        height = img->height;
        channels = img->channels;

        FilterLaplacian lap;
        lap_img = lap.Process(Single(img), lap_img);

        for(int i = 0; i < nSuperPixels; i++) {
            mPixel[i] = 0.35f * 0.35f;    //10.0f*10.0f;
        }

        #ifdef PIC_DEBUG
            printf("nSuperPixels: %d S: %d\n", nSuperPixels, S);
        #endif
        
        int ind = 0;
        int S_half = S >> 1;

        for(int i = S_half; i < (img->height - S_half + 1); i += S) {
            for(int j = S_half; j < (img->width - S_half + 1); j += S) {

                float bValue = FLT_MAX;
                int bX, bY;

                for(int y = -1; y <= 1; y++) {
                    for(int x = -1; x <= 1; x++) {
                        int ix = (j + x);
                        int iy = (i + y);
                        float *data = (*lap_img)(ix, iy);

                        float acc = 0.0f;

                        for(int c = 0; c < img->channels; c++) {
                            acc += fabsf(data[c]);
                        }

                        if(acc < bValue) {
                            bValue = acc;
                            bX = ix;
                            bY = iy;
                        }
                    }
                }

                centers[ind].x = bX;
                centers[ind].y = bY;
                centers[ind].value = new float [img->channels];

                BBox box(centers[ind].x - S_half, centers[ind].x + S_half + 1,
                         centers[ind].y - S_half, centers[ind].y + S_half + 1);

                img->getMeanVal(&box, centers[ind].value);
                ind++;
            }
        }

        //For each pass
        int iter = 0;
        bool bCheck = true;

        while(bCheck) {
            bCheck = pass(img, S);

            if(!bCheck && iter <= 10) {
                bCheck = true;
            }

            iter++;
        }

        #ifdef PIC_DEBUG
            printf("Iterations: %d\n", iter);
        #endif
    }

    /**
     * @brief getLabelsBuffer
     * @param out
     * @return
     */
    int *getLabelsBuffer(int *out = NULL)
    {
        if(labels_distance == NULL) {
            return NULL;
        }

        int size = labels_distance->width * labels_distance->height;

        if(size < 1) {
            return NULL;
        }

        if(out == NULL) {
            out = new int[size];
        }

        for(int i = 0; i < size; i++) {
            out[i] = int(labels_distance->data[i << 1]);
        }

        return out;
    }

    /**
     * @brief getMeanImage
     * @param imgOut
     * @return
     */
    Image *getMeanImage(Image *imgOut)
    {
        if(imgOut == NULL) {
            imgOut = new Image(1, width, height, channels);
        }

        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                float *pixel = (*imgOut)(j, i);
                float *l_d   = (*labels_distance)(j, i);

                int label = int(l_d[0]);

                if(label > -1) {
                    for(int k = 0; k < channels; k++) {
                        pixel[k] = centers[label].value[k];
                    }
                }
            }
        }

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_SUPERPIXELS_SLIC_HPP */

