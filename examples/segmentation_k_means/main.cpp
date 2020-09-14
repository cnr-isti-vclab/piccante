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
    int n;

    if(argc == 2) {
        n = atoi(argv[1]);
    } else {
        n = 100000;
    }
    std::mt19937 m;

    int nDim = 2;
    float *points = new float[nDim * n];
    for(int i = 0; i< n; i++) {
        pic::Vec<2, float> tmp = pic::randomPoint<2>(&m);
        points[i * 2    ] = tmp[0];
        points[i * 2 + 1] = tmp[1];
    }

    std::vector< std::set<unsigned int> *> labels;

    int k = 3;

    pic::KMeans<float> km(k, 100);

    float *centers = km.execute(points, n, nDim, NULL, labels);

//     = pic::kMeans<float>(points, n, nDim, k, NULL, labels, 100);

    if(centers != NULL) {
        printf("K-Means ok!\n");

        pic::Image img(512, 512, 3);

        float colors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        for(unsigned int i = 0; i < labels.size(); i++) {

            float r = colors[i * 3    ];
            float g = colors[i * 3 + 1];
            float b = colors[i * 3 + 2];

            printf("Label size: %ld\n", labels[i]->size());

            for (std::set<unsigned int>::iterator it=labels[i]->begin(); it!=labels[i]->end(); it++) {
                unsigned int index = *it;
                int x = points[index * 2    ] * 255 + 256;
                int y = points[index * 2 + 1] * 255 + 256;
                float *data = img(x, y);
                data[0] = r;
                data[1] = g;
                data[2] = b;
            }
        }

        img.Write("../data/output/s_kmeans.png");
    }

    return 0;
}
