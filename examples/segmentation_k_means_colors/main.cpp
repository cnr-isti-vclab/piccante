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

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img_str;

    if(argc == 2) {
        img_str = argv[1];
    } else {
        img_str = "../data/input/tommaseo_statue.png";
    }

    printf("Reading an image...");
    pic::Image img;
    img.Read(img_str);

    printf("Ok\n");

    if(img.isValid()) {
        int nSamples = 0;
        float *samples = img.getColorSamples(NULL, nSamples, 0.125f);

        std::vector< std::set<pic::uint> *> labels;
        int channels = img.channels;
        pic::uint k;
        float *centers = pic::KMeans<float>::select(samples, nSamples, channels, labels, k, 0.01f, 100);

        printf("The number of k is: %d\n", k);

        if(centers != NULL) {
            printf("The number of k is: %d\n", k);

            int n = img.size();

            for(int i = 0; i < n; i+= channels) {
                float *data_i = &img.data[i];
                float *data_c = NULL;
                float dist = FLT_MAX;

                for(pic::uint j = 0; j < k; j++) {
                    float *data_j = &centers[j * channels];
                    float tmp_dist = pic::Arrayf::distanceSq(data_i, data_j, channels);

                    if(tmp_dist < dist) {
                        dist = tmp_dist;
                        data_c = data_j;
                    }
                }

                pic::Arrayf::assign(data_c, channels, data_i);
            }

            bool bWrite = img.Write("../data/output/s_kmeans_colors.png");
            if(bWrite) {
                printf("s_kmeans_colors.png was written with success!\n");
            }
        }
    }

    return 0;
}
