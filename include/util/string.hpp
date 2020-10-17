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

#ifndef PIC_UTIL_STRING_HPP
#define PIC_UTIL_STRING_HPP

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#include "../base.hpp"


#ifdef PIC_WIN32
    #include <direct.h>
#endif

#ifndef PIC_WIN32
    #include <unistd.h>
#endif


namespace pic {

/**
*@brief MAKE_STRING creates a string from a block.
*/
#define MAKE_STRING(input_string) #input_string

/**
 * @brief StringVec is an std::vector of std::string.
 */
typedef std::vector<std::string > StringVec;

/**
 * @brief stdStringRep replaces strSub in str with strRep just once.
 * @param str is the input string.
 * @param strSub is the substring to find in str.
 * @param strRep is the string for replacing strSub.
 * @return It returns str where strSub is replaced with strRep.
 */
inline std::string stdStringRep(std::string str, std::string strSub,
                                std::string strRep)
{
    std::string ret = str;

    size_t found = ret.find(strSub);

    if(found != std::string::npos) {
        ret.replace(found, strRep.length(), strRep);
    }

    return ret;
}

/**
 * @brief stdStringRepAll replaces all strSub in str with strRep.
 * @param str
 * @param strSub
 * @param strRep
 * @return
 */
inline std::string stdStringRepAll(std::string str, std::string strSub,
                                   std::string strRep)
{
    auto n_sub = strSub.size();
    auto n_rep = strRep.size();

    std::string ret = str;
    std::string::size_type pos = ret.find(strSub);

    while(pos != std::string::npos) {
        ret.replace(pos, n_sub, strRep);

        pos = ret.find(strSub, pos + 1 - n_sub + n_rep);
    }

    return ret;
}

/**
 * @brief fromNumberToString converts a number into a string.
 * @param num is an input number.
 */
template<class T>
inline std::string fromNumberToString(T num)
{
    std::ostringstream convert;
    convert << num;
    return convert.str();
}

/**
 * @brief getSeparatorChar returns the folder separator in path as a char.
 * @param path
 * @return
 */
inline char getSeparatorChar(std::string path)
{
    if(path.find("/") != std::string::npos) {
        return '/';
    } else {
        if(path.find("\\") != std::string::npos) {
            return '\\';
        } else {
            return '/';
        }
    }
}

/**
 * @brief RemoveExtension removes the extension of a string.
 * @param name
 * @return
 */
inline std::string removeExtension(std::string name)
{
    std::string tmp(name);
    std::reverse(tmp.begin(), tmp.end());

    size_t pos = tmp.find(".");

    if(pos != std::string::npos) {
        name.erase(name.end() - pos - 1, name.end());
    }

    return name;
}

/**
 * @brief removeLocalPath removes the local path of a string.
 * @param name
 * @return
 */
inline std::string removeLocalPath(std::string name)
{
    std::string toFind(1, getSeparatorChar(name));

    if(toFind.empty()) {
        return name;
    } else {
        size_t oldPos;
        size_t pos = 0;
        do{
            oldPos = pos;
            pos = name.find(toFind, pos + 1);
        } while(pos != std::string::npos);

        name.erase(0, oldPos + 1);
    }

    return name;
}

/**
 * @brief getFileNameOnly
 * @param name is the input name with global/local path and extension
 * @return it returns ONLY the file name without path
 */
inline std::string getFileNameOnly(std::string name)
{
    return removeLocalPath(removeExtension(name));
}

/**
 * @brief getExtension gets the extension of a file name.
 * @param name
 * @return
 */
inline std::string getExtension(std::string name)
{
    std::string tmp(name);
    std::reverse(tmp.begin(), tmp.end());

    size_t pos = tmp.find(".");
    std::string ext = "";

    if(pos != std::string::npos) {
        auto n = name.length() - pos;
        ext = name.substr(n, n);
    }

    return ext;
}

/**
 * @brief addSuffix adds a suffix to a file name.
 * @param name
 * @param suffix
 * @return
 */
inline std::string addSuffix(std::string name, std::string suffix)
{
    std::string tmp = removeExtension(name);
    std::string tmpExt = getExtension(name);
    return tmp + suffix + "." + tmpExt;
}

/**
 * @brief replaceExtension changes .format in a file name.
 * @param nameOut
 * @param fmtIn
 * @param fmtOut
 * @return
 */
inline std::string replaceExtension(std::string nameOut, std::string fmtIn,
                                std::string fmtOut)
{
    size_t found = nameOut.find(fmtIn);

    if(found != std::string::npos) {
        nameOut.replace(nameOut.begin() + found, nameOut.end(), fmtOut);
    }

    return nameOut;
}

/**
 * @brief countSubString counts how many subStr are in str.
 * @param str is the input string.
 * @param subStr is the substring to count in str.
 * @return the number of times subStr appears in str.
 */
inline int countSubString(std::string str, std::string subStr)
{
    int count = 0;

    std::string::size_type pos = str.find(subStr);

    while(pos != std::string::npos) {
        count++;
        pos = str.find(subStr, pos + 1);
    }

    return count;
}

/**
 * @brief getLocaDirectory gets local path.
 * @param path
 * @return
 */
inline std::string getLocaDirectory(std::string path)
{
    std::string ret = path;

    std::string toFind(1, getSeparatorChar(path));

    if(toFind.empty()) {
        return ret;
    }

    size_t pos1 = path.rfind(toFind);

    if(pos1 != std::string::npos) {
        ret = path.substr(0, pos1);
        size_t pos2 = ret.rfind(toFind);

        if(pos2 != std::string::npos) {
            return ret.substr(pos2 + 1, ret.length());
        }
    }

    return ret;
}

/**
 * @brief getSeparator returns the folder separator in path as a string
 * @param path
 * @return
 */
inline std::string getSeparator(std::string path)
{
    char sepChar = getSeparatorChar(path);
    std::string strOut;
    return strOut + sepChar;
}

/**
 * @brief getFolderName gets the folder name from the path.
 * @param path
 * @return
 */
inline std::string getFolderName(std::string path)
{
    size_t found = path.find_last_of(getSeparator(path));

    if(found != std::string::npos) {
        return path.substr(0, found);
    } else {
        return "./";
    }
}

/**
 * @brief getFileName gets the file name.
 * @param path
 * @return
 */
inline std::string getFileName(std::string path)
{
    std::string toFind;
    std::string ret = path;

    if(path.find("/") != std::string::npos) {
        toFind = "/";
    } else {
        if(path.find("\\") != std::string::npos) {
            toFind = "\\";
        } else {
            return ret;
        }
    }

    size_t pos = path.rfind(toFind);

    if(pos != std::string::npos) {
        ret = path.substr(pos + 1, path.length());
        return ret;
    }

    return ret;
}

/**
 * @brief parseStringToStdVector
 * @param str
 * @param delim
 * @param str_vec
 */
inline void parseStringToStdVector(std::string str, char delim,
                                   StringVec *str_vec)
{
    std::stringstream ss(str);

    while(!ss.eof()) {
        std::string tmpStr;
        std::getline(ss, tmpStr, delim);
        str_vec->push_back(tmpStr);
    }
}

/**
 * @brief genBilString
 * @param type
 * @param sigma_s
 * @param sigma_r
 * @return
 */
inline std::string genBilString(std::string type, float sigma_s,
                                    float sigma_r)
{
    std::string ret = type +
            "_Ss_" + fromNumberToString(sigma_s) +
            "_Sr_" + fromNumberToString(sigma_r);
    return ret;
}

/**
 * @brief fromFileToStdString writes a file into a std::string.
 * @param nameFile
 * @return
 */
inline std::string fromFileToStdString(std::string nameFile)
{
    std::ifstream infile;
    infile.open(nameFile.c_str(), std::ios::in);

    std::string ret;

    if((!infile.is_open()) || (!infile.good())) {
        return ret;
    }

    int c = infile.get();
    while (infile.good()) {
        ret += c;
        c = infile.get();
    }

    infile.close();

    return ret;
}

/**
 * @brief checkAbsolutePath checks if the path is absolute or not.
 * @param path
 * @return
 */
inline bool checkAbsolutePath(std::string path)
{
    //win32 absolute path
    if(path.find(":\\") != std::string::npos) {
        return true;
    }

    if(path.find(":/") != std::string::npos) {
        return true;
    }

    if(path.find("\\\\\"") != std::string::npos) {
        return true;
    }

    //unix/mac path
    return (path.at(0) == '/');
}

/**
 * @brief fromStdStringToChar converts from a std::string to a char*.
 * @param str
 * @return
 */
inline char *fromStdStringToChar(std::string str)
{
    char *cstr = new char [str.size() + 1];
    strcpy (cstr, str.c_str());
    return cstr;
}

/**
 * @brief checkPath
 * @param name
 * @return
 */
inline std::string checkPath(std::string name)
{
    if(name.length() < 3) {
        return "";
    }

    if((name.at(0) == '.') && (name.at(0) == '.')) {
        #ifdef PIC_WIN32
            char *path = _getcwd(NULL, 0);
        #endif

        #ifndef PIC_WIN32
            char *path = getcwd(NULL, 0);
        #endif

        std::string dsepName = getSeparator(name);
        std::string dsepPath = getSeparator(path);

        name = stdStringRepAll(name, dsepName, dsepPath);
        if(name.at(2) == '\\' || name.at(2) == '/') {
            name = name.substr(3);
        } else {
            name = name.substr(2);
        }

        std::string newPath  = path + dsepPath + name;
        return newPath;
    } else {
        return "";
    }
}

/**
 * @brief adjustPath modifies the path if it is not global.
 * @param nameFile
 * @param pathFolder
 * @return
 */
std::string adjustPath(std::string nameFile, std::string pathFolder)
{
    if(!checkAbsolutePath(nameFile)) {
        std::string fullPath = checkPath(nameFile);

        if(fullPath.empty()) {
            std::string ret = pathFolder + getSeparator(pathFolder) + nameFile;
            return ret;
        } else {
            return fullPath;
        }
    } else {
        return nameFile;
    }
}

/**
 * @brief removeInitialSpaces removes spaces at the beginning of a string.
 * @param name
 * @return
 */
inline std::string removeInitialSpaces(char name[])
{
    size_t pos;
    std::string ret = name;

    pos = ret.find(' ');
    ret.erase(pos, 1);

    pos = ret.find('\n');
    ret.erase(pos, 1);

    return ret;
}

} // end namespace pic

#endif /* PIC_UTIL_STRING_HPP */

