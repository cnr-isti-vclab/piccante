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

#ifndef PIC_ALGORITHMS_CONNECTED_COMPONENTS_HPP
#define PIC_ALGORITHMS_CONNECTED_COMPONENTS_HPP

#include <vector>
#include <set>
#include <map>

#include "image.hpp"
#include "filtering/filter_luminance.hpp"

namespace pic {

//Connected components on a single channel image
typedef std::vector<int> ConnectComp;

struct LabelInfo {
    float id;
    float minLabel;

    friend bool operator<(LabelInfo const &a, LabelInfo const &b)
    {
        return a.id < b.id;
    }
};

class LabelOutput
{
public:
    float id;
    std::vector<int> coords;

    LabelOutput()
    {
    }

    LabelOutput(float id, int i)
    {
        this->id = id;
        coords.push_back(i);
    }

    void Add(int i)
    {
        coords.push_back(i);
    }

    friend bool operator<(LabelOutput const &a, LabelOutput const &b)
    {
        return a.id < b.id;
    }
};


/**
 * @brief ConnectedComponents computes connected components in an image
 * @param img
 * @param ret
 * @param comp
 * @param channel
 * @return
 */
Image *ConnectedComponents(Image *img, std::vector<LabelOutput> &ret,
                              Image *comp = NULL, float thr = 0.05f)
{
    //Check input paramters
    if(img == NULL) {
        return NULL;
    }

    float *data  = img->data;
    int width    = img->width;
    int height   = img->height;
    int channels = img->channels;
    int n = height * width;

    if(comp == NULL) {
        comp = new Image(1, width, height, 1);
    }

    comp->SetZero();

    //First pass: assigning basics labels
    // and generating the list of neighbors
    int label = 1;

    std::set<LabelInfo> labelEq;
    std::set<LabelInfo>::iterator it;

    for(int j = 0; j < height; j++) {
        int indY = j * width;

        for(int i = 0; i < width; i++) {
            int ind = (indY + i);

            int ind_img = ind * channels;

            //neighbors
            int neighbors[2];
            int nNeighbors = 0;

            if((i - 1) > -1) {
                int ind_img_prev = ind_img - channels;

                float n1 = Array<float>::norm(&data[ind_img], channels);
                float n2 = Array<float>::norm(&data[ind_img_prev], channels);
                float dist = sqrtf(Array<float>::distanceSq(&data[ind_img], &data[ind_img_prev], channels));

                if(dist <= (thr * MAX(n1, n2))) {
                    neighbors[0] = ind - 1;
                    nNeighbors++;
                }
            }

            if((j - 1) > -1) {
                int ind_img_prev = ind_img - (width * channels);

                float n1 = Array<float>::norm(&data[ind_img], channels);
                float n2 = Array<float>::norm(&data[ind_img_prev], channels);
                float dist = sqrtf(Array<float>::distanceSq(&data[ind_img], &data[ind_img_prev], channels));

                if(dist <= (thr * MAX(n1, n2))) {
                    neighbors[nNeighbors] = ind - width;
                    nNeighbors++;
                }
            }

            //No neighbors?
            if(nNeighbors == 0) {
                comp->data[ind] = float(label);
                label++;
            }

            if(nNeighbors == 1) {
                comp->data[ind] = comp->data[neighbors[0]];
            }

            if(nNeighbors == 2) {
                //Assigning the label of neighbors
                float minVal, t1, t2;
                t1 = comp->data[neighbors[0]];
                t2 = comp->data[neighbors[1]];
                minVal = MIN(t1, t2);

                //Tracking neighbors
                LabelInfo tmpLI;
                bool test = true;

                while(test) {
                    test = false;
                    tmpLI.id = minVal;
                    it = labelEq.find(tmpLI);

                    if(it != labelEq.end()) {
                        float tmpMinLabel = it->minLabel;

                        if(minVal > tmpMinLabel) {
                            minVal = tmpMinLabel;
                            test = true;
                        }
                    }
                }

                comp->data[ind] = minVal;

                //Tracking T1
                test = true;
                tmpLI.id = t1;
                it = labelEq.find(tmpLI);

                if(it != labelEq.end()) {
                    LabelInfo tmp_it;
                    tmp_it.id = it->id;
                    tmp_it.minLabel = minVal;

                    labelEq.erase(it);
                    labelEq.insert(tmp_it);
                } else {
                    LabelInfo tmpLabelInfo;
                    tmpLabelInfo.id = t1;
                    tmpLabelInfo.minLabel = minVal;
                    labelEq.insert(tmpLabelInfo);
                }

                //T2
                tmpLI.id = t2;
                it = labelEq.find(tmpLI);

                if(it != labelEq.end()) {
                    LabelInfo tmp_it;
                    tmp_it.id = it->id;
                    tmp_it.minLabel = minVal;

                    labelEq.erase(it);
                    labelEq.insert(tmp_it);
                } else {
                    LabelInfo tmpLabelInfo;
                    tmpLabelInfo.id = t2;
                    tmpLabelInfo.minLabel = minVal;
                    labelEq.insert(tmpLabelInfo);
                }
            }
        }
    }

    //Label Search
    LabelInfo tmpLI;
    std::set<LabelInfo>::iterator it2;

    std::set<LabelInfo> labelEq_new;

    for(it2 = labelEq.begin() ; it2 != labelEq.end(); it2++) {
        float minVal = it2->minLabel;
        bool test = true;

        while(test) {
            test = false;
            tmpLI.id = minVal;
            it = labelEq.find(tmpLI);

            if(it != labelEq.end()) {
                float tmpMinLabel = (*it).minLabel;

                if(minVal > tmpMinLabel) {
                    minVal = tmpMinLabel;
                    test = true;
                }
            }
        }

        LabelInfo tmp_it;
        tmp_it.id = it2->id;
        tmp_it.minLabel = minVal;
        labelEq_new.insert(tmp_it);
    }

    //Second pass: using tracked neighbors
    //for assigning the correct labels
    //TO DO: optimizing outside this loop
    std::set<float> unique;
    std::set<float>::iterator uniqueIt;
    std::map<float, int> mapping;

    int counter = 0;

    for(int i=0; i < n; i++) {
        tmpLI.id = comp->data[i];
        it = labelEq_new.find(tmpLI);

        if(it != labelEq_new.end()) {
            comp->data[i] = it->minLabel;
        }

        //Storing coordiantes of the connected components
        float id = comp->data[i];
        uniqueIt = unique.find(id);

        if(uniqueIt != unique.end()) {
            ret[mapping[id]].Add(i);
        } else {
            std::pair<float, int> tmp = std::make_pair(id, counter);
            mapping.insert(tmp);

            LabelOutput tmpRet(id, i);
            ret.push_back(tmpRet);

            unique.insert(id);
            counter++;
        }
    }
    return comp;
}

} // end namespace pic

#endif /* PIC_ALGORITHMS_CONNECTED_COMPONENTS_HPP */

