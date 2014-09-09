/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP
#define PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP

#include "filtering/filter.hpp"

#include "algorithms/camera_response_function.hpp"

namespace pic {

class FilterAssembleHDR: public Filter
{
protected:
    CRF_WEIGHT              weight_type;
    IMG_LIN                 linearization_type;
    std::vector<float *>    *icrf;

    /**ProcessBBox: assembling an HDR image*/
    void ProcessBBox(ImageRAW *dst, ImageRAWVec src, BBox *box)
    {
        int width = dst->width;
        int channels = dst->channels;

        unsigned int n = src.size();

        for(int j = box->y0; j < box->y1; j++) {
            int ind = j * width;

            for(int i = box->x0; i < box->x1; i++) {

                //Assembling kernel
                int c = (ind + i) * channels;

                bool flag = false;

                float maxVal = -1.0f;

                for(int k = 0; k < channels; k++) {
                    float weight_norm = 0.0f;
                    float acc = 0.0f;

                    for(unsigned int l = 0; l < n; l++) {
                        float x = src[l]->data[c + k];

                        float weight = WeightFunction(x, weight_type);

                        float x_lin;
                        if((icrf != NULL) || (linearization_type != LIN_ICFR)) {
                            x_lin = Linearize(x, linearization_type, icrf->at(k));
                        } else {
                            x_lin = x;
                        }

                        acc += (weight * x_lin) / src[l]->exposure;

                        weight_norm += weight;
                    }

                    flag = flag || (weight_norm <= 0.0f);

                    float final_value = weight_norm > 0.0f ? (acc / weight_norm) : -1.0f;
                    dst->data[c + k] = final_value;

                    maxVal = final_value > maxVal ? final_value : maxVal;
                }

                //we had a saturated pixel...
                for(int k = 0; k < channels; k++) {
                    if(dst->data[c + k] < 0.0f) {
                        dst->data[c + k] = maxVal;
                    }
                }
            }
        }
    }

public:
    //Basic constructors
    FilterAssembleHDR(CRF_WEIGHT weight_type = CRF_GAUSS, IMG_LIN linearization_type = LIN_LIN, std::vector<float *> *icrf = NULL)
    {
        this->weight_type = weight_type;

        this->linearization_type = linearization_type;
        this->icrf = icrf;
    }

    //Assemble an HDR image from RAW images
    static void FromRAWs(std::string nameFileIn, std::string nameFileOut)
    {
        FILE *file = fopen(nameFileIn.c_str(), "r");

        if(file == NULL) {
            return;
        }

        //Read header
        int width, height, bits;
        char tmp[64];
        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &width);
        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &height);
        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &bits);

        ImageRAWVec stack;
        int i = 0;

        while(!feof(file)) {
            char name[1024];
            float exposure;
            fscanf(file, "%f", &exposure);
            fscanf(file, "%s", name);

            printf("Processing image: %s\n", name);

            ImageRAW *img = new ImageRAW();

            img->ReadRAW(name, "NULL", RAW_U16_RGGB, width, height);
            img->exposure = exposure;
            stack.push_back(img);
            i++;
        }

        fclose(file);

        FilterAssembleHDR fAHDR;
        ImageRAW *imgOut = fAHDR.ProcessP(stack, NULL);
        imgOut->Write(nameFileOut);
    }
};

} // end namespace pic

#endif /* PIC_FILTERING_FILTER_ASSEMBLE_HDR_HPP */

