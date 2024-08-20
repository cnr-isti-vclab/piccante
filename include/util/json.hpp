/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2019
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.

*/

#ifndef PIC_IO_JSON
#define PIC_IO_JSON

#include <stdio.h>
#include <string>
#include <set>
#include <regex>
#include <vector>

#include "../base.hpp"
#include "../util/string.hpp"
#include "../util/math.hpp"

namespace pic {


enum JSONVALUETYPE{JNUMBER, JOBJECT, JARRAY, JSTRING, JFALSE, JTRUE, JNULL};

class JSONValue
{
public:
    JSONVALUETYPE type;

    JSONValue()
    {
        type = JNULL;
    }

    void setTrue()
    {
        type = JTRUE;
    }

    void setFalse()
    {
        type = JFALSE;
    }
};

class JSONString: public JSONValue
{
public:
    std::string str;

    JSONString()
    {
        str = "";
        type = JSTRING;
    }

};

class JSONNumber: public JSONValue
{
public:
    float numf;
    int numi;
    bool bFloat;

    JSONNumber()
    {
        type = JNUMBER;
    }

    void print()
    {
        if(bFloat) {
            printf("%f", numf);
        } else {
            printf("%d", numi);
        }
    }
};

class JSONArray: public JSONValue
{
public:
    std::vector<JSONValue> array;

    JSONArray()
    {
        type = JARRAY;
    }
};

class JSONObject: JSONValue
{
public:
    std::string name;
    JSONValue *value;

    JSONObject()
    {
        type = JOBJECT;
        value = NULL;
    }
};

class JSONFile
{
protected:

public:

     JSONFile()
     {
     }

     /**
      * @brief parseNumber
      * @param str
      * @return
      */
     bool parseNumber(std::string str, JSONNumber &ret)
     {
         std::regex j_integer("(\\+|-)?[[:digit:]]+");
         std::regex j_float("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?");
         std::regex j_float_scientific("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");

         if(regex_match(str, j_integer)) {
             ret.bFloat = false;
             ret.numi = atoi(str.c_str());
             return true;
         } else {
             if(std::regex_match(str, j_float)) {
                 ret.bFloat = true;
                 ret.numf = atof(str.c_str());

                 return true;
             } else {
                 if(std::regex_match(str, j_float_scientific)) {
                     ret.bFloat = true;
                     ret.numf = atof(str.c_str());
                     return true;

                 }

                 return false;
             }
         }
     }

     bool parseString(std::string str, JSONString &ret)
     {
         std::regex j_string("\"[[:print:]*[:blank:]*[:punct:]*[:upper:]*[\\]*[\n]*[\/]*[\"]*[\b]*[\f]*[\n]*[\r]*[\t]*]*\"");
         if (regex_match(str, j_string)) {
             ret.str = str;
             ret.str.erase(0, 1);
             ret.str.erase(ret.str.size() -1);
             return true;
         }
         return false;
     }

     bool parseWhitespace(std::string str, JSONString& ret)
     {
         std::regex j_string("[[:space:]*[\t]*[\r]*[\n]*]*");
         if (regex_match(str, j_string)) {
             return true;
         }
         return false;
     }

     void parseObject()
     {

     }

     void parseArray()
     {

     }

     /**
      * @brief testParserNumbers
      */
     void testParserNumbers()
     {
         printf("Test numbers:\n");
         JSONNumber ret;
         parseNumber("121324", ret);
         printf("\n");
         ret.print();

         printf("\n");

         parseNumber("121.324", ret);
         ret.print();
         printf("\n");

         parseNumber("1.4e6", ret);
         ret.print();
         printf("\n");

     }

     void testParserString()
     {
         printf("Test strings:\n");
         JSONString ret;
         parseString("\"number1\"", ret);
         printf("%s\n", ret.str.c_str());

         parseString("\"./number1\"", ret);
         printf("%s\n", ret.str.c_str());
     }
};

} // end namespace pic

#endif /* PIC_IO_JSON */

