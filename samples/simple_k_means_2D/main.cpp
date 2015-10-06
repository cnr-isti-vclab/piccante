/*

PICCANTE
The hottest HDR imaging library!
http://piccantelib.net

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);


    std::vector< pic::Vec<2, float> > point;
    int n = 1000;

    std::mt19937 m;

    for(int i = 0; i< n; i++) {
        point.push_back(pic::randomPoint<2>(&m));
    }

    std::vector<unsigned int> centers;
    std::vector< std::set<unsigned int> *> labels;

    bool bRet = pic::kMeans(point, 3, 1000, centers, labels);

    if(bRet) {
        printf("K-Means ok!\n");

        pic::Image img(512, 512, 3);

        float colors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        for(unsigned int i = 0; i < labels.size(); i++) {

            float r = colors[i * 3    ];
            float g = colors[i * 3 + 1];
            float b = colors[i * 3 + 2];

            printf("Label size: %d\n", labels[i]->size());

            for (std::set<unsigned int>::iterator it=labels[i]->begin(); it!=labels[i]->end(); it++) {
                unsigned int index = *it;
                int x = point[index][0] * 255 + 256;
                int y = point[index][1] * 255 + 256;
                float *data = img(x, y);
                data[0] = r;
                data[1] = g;
                data[2] = b;
            }
        }
        img.Write("../data/output/kmeans.bmp");
    }

    return 0;
}
