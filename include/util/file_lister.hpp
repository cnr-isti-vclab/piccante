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

#ifndef PIC_UTIL_FILE_LISTER_HPP
#define PIC_UTIL_FILE_LISTER_HPP

#include "util/string.hpp"

#include <string>
#include <iostream>
#include <fstream>

namespace pic {

class FileLister
{
public:
    FileLister()
    {
    }

    static std::string FileNumber(std::string nameFile, std::string nameExt)
    {
        int counter = 0;

        std::string nameTime = nameFile;
        nameTime += ".";
        nameTime += nameExt;

        std::ifstream infile(nameTime.c_str());

        while(infile) {
            infile.close();
            nameTime = nameFile;
            nameTime += "_";
            nameTime += NumberToString(counter);
            nameTime += ".";
            nameTime += nameExt;

            infile.open(nameTime.c_str());
            counter++;
        }

        return nameFile;
    }

    static StringVec *List(std::string nameDir, std::string nameFilter,
                           StringVec *sVecOut)
    {
        if(sVecOut == NULL) {
            sVecOut = new StringVec;
        } else {
            sVecOut->clear();
        }

        /*
#ifndef PIC_DISABLE_BOOST

        try {
            fs::path full_path(fs::initial_path<fs::path>());

            fs::path path_c = fs::path(nameDir.c_str());
            full_path = fs::system_complete(path_c);//, fs::native);

            fs::directory_iterator end_iter;

            for(fs::directory_iterator dir_itr(full_path);
                dir_itr != end_iter;
                ++dir_itr) {

                if(is_regular(dir_itr->status())) {

                    std::string tmp2 = dir_itr->path().generic_string();

                    if(tmp2.find(nameFilter.c_str()) == std::string::npos) {
                        continue;
                    }

                    //Save the name in the list
                    sVecOut->push_back(tmp2);
#ifdef PIC_DEBUG
                    printf("%s\n", tmp2.c_str());
#endif
                }
            }
        } catch(std::exception &e) {
            std::cout << "Problem with directory: " + nameDir + " " + e.what();
        }

#endif
        */
        return sVecOut;
    }
};

} // end namespace pic

#endif /* PIC_UTIL_FILE_LISTER_HPP */

