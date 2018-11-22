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

#ifndef PIC_UTIL_BBOX_HPP
#define PIC_UTIL_BBOX_HPP

#include "../base.hpp"
#include "../util/string.hpp"

namespace pic {

/**
 * @brief The BBox class manages the creation of bounding boxes for images.
 */
class BBox
{
public:
    int x0, y0, z0, x1, y1, z1;
    int width, height, frames;

    /**
     * @brief BBox is a basic constructor. It does nothing.
     */
    BBox()
    {

    }

    /**
     * @brief BBox is a constructor setting the BBox up.
     * @param width is the maxium horizontal coordinate in pixels.
     * The minimum is set to 0.
     * @param height is the maxium vertical coordinate in pixels.
     * The minimum is set to 0.
     */
    BBox(int width, int height)
    {
        setBox(0, width, 0, height, 0, 1, width, height, 1);
    }

    /**
     * @brief BBox is a constructor setting the BBox up.
     * @param width is the maxium horizontal coordinate in pixels.
     * The minimum is set to 0.
     * @param height is the maxium vertical coordinate in pixels.
     * The minimum is set to 0.
     * @param frames is the maxium temporal coordinate in pixels.
     * The minimum is set to 0.
     */
    BBox(int width, int height, int frames)
    {
        setBox(0, width, 0, height, 0, frames, width, height, frames);
    }

    /**
     * @brief BBox is a constructor setting the BBox up.
     * @param x0 is the minimum horizontal coordinate in pixels.
     * @param x1 is the maximum horizontal coordinate in pixels.
     * @param y0 is the minimum vertical coordinate in pixels.
     * @param y1 is the maximum vertical coordinate in pixels.
     */
    BBox(int x0, int x1, int y0, int y1)
    {
        setBox(x0, x1, y0, y1, 0, 1, -1, -1, 1);
    }

    /**
     * @brief BBox is a constructor setting the BBox up.
     * @param x0 is the minimum horizontal coordinate in pixels.
     * @param x1 is the maximum horizontal coordinate in pixels.
     * @param y0 is the minimum vertical coordinate in pixels.
     * @param y1 is the maximum vertical coordinate in pixels.
     * @param width is the horizontal size in pixels.
     * @param height is the vertical size in pixels.
     */
    BBox(int x0, int x1, int y0, int y1, int width, int height)
    {
        setBox(x0, x1, y0, y1, 0, 1, width, height, 1);
    }

    /**
     * @brief BBox
     * @param x0 is the horizontal coordinate in pixels.
     * @param y0 is the vertical coordinate in pixels.
     * @param size is the patch size
     * @param width is the original width of the image.
     * @param height is the original height of the image.
     */
    BBox(int x0, int y0, int size, int width, int height)
    {
        setCentered(x0, y0, size, width, height);
    }

    /**
     * @brief Size computes the number of pixels in a bounding box.
     * @return It returns the number of pixels in a bounding box.
     */
    int Size()
    {
        return (y1 - y0) * (x1 - x0) * (z1 - z0);
    }

    /**
     * @brief setBox sets a BBox up.
     * @param x0 is the minimum horizontal coordinate in pixels.
     * @param x1 is the maximum horizontal coordinate in pixels.
     * @param y0 is the minimum vertical coordinate in pixels.
     * @param y1 is the maximum vertical coordinate in pixels.
     * @param z0 is the minimum temporal coordinate in pixels.
     * @param z1 is the maximum temporal coordinate in pixels.
     * @param width is the original width of the image.
     * @param height is the original height of the image.
     * @param frames is the original length of the image.
     */
    void setBox(int x0, int x1, int y0, int y1, int z0, int z1, int width,
                int height, int frames)
    {
        this->x0 = x0;
        this->y0 = y0;
        this->z0 = z0;

        this->x1 = x1;
        this->y1 = y1;
        this->z1 = z1;

        this->width = width;
        this->height = height;
        this->frames = frames;
    }

    /**
     * @brief SetCentered
     * @param x0 is the horizontal coordinate in pixels.
     * @param y0 is the vertical coordinate in pixels.
     * @param size is the patch size
     * @param width is the original width of the image.
     * @param height is the original height of the image.
     */
    void setCentered(int x0, int y0, int size, int width, int height)
    {
        this->z0 = 0;
        this->z1 = 1;

        int halfSize = size >> 1;

        this->x0 = x0 - halfSize;
        this->x1 = x0 + halfSize;

        this->y0 = y0 - halfSize;
        this->y1 = y0 + halfSize;

        this->width = width;
        this->height = height;
        this->frames = 1;
    }

    /**
     * @brief getFourBlocks sets the BBox as a quadrant of a given size.
     * @param width is horizontal size in pixels.
     * @param height is the vertical size in pixels.
     * @param i is the i-th quadrant.
     */
    void getFourBlocks(int width, int height, int i)
    {
        int halfWidth  = width >> 1;
        int halfHeight = height >> 1;

        int dataX[] = {0, 0, halfWidth, halfWidth};
        int dataY[] = {0, halfHeight, 0, halfHeight};

        if((width % 2) == 1) {
            halfWidth++;
        }

        if((height % 2) == 1) {
            halfHeight++;
        }

        setBox(dataX[i], dataX[i] + halfWidth,
               dataY[i], dataY[i] + halfHeight,
               0, 1,
               width, height, 1);
        //	print();
    }

    /**
     * @brief toString returns a string representation of BBox
     * @return It returns a string with the BBox content.
     */
    std::string toString()
    {
        return "X = (" + fromNumberToString(x0) + ", " + fromNumberToString(x1) + ") " +
               "Y = (" + fromNumberToString(y0) + ", " + fromNumberToString(y1) + ") " +
               "Z = (" + fromNumberToString(z0) + ", " + fromNumberToString(z1) + ")";
    }
};

} // end namespace pic

#endif /* PIC_UTIL_BBOX_HPP */

