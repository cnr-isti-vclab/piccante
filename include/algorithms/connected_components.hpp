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
#include <utility>

#include "../base.hpp"

#include "../image.hpp"

#include "../util/buffer.hpp"

namespace pic {

struct LabelInfo
{
    unsigned int id;
    unsigned int minLabel;

    friend bool operator<(LabelInfo const &a, LabelInfo const &b)
    {
        return a.id < b.id;
    }
};

class LabelOutput
{
public:
    unsigned int id;
    std::vector< int > coords;

    LabelOutput()
    {
    }

    LabelOutput(unsigned int id, int i)
    {
        this->id = id;
        coords.push_back(i);
    }

    void add(int i)
    {
        coords.push_back(i);
    }

    friend bool operator<(LabelOutput const &a, LabelOutput const &b)
    {
        return a.id < b.id;
    }
};

template<class T>
class ConnectedComponents
{
protected:
    float thr;

    /**
     * @brief secondPass
     * @param imgOut
     * @param labelEq
     */
    void secondPass(unsigned int *imgOut, std::vector<LabelOutput> &ret, std::set<LabelInfo> &labelEq, int n)
    {
        //Label Search
        LabelInfo tmpLI;
        std::set<LabelInfo> labelEq_new;

        for(auto it2 = labelEq.begin() ; it2 != labelEq.end(); it2++) {
            auto minVal = it2->minLabel;
            bool test = true;

            while(test) {
                test = false;
                tmpLI.id = minVal;
                auto it = labelEq.find(tmpLI);

                if(it != labelEq.end()) {
                    auto tmpMinLabel = (*it).minLabel;

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
        std::set<unsigned int> unique;
        //std::set<unsigned int>::iterator uniqueIt;
        std::map<unsigned int, int> mapping;

        int counter = 0;

        for(int i = 0; i < n; i++) {
            tmpLI.id = imgOut[i];
            auto it = labelEq_new.find(tmpLI);

            if(it != labelEq_new.end()) {
                imgOut[i] = it->minLabel;
            }

            //store coordiantes of the connected components
            auto id = imgOut[i];
            auto uniqueIt = unique.find(id);

            if(uniqueIt != unique.end()) {
                ret[mapping[id]].add(i);
            } else {
                std::pair<unsigned int, int> tmp = std::make_pair(id, counter);
                mapping.insert(tmp);

                LabelOutput tmpRet(id, i);
                ret.push_back(tmpRet);

                unique.insert(id);
                counter++;
            }
        }
    }

    void track(unsigned int *imgOut, int &label, std::set<LabelInfo> &labelEq,
               int neighbors[2], int nNeighbors, int ind)
    {
        std::set<LabelInfo>::iterator it;
        //No neighbors?
        if(nNeighbors == 0) {
            imgOut[ind] = label;
            label++;
        }

        if(nNeighbors == 1) {
            imgOut[ind] = imgOut[neighbors[0]];
        }

        if(nNeighbors == 2) {
            //Assign the label of neighbors
            unsigned int minVal, t1, t2;
            t1 = imgOut[neighbors[0]];
            t2 = imgOut[neighbors[1]];
            minVal = MIN(t1, t2);

            //Track neighbors
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

            imgOut[ind] = minVal;

            //Track T1
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

            //Track T2
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

public:

    /**
     * @brief ConnectedComponents
     * @param thr
     */
    ConnectedComponents(float thr = 0.05f)
    {
        this->thr  = thr > 0.0f ? thr : 0.05f;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param imgOut
     * @param ret
     */
    unsigned int *execute(Image *imgIn, unsigned int *imgOut, std::vector<LabelOutput> &ret)
    {
        //Check input paramters
        if(imgIn == NULL) {
            return imgOut;
        }

        float *data  = imgIn->data;
        int width    = imgIn->width;
        int height   = imgIn->height;
        int channels = imgIn->channels;

        int n = width * height;

        if(imgOut == NULL) {
            imgOut = new unsigned int[n];
        }

        Buffer<unsigned int>::assign(imgOut, n, 0);

        //First pass:
        // 1) assign basics labels
        // 2) generate the list of neighbors
        int label = 1;
        std::set<LabelInfo> labelEq;
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

                    float n1 = Arrayf::norm(&data[ind_img], channels);
                    float n2 = Arrayf::norm(&data[ind_img_prev], channels);
                    float dist = sqrtf(Arrayf::distanceSq(&data[ind_img], &data[ind_img_prev], channels));

                    if(dist <= (thr * MAX(n1, n2))) {
                        neighbors[0] = ind - 1;
                        nNeighbors++;
                    }
                }

                if((j - 1) > -1) {
                    int ind_img_prev = ind_img - (width * channels);

                    float n1 = Arrayf::norm(&data[ind_img], channels);
                    float n2 = Arrayf::norm(&data[ind_img_prev], channels);
                    float dist = sqrtf(Arrayf::distanceSq(&data[ind_img], &data[ind_img_prev], channels));

                    if(dist <= (thr * MAX(n1, n2))) {
                        neighbors[nNeighbors] = ind - width;
                        nNeighbors++;
                    }
                }

                track(imgOut, label, labelEq, neighbors, nNeighbors, ind);
            }
        }

        secondPass(imgOut, ret, labelEq, n);
        return imgOut;
    }

    /**
     * @brief execute
     * @param imgIn
     * @param width
     * @param height
     * @param imgOut
     * @param ret
     * @return
     */
    unsigned int *execute(T *imgIn, int width, int height, unsigned int *imgOut, std::vector<LabelOutput> &ret)
    {
        //Check input paramters
        if(imgIn == NULL) {
            return imgOut;
        }

        int n = width * height;

        if(imgOut == NULL) {
            imgOut = new unsigned int[n];
        }

        Buffer<unsigned int>::assign(imgOut, n, 0);

        T *data = imgIn;
        //First pass:
        // 1) assign basics labels
        // 2) generate the list of neighbors
        int label = 1;
        std::set<LabelInfo> labelEq;
        for(int j = 0; j < height; j++) {
            int indY = j * width;

            for(int i = 0; i < width; i++) {
                int ind = (indY + i);

                //neighbors
                int neighbors[2];
                int nNeighbors = 0;

                if((i - 1) > -1) {
                    int ind_prev = ind - 1;
                    if(data[ind] == data[ind_prev]) {
                        neighbors[0] = ind_prev;
                        nNeighbors++;
                    }
                }

                if((j - 1) > -1) {
                    int ind_prev = ind - width;
                    if(data[ind] == data[ind_prev]) {
                        neighbors[nNeighbors] = ind_prev;
                        nNeighbors++;
                    }
                }

                track(imgOut, label, labelEq, neighbors, nNeighbors, ind);
            }
        }

        secondPass(imgOut, ret, labelEq, n);
        return imgOut;
    }

    /**
     * @brief reCount
     * @param imgLabel
     * @param ret
     * @return
     */
    static unsigned int *reCount(unsigned int *imgLabel, std::vector<LabelOutput> &ret)
    {
        if(imgLabel == NULL) {
            return NULL;
        }

        for(unsigned int i = 0; i < ret.size(); i++) {
            ret[i].id = i;

            IndexedArrayui::assign(imgLabel, ret[i].coords, i);
        }

        return imgLabel;
    }

    /**
     * @brief convertFromIntegerToImage
     * @param imgLabel
     * @param imgOut
     * @param width
     * @param height
     * @return
     */
    static Image* convertFromIntegerToImage(unsigned int *imgLabel, Image *imgOut, int width, int height)
    {
        if(imgLabel == NULL) {
            return imgOut;
        }

        if(imgOut == NULL) {
            imgOut = new Image(1, width, height, 1);
        }

        int n = width * height;
        for(int i = 0; i < n; i++) {
            imgOut->data[i] = float(imgLabel[i]);
        }

        return imgOut;
    }

    /**
     * @brief computeLabelsList
     * @param labels
     * @param n
     * @param labelsList
     */
    static void computeLabelsListFromImageLabels(unsigned int *labels, int n,  std::vector<LabelOutput> &labelsList)
    {
        if(labels == NULL || n < 1) {
            return;
        }

        labelsList.clear();

        std::set<unsigned int> labels_tracker;

        std::map<unsigned int, int> labels_map;

        int c = 0;
        for(int i = 0; i < n; i++) {

            unsigned int j = labels[i];
            auto search = labels_tracker.find(j);
            if (search != labels_tracker.end()) {
                labels_tracker.insert(j);
                labels_map[j] = c;

                LabelOutput tmp;
                tmp.id = j;
                labelsList.push_back(tmp);

                c++;
            }

            labelsList[labels_map[j]].add(i);
        }
    }

    /**
     * @brief computeImageLabelsFromLabelsList
     * @param labelsList
     * @param labels
     * @param n
     * @return
     */
    static unsigned int *computeImageLabelsFromLabelsList(std::vector<LabelOutput> &labelsList, unsigned int *labels, int n)
    {
        if(n < 1 || labelsList.empty()) {
            return labels;
        }

        if(labels == NULL) {
            labels = new unsigned int[n];
        }

        for(unsigned int i = 0; i < labelsList.size(); i++) {
            for(unsigned int j = 0; j < labelsList[i].coords.size(); j++) {
                int k = labelsList[i].coords[j];
                labels[k] = labelsList[i].id;
            }
        }

        return labels;
    }
};


} // end namespace pic

#endif /* PIC_ALGORITHMS_CONNECTED_COMPONENTS_HPP */

