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

#ifndef PIC_UTIL_TILE_LIST_HPP
#define PIC_UTIL_TILE_LIST_HPP

#include <thread>
#include <mutex>
#include <vector>

#include "../base.hpp"
#include "../util/tile.hpp"

namespace pic {

/**
 * @brief The TileList class
 */
class TileList
{
protected:
    uint counter;

#ifndef PIC_DISABLE_THREAD
    std::mutex mutex;
#endif

public:
    int width, height;
    int h_tile, w_tile;
    int mod_h, mod_w;

    /**
     * @brief tiles a list of tiles
     */
    std::vector<Tile> tiles;

    /**
     * @brief TileList basic constructor
     */
    TileList();

    /**
     * @brief TileList creates a list of tiles.
     * @param tileSize is the width and height of a tile in pixels.
     * @param width is the horizontal size of the original image in pixels.
     * @param height is the vertical size of the original image in pixels.
     */
    TileList(int tileSize, int width, int height);

    ~TileList();

    /**
     * @brief genBBox
     * @param index
     * @return
     */
    BBox getBBox(int index);

    /**
     * @brief getNext returns the index of the next tile to process.
     * @return This function returns the index of the next tile to proces.
     */
    uint getNext();

    /**
     * @brief size
     * @return
     */
    uint size();

    /**
     * @brief resetCounter sets the counter to zero.
     */
    void resetCounter();

    /**
     * @brief Create creates a list of tiles.
     * @param tileSize is the width and height of a tile in pixels.
     * @param width is the horizontal size of the original image in pixels.
     * @param height is the vertical size of the original image in pixels.
     */
    void create(int tileSize, int width, int height);

    /**
     * @brief read loads a TileList from a file.
     * @param name is the file name.
     * @param flag is a boolean value. If it is true, this loads
     * an Image from the tile name. Otherwise, an Image of the tile
     * size is allocated.
     * @return This function returns true if it is successfull.
     */
    bool read(std::string name, bool flag);

    /**
     * @brief write saves a TileList into a file.
     * @param name is the file name
     * @return This function returns true if it is successfull.
     */
    bool write(std::string name);

    /**
     * @brief writeIntoMemory copies tiles inside an output image.
     * @param output is the Image where tiles will be copied to.
     */
    void writeIntoMemory(Image *output);
};

PIC_INLINE TileList::TileList()
{
    counter = 0;

    w_tile = 0;
    h_tile = 0;

    mod_h = 0;
    mod_w = 0;
}

PIC_INLINE TileList::TileList(int tileSize, int width, int height)
{
    counter = 0;
    create(tileSize, width, height);
}

PIC_INLINE TileList::~TileList()
{
    tiles.clear();
}

PIC_INLINE BBox TileList::getBBox(int index)
{
    int i = index % tiles.size();

    return tiles[i].getBBox(width, height);
}

PIC_INLINE uint TileList::getNext()
{
    uint ret = 0;
    {
#ifndef PIC_DISABLE_THREAD
        std::lock_guard<std::mutex> lock(mutex);
#endif
        ret = counter;
        counter++;
    }
    return ret;
}

PIC_INLINE uint TileList::size()
{
    return (uint)(tiles.size());
}

PIC_INLINE void TileList::resetCounter()
{
    {
#ifndef PIC_DISABLE_THREAD
        std::lock_guard<std::mutex> lock(mutex);
#endif
        counter = 0;
    }
}

PIC_INLINE void TileList::create(int tileSize, int width, int height)
{
    resetCounter();

    if(!tiles.empty()) {
        if((tiles[0].width == tileSize) && (this->width == width) &&
           (this->height == height)) {
            return;
        }

        tiles.clear();
    }

    this->width = width;
    this->height = height;

    h_tile = height / tileSize;
    w_tile = width  / tileSize;
    mod_h  = height % tileSize;
    mod_w  = width  % tileSize;

    //main blocks
    bool bWidth = mod_w != 0;
    for(int i = 0; i < h_tile; i++) {
        Tile tile;
        tile.width = tileSize;
        tile.height = tileSize;
        tile.startY = i * tileSize;

        for(int j = 0; j < w_tile; j++) {
            tile.startX = j * tileSize;
            tiles.push_back(tile);
        }

        //extra blocks
        if(bWidth) {
            tile.startX = w_tile * tileSize;
            tile.width  = mod_w;
            tiles.push_back(tile);
        }
    }

    //fixed height strip blocks
    if(mod_h != 0) {
        int i = h_tile;

        Tile tile;
        tile.startY = i * tileSize;
        tile.width  = tileSize;

        for(int j = 0; j < w_tile; j++) {
            tile.startX = j * tileSize;
            tile.height  = mod_h;
            tiles.push_back(tile);
        }

        if(bWidth) {
            tile.startX = w_tile * tileSize;
            tile.width  = mod_w;
            tile.height  = mod_h;
            tiles.push_back(tile);
        }
    }
}

PIC_INLINE void TileList::writeIntoMemory(Image *output)
{
    if(output == NULL) {
        return;
    }

    if(!output->isValid()) {
        return;
    }

    for(uint i = 0; i < tiles.size(); i++) { //for each tile
        if(tiles[i].tile != NULL) {
            output->copySubImage(tiles[i].tile, tiles[i].startX, tiles[i].startY);
        }

        #ifdef PIC_DEBUG
            printf("Tile x: %d y: %d\n", tiles[i].startX, tiles[i].startY);
        #endif
    }
}

PIC_INLINE bool TileList::read(std::string name, bool flag)
{
    FILE *file = fopen(name.c_str(), "r");

    if(file == NULL) {
        return false;
    }

    //tmp vars
    char tmp[128];
    char txt[128];

    //number of tiles
    int n;
    fscanf(file, "%s", tmp);
    fscanf(file, "%d", &n);

    //flag
    fscanf(file, "%s", tmp);
    fscanf(file, "%s", txt);

    Tile tmpTile;
    char tmp_name[128];

    for(int i = 0; i < n; i++) { //for each tile
        fscanf(file, "%s", tmp);
        fscanf(file, "%s", tmp_name);

        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &tmpTile.startX);

        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &tmpTile.startY);

        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &tmpTile.width);

        fscanf(file, "%s", tmp);
        fscanf(file, "%d", &tmpTile.height);

        tmpTile.name = tmp_name;

        if(flag) {
            tmpTile.tile = new Image(tmpTile.name);
        } else {
            tmpTile.tile = new Image(1, tmpTile.width, tmpTile.height, 3);
        }

        tiles.push_back(tmpTile);
    }

    fclose(file);
    return true;
}

PIC_INLINE bool TileList::write(std::string name)
{
    FILE *file = fopen(name.c_str(), "w");

    if(file == NULL) {
        return false;
    }

    //number of tiles
    int n = int(tiles.size());
    fprintf(file, "NUMBER_OF_TILES: %d\n", n);

    //flag
    fprintf(file, "FLAG: NONE\n");

    for(int i = 0; i < n; i++) { //for each tile

        bool bName = !tiles[i].name.empty();
        if(bName) {
            fprintf(file, "Tile_name: %s\n", tiles[i].name.c_str());
        } else {
            fprintf(file, "Tile_name: none\n");
        }

        fprintf(file, "StartX: %d\n", tiles[i].startX);
        fprintf(file, "StartY: %d\n", tiles[i].startY);

        fprintf(file, "Width: %d\n", tiles[i].width);
        fprintf(file, "Height: %d\n", tiles[i].height);

        if(bName && tiles[i].tile != NULL) {
            tiles[i].tile->Write(tiles[i].name);
        }
    }

    fclose(file);

    return true;
}

} // end namespace pic

#endif /* PIC_UTIL_TILE_LIST_HPP */

