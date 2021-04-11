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

#ifndef PIC_ALGORITHMS_HDR_MERGER_HPP
#define PIC_ALGORITHMS_HDR_MERGER_HPP

#include <vector>
#include <string>

#include "../base.hpp"
#include "../util/vec.hpp"
#include "../algorithms.hpp"
#include "../algorithms/camera_response_function.hpp"
#include "../features_matching/ward_alignment.hpp"
#include "../filtering/filter_assemble_hdr.hpp"

namespace pic {

enum HDRAlign{HA_NONE, HA_MTB, HA_FEATURES};

class HDRMerger
{
protected:
    CameraResponseFunction *crf;
    FilterAssembleHDR merger;
    HDRAlign hdra;

    CRF_WEIGHT weight;
    HDR_REC_DOMAIN domain;

    std::vector<std::string> file_name_vec;
    std::vector<float> exposure_time_vec;

    /**
     * @brief incrementalAlignment
     * @param stack
     * @param s_vec
     */
    void incrementalAlignment(ImageVec &stack, std::vector<Vec2i> &s_vec)
    {
        int n = int(stack.size());

        for(int i = 0; i < (n - 1); i++) {
            Vec2i s_i = WardAlignment::execute(stack[i + 1], stack[i]);
            s_vec.push_back(s_i);
        }

        int n2 = int(s_vec.size());
        for(int i = 0; i < (n2 - 1); i++) {
            Vec2i n_i = s_vec[i];

            for(int j = (i + 1); j < n2; j++) {
                n_i += s_vec[j];
            }

            s_vec[i] = n_i;
        }
    }

public:

    /**
     * @brief HDRMerger
     */
    HDRMerger()
    {
        domain = HRD_LOG;
        weight = CW_DEB97;
        hdra = HA_NONE;

        crf = NULL;
    }

    ~HDRMerger()
    {
        release();
    }

    /**
     * @brief release
     */
    void release()
    {
    }

    /**
     * @brief update
     * @param weight
     * @param domain
     * @param crf
     */
    void update(CRF_WEIGHT weight, HDR_REC_DOMAIN domain,
                HDRAlign hdra,
                CameraResponseFunction *crf = NULL)
    {
        this->hdra = hdra;
        this->weight = weight;
        this->domain = domain;
        this->crf = crf;
    }

    /**
     * @brief addFile
     * @param file_name
     * @param exposure_time
     */
    void addFile(std::string file_name, float exposure_time = -1.0f)
    {
        file_name_vec.push_back(file_name);
        exposure_time_vec.push_back(exposure_time);
    }

    /**
     * @brief execute
     * @param imgOut
     * @return
     */
    Image *execute(Image *imgOut = NULL)
    {
        ImageVec stack;

        bool bValid = true;
        int n = int(file_name_vec.size());

        for(int i = 0; i < n; i++) {
            Image *img = new Image();
            img->Read(file_name_vec[i], LT_NOR);
            stack.push_back(img);
            bValid = bValid && img->isValid();
        }

        if(!bValid) {
            return imgOut;
        }

        ImageVec stack_aligned;
        ImageVec stack_aligned_track;

        //align images
        if(hdra != HA_NONE && (n > 1)) {
            ImaveVecSortByExposureTime(stack);

            if(hdra == HA_MTB) {
                std::vector<Vec2i> shifts;
                incrementalAlignment(stack, shifts);

                stack_aligned.push_back(stack[n - 1]);

                for(int i = 0; i < int(shifts.size()); i++) {
                    auto s_i = shifts[i];
                    if(s_i[0] == 0 && s_i[1] == 0) {
                        stack_aligned.push_back(stack[i]);
                    } else {
                        auto tmp_i = WardAlignment::shiftImage(stack[i], shifts[i], NULL);
                        stack_aligned.push_back(tmp_i);
                        stack_aligned_track.push_back(tmp_i);
                    }
                }
            }
        }

        //compute CRF
        if(crf == NULL) {
            crf = new CameraResponseFunction();
            crf->DebevecMalik(stack, weight, 256, 20.0f);
        }

        //merge all exposure images
        merger.update(crf, weight, domain);

        if(hdra != HA_NONE) {
            imgOut = merger.Process(stack_aligned, imgOut);
        } else {
            imgOut = merger.Process(stack, imgOut);
        }

        stdVectorClear(stack);
        stdVectorClear(stack_aligned_track);

        return imgOut;
    }
};

} // end namespace pic

#endif /* PIC_ALGORITHMS_WEIGHT_FUNCTION_HPP */

