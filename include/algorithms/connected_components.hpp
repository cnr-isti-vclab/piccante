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
    uint id;
    uint minLabel;

    friend bool operator<(LabelInfo const &a, LabelInfo const &b)
    {
        return a.id < b.id;
    }
};

class LabelOutput
{
public:
    uint id;
    std::vector< int > coords;
    std::set< int > neighbors;
    bool bValid;

    LabelOutput()
    {
        id = 0;
        bValid = true;
    }

    LabelOutput(uint id, int i)
    {
        this->id = id;
        coords.push_back(i);
        bValid = true;
    }

    void push_back(int i)
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
    void secondPass(uint *imgOut, std::vector<LabelOutput> &ret, std::set<LabelInfo> &labelEq, int n)
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
        std::set<uint> unique;
        //std::set<uint>::iterator uniqueIt;
        std::map<uint, int> mapping;

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
                ret[mapping[id]].push_back(i);
            } else {
                std::pair<uint, int> tmp = std::make_pair(id, counter);
                mapping.insert(tmp);

                LabelOutput tmpRet(id, i);
                ret.push_back(tmpRet);

                unique.insert(id);
                counter++;
            }
        }
    }

    void track(uint *imgOut, int &label, std::set<LabelInfo> &labelEq,
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
            uint minVal, t1, t2;
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
    uint *execute(Image *imgIn, uint *imgOut, std::vector<LabelOutput> &ret)
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
            imgOut = new uint[n];
        }

        Buffer<uint>::assign(imgOut, n, 0);

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
    uint *execute(T *imgIn, int width, int height, uint *imgOut, std::vector<LabelOutput> &ret)
    {
        //Check input paramters
        if(imgIn == NULL) {
            return imgOut;
        }

        int n = width * height;

        if(imgOut == NULL) {
            imgOut = new uint[n];
        }

        Buffer<uint>::assign(imgOut, n, 0);

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
    static uint *reCount(uint *imgLabel, std::vector<LabelOutput> &labelsList)
    {
        if(imgLabel == NULL) {
            return NULL;
        }

        uint c = 0;
        for(uint i = 0; i < labelsList.size(); i++) {
            if(labelsList[i].bValid) {
                labelsList[i].id = c;
                IndexedArrayui::assign(imgLabel, labelsList[i].coords, c);
                c++;
            }
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
    static Image* convertFromIntegerToImage(uint *imgLabel, Image *imgOut, int width, int height)
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
    static void computeLabelsListFromImageLabels(uint *labels, int n,  std::vector<LabelOutput> &labelsList)
    {
        if(labels == NULL || n < 1) {
            return;
        }

        labelsList.clear();

        std::set<uint> labels_tracker;

        std::map<uint, int> labels_map;

        int c = 0;
        for(int i = 0; i < n; i++) {
            uint j = labels[i];
            auto search = labels_tracker.find(j);
            if (search != labels_tracker.end()) {
                labels_tracker.insert(j);
                labels_map[j] = c;

                LabelOutput tmp;
                tmp.id = j;
                labelsList.push_back(tmp);

                c++;
            }

            labelsList[labels_map[j]].push_back(i);
        }
    }

    /**
     * @brief computeImageLabelsFromLabelsList
     * @param labelsList
     * @param labels
     * @param n
     * @return
     */
    static uint *computeImageLabelsFromLabelsList(std::vector<LabelOutput> &labelsList, uint *labels, int n)
    {
        if(n < 1 || labelsList.empty()) {
            return labels;
        }

        if(labels == NULL) {
            labels = new uint[n];
        }

        for(uint i = 0; i < labelsList.size(); i++) {
            if(labelsList[i].bValid) {
                for(uint j = 0; j < labelsList[i].coords.size(); j++) {
                    int k = labelsList[i].coords[j];
                    labels[k] = labelsList[i].id;
                }
            }
        }

        return labels;
    }


    /**
     * @brief getMappingLabelsList
     * @param labelsList
     * @param labels_map
     */
    static void getMappingLabelsList(std::vector<LabelOutput> &labelsList, std::map<uint, int> &labels_map)
    {
        for(uint i = 0; i < labelsList.size(); i++) {
            labels_map[labelsList[i].id] = i;
        }
    }

    /**
     * @brief computeNeighbors
     * @param labels
     * @param width
     * @param height
     * @param labelsList
     */
    static void computeNeighbors(uint *labels, int width, int height, std::vector<LabelOutput> &labelsList)
    {
        std::map<uint, int> labels_map;
        getMappingLabelsList(labelsList, labels_map);

        int width_m_1 = width - 1;
        int height_m_1 = height - 1;

        for(int i = 0; i < height; i++) {
            int shift = i * width;

            for(int j = 0; j < width; j++) {
                int ind = shift + j;

                uint l_ind = labels[ind];
                int ind2 = labels_map[l_ind];

                if(i > 0) {
                    if(l_ind != labels[ind - width]) {
                        labelsList[ind2].neighbors.insert(labels[ind - width]);
                    }
                }

                if(j > 0) {
                    if(l_ind != labels[ind - 1]) {
                        labelsList[ind2].neighbors.insert(labels[ind - 1]);
                    }
                }

                if(i < height_m_1) {
                    if(l_ind != labels[ind + width]) {
                        labelsList[ind2].neighbors.insert(labels[ind + width]);
                    }
                }

                if(j < width_m_1) {
                    if(l_ind != labels[ind + 1]) {
                        labelsList[ind2].neighbors.insert(labels[ind + 1]);
                    }
                }

            }
        }
    }

    /**
     * @brief mergeIsolatedAreasWithThreshold
     * @param labels
     * @param width
     * @param height
     * @param labelsList
     * @param threshold
     */
    static void mergeIsolatedAreasWithThreshold(uint *labels, int width, int height, std::vector<LabelOutput> &labelsList, int threshold = 1)
    {
        if(threshold < 1 || labels == NULL || labelsList.empty()) {
            return;
        }

        if(labelsList[0].neighbors.empty()) {
            computeNeighbors(labels, width, height, labelsList);
        }

        std::map<uint, int> labels_map;
        getMappingLabelsList(labelsList, labels_map);

        for(uint i = 0; i < labelsList.size(); i++) {
            if(!labelsList[i].bValid || labelsList[i].neighbors.empty()) {
                continue;
            }

            if(labelsList[i].neighbors.size() == 1) {
                uint id = *labelsList[i].neighbors.begin();
                int index = labels_map[id];

                if(labelsList[index].bValid) {

                    if(labelsList[i].coords.size() > labelsList[index].coords.size()) {
                        labelsList[index].bValid = false;

                        //update coordinates
                        labelsList[i].coords.insert(labelsList[i].coords.begin(),
                                                    labelsList[index].coords.begin(),
                                                    labelsList[index].coords.end());

                        //update neighbors
                        if(labelsList[index].neighbors.size() > 1) {
                            labelsList[i].neighbors.insert(labelsList[index].neighbors.begin(), labelsList[index].neighbors.end());

                            //update all neighbors removing index and adding i!
                            for (auto it = labelsList[index].neighbors.begin(); it != labelsList[index].neighbors.end(); it++) {
                                uint id2 = *it;
                                int index2 = labels_map[id2];

                                labelsList[index2].neighbors.erase(index);
                                labelsList[index2].neighbors.insert(i);
                            }
                        }
                    } else {
                        labelsList[i].bValid = false;

                        //update coordinates
                        labelsList[index].coords.insert(labelsList[index].coords.begin(),
                                                        labelsList[i].coords.begin(),
                                                        labelsList[i].coords.end());

                        //it does not have anymore this neighbor because it has been merged
                        labelsList[index].neighbors.erase(i);
                    }
                }
            }
        }

        computeImageLabelsFromLabelsList(labelsList, labels, width * height);
    }

};


} // end namespace pic

#endif /* PIC_ALGORITHMS_CONNECTED_COMPONENTS_HPP */

