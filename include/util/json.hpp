/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2024
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
#include <stack>
#include "../base.hpp"
#include "../util/string.hpp"
#include "../util/math.hpp"

namespace pic {


enum JSONVALUETYPE{JNUMBER, JOBJECT, JARRAY, JSTRING, JFALSE, JTRUE, JNULL};

class JSONObject;

class JSONArray;

class JSONValue
{
public:
    JSONVALUETYPE type;
    bool lookingForValue;

    JSONValue()
    {
        lookingForValue = false;
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

    void setNull()
    {
        type = JNULL;
    }

    virtual JSONValue* check(std::string key)
    {
        return NULL;
    }

    virtual void addName(std::string name) 
    {

    }

    virtual void addValue(JSONValue* value) 
    {

    }

    void printType() 
    {
        switch (type)
        {
        case JARRAY:
            printf("Array");
            break;
        case JOBJECT:
            printf("Object");
            break;
        case JSTRING:
            printf("String");
            break;
        }
        printf("\n");
    }

    virtual void print()
    {
        if (type == JTRUE) {
            printf("true");
        }

        if (type == JFALSE) {
            printf("false");
        }

        if (type == JNULL) {
            printf("null");
        }
    }

    JSONObject* getObject(std::string key)
    {
        JSONValue* tmp = check(key);

        if (tmp != NULL) {
            if (tmp->type == JOBJECT) {
                return (JSONObject*)tmp;
            }
        }
        return NULL;
    }

    JSONArray* getArray(std::string key)
    {
        JSONValue* tmp = check(key);

        if (tmp != NULL) {
            if (tmp->type == JARRAY) {
                return (JSONArray*)tmp;
            }
        }
        return NULL;
    }
};

class JSONString: public JSONValue
{
public:
    std::string str;

    JSONString()
    {
        this->str = "";
        this->type = JSTRING;
    }

    JSONString(std::string str)
    {
        this->str = str;
        this->type = JSTRING;
    }

    void print()
    {
        printf("\"%s\"", str.c_str());
    }

};


class JSONNumber : public JSONValue
{
public:
    double numf;
    int numi;
    bool bFloat;

    JSONNumber()
    {
        bFloat = false;
        numf = 0.0;
        numi = 0;
        type = JNUMBER;
    }

    float getFloat()
    {
        if (bFloat) {
            return float(numf);
        } else {
            return float(numi);
        }
    }

    double getDouble()
    {
        if (bFloat) {
            return numf;
        }
        else {
            return double(numi);
        }
    }

    int getInteger()
    {
        if (bFloat) {
            return int(numf);
        }
        else {
            return numi;
        }
    }

    void print()
    {
        if (bFloat) {
            printf("%f", numf);
        }
        else {
            printf("%d", numi);
        }
    }
};

class JSONArray : public JSONValue
{
public:
    std::vector<JSONValue*> array;

    JSONArray()
    {
        type = JARRAY;
    }

    int size()
    {
        return int(array.size());
    }

    JSONValue *get(int index)
    {
        return array.at(index);
    }

    void addValue(JSONValue* data) {
        array.push_back(data);
    }

    void print()
    {
        printf("[");
        for (int i = 0; i < array.size(); i++) {
            array[i]->print();
            if (i < (array.size() - 1)) {
                printf(", ");
            }
        }

        printf("]");
    }
};

class JSONObject : public JSONValue
{
public:
    std::vector<std::string> names;
    std::vector<JSONValue*> values;

    JSONObject()
    {
        type = JOBJECT;
    }

    bool empty()
    {
        return names.empty();
    }

    void addName(std::string name) {
        names.push_back(name);
    }

    void addValue(JSONValue* data) {
        values.push_back(data);
    }

    void print()
    {
        printf("{\n");
        int n = int(MIN(names.size(), values.size()));

        for (int i = 0; i < n; i++) {
            printf("   %s:", names[i].c_str());
            values[i]->print();
            if (i < (n - 1)) {
                printf(",\n");
            }
        }
        printf("\n}\n");
    }

    JSONValue* check(std::string key)
    {
        JSONValue* out = NULL;
        for (int i = 0; i < names.size(); i++) {
            if (names[i].compare(key) == 0) {
                out = values[i];
                break;
            }
        }

        return out;
    }

    std::string getString(std::string key)
    {
        std::string out = "";

        JSONValue* tmp = check(key);
        if (tmp != NULL) {
            if (tmp->type == JSTRING) {
                auto tmp2 = (JSONString*)tmp;
                out = tmp2->str;
            }
        }
        return out;
    }

    float getFloat(std::string key)
    {
        float out = 0.0f;

        pic::JSONValue* tmp = check(key);

        if (tmp != NULL) {
            if (tmp->type == JNUMBER) {
                auto tmp2 = (JSONNumber*)tmp;
                return tmp2->getFloat();
            }
        }

        return out;
    }

    int getInteger(std::string key)
    {
        int out = 0;

        pic::JSONValue* tmp = check(key);

        if (tmp != NULL) {
            if (tmp->type == JNUMBER) {
                auto tmp2 = (JSONNumber*)tmp;
                return tmp2->getInteger();
            }
        }

        return out;
    }

    double getDouble(std::string key)
    {
        double out = 0.0;

        pic::JSONValue* tmp = check(key);

        if (tmp != NULL) {
            if (tmp->type == JNUMBER) {
                auto tmp2 = (JSONNumber*)tmp;
                return tmp2->getDouble();
            }
        }

        return out;
    }

    bool getBool(std::string key)
    {
        bool out = false;

        JSONValue* tmp = check(key);
        if (tmp != NULL) {
            if (tmp->type == JTRUE) {
                out = true;
            }

            if (tmp->type == JFALSE) {
                out = false;
            }
        }

        return out;
    }

    void getFloatArray(std::string key, std::vector<float> &out)
    {
        out.clear();

        pic::JSONValue* tmp = check(key);

        if (tmp != NULL) {
            if (tmp->type == pic::JARRAY) {
                pic::JSONArray* arr = (pic::JSONArray*)tmp;
                for (int i = 0; i < arr->size(); i++) {
                    out.push_back(((pic::JSONNumber*)((pic::JSONNumber*)arr->get(i)))->getFloat());
                }
            }
        }
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
     bool parseNumber(std::string str, JSONNumber *ret)
     {
         std::regex j_integer("(\\+|-)?[[:digit:]]+");
         std::regex j_float("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?");
         std::regex j_float_scientific("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");

         if(regex_match(str, j_integer)) {
             ret->bFloat = false;
             ret->numi = atoi(str.c_str());
             return true;
         } else {
             if(std::regex_match(str, j_float)) {
                 ret->bFloat = true;
                 ret->numf = atof(str.c_str());

                 return true;
             } else {
                 if(std::regex_match(str, j_float_scientific)) {
                     ret->bFloat = true;
                     ret->numf = atof(str.c_str());
                     return true;

                 }

                 return false;
             }
         }
     }
      

     bool parseString(std::string str, JSONString &ret)
     {
         std::regex j_string("\"([[:print:]]*[[:space:]]*[[:punct:]]*[[:upper:]]*)*\"");
         //         [\\]*[\n]*[\/]*[\"]*[\b]*[\f]*[\n]*[\r]*[\t]*
         if (regex_match(str, j_string)) {
             ret.str = str;
             ret.str.erase(0, 1);
             ret.str.erase(ret.str.size() -1);
             return true;
         }
         return false;
     }

     bool parseWhitespace(std::string str)
     {
         std::regex j_string("([[:space:]]*[[:blank:]]*\t*\r*\n*)*");
         if (regex_match(str, j_string)) {
             return true;
         }
         return false;
     }

     bool checkWhitespace(char current) {
         std::regex j_string("([[:space:]]*[[:blank:]]*\t*\r*\n*)*");

         std::string str(1, current);
         return regex_match(str, j_string);
     }

     /**
      * @brief testParserNumbers
      */
     void testParserNumbers()
     {
         printf("Test numbers:\n");
         JSONNumber ret;
         parseNumber("121324", &ret);
         printf("\n");
         ret.print();

         printf("\n");

         parseNumber("121.324", &ret);
         ret.print();
         printf("\n");

         parseNumber("1.4e6", &ret);
         ret.print();
         printf("\n");

     }

     void testParserString()
     {
         printf("Test strings:\n");
         JSONString ret;
         parseString("\"number1\"", ret);
         printf("%s\n", ret.str.c_str());

         parseString("\"num ber1\"", ret);
         printf("%s\n", ret.str.c_str());

         parseString("\"./number1\"", ret);
         printf("%s\n", ret.str.c_str());
     }

     std::string parseString(std::string lines, int &c, int &code) {
         std::string tmp_str;
         bool bFlag = true;
         int n = int(lines.size()) - 1;
         while ((c < n) && bFlag) {
             c++;
             if ((lines.at(c) == '}') || (lines.at(c) == ']')) {
                 code = -1;
                 return "";
             }

             if (lines.at(c) == '\"') {
                 while ((c < n) && bFlag) {
                     c++;    
                     if (lines.at(c) == '\\') {
                         c++;
                         switch (lines.at(c)) {
                         case '\"':
                             tmp_str += '\"';
                         case '\\':
                             tmp_str += '\\';
                         case '/':
                             tmp_str += '\/';
                         case 'b':
                             tmp_str += '\b';
                         case 'n':
                             tmp_str += '\n';
                         case 'f':
                             tmp_str += '\f';
                         case 'r':
                             tmp_str += '\r';
                         case 't':
                             tmp_str += '\t';
                         default:
                             tmp_str += '\\';
                             tmp_str += lines.at(c);
                         }
                     } else {
                         if (lines.at(c) == '\"') {
                             bFlag = false;
                         } else {
                             tmp_str += lines.at(c);
                         }
                     }
                 }
             }
         }
         code = 0;
         return tmp_str;
     }

     void find(std::string lines, char toBeFound, int &c) {
         int n = int(lines.size()) - 1;

         while (c < n) {
             if (lines.at(c) != toBeFound) {
                 break;
             }
             c++;
         }
     }

     JSONValue* parse(std::string filename)
     {
         std::ifstream in(filename, std::ios_base::in);

         std::string line;
         std::string lines;
         while (getline(in, line)) {
             lines += "\n" + line;
         }
         bool bParse = true;


         int c = 0;
         JSONValue* last_caller = NULL;
         std::stack<JSONValue*> stack;
         JSONValue* root = NULL;

         int n = int(lines.size()) - 1;
         while (c < n) {

             if (lines.at(c) == '{') {
                 JSONObject* tmp = new JSONObject();
                 tmp->lookingForValue = false;

                 if (last_caller == NULL) {
                     root = (JSONValue*)tmp;
                     last_caller = tmp;
                 } else {
                     stack.push(last_caller);

                     if (last_caller->lookingForValue) {
                         last_caller->addValue((JSONValue*)tmp);
                     }

                     last_caller = tmp;
                 }
             }

             if (lines.at(c) == '[') {
                 JSONArray* tmp = new JSONArray();
                 tmp->lookingForValue = true;

                 if (last_caller == NULL) {
                     root = (JSONValue*)tmp;
                 } else {
                     last_caller->addValue((JSONValue*)tmp);
                     stack.push(last_caller);
                 }
                 last_caller = tmp;
             }

             if (last_caller != NULL) {
                 //parse strings value
                 if (lines.at(c) == '\"') {
                     c--;
                     int code = 0;
                     std::string name = parseString(lines, c, code);
                     if (last_caller->lookingForValue) {
                         last_caller->addValue(new JSONString(name));
                     }

                     if (last_caller->type == JOBJECT) {
                         last_caller->lookingForValue = false;
                     }
                 }
                 

                 //parse true
                 if (lines.at(c) == 't') {
                     std::string tmp_str = lines.substr(c, 4);

                     if (tmp_str.compare("true") == 0) {
                         JSONValue* value = new JSONValue();
                         value->setTrue();
                         last_caller->addValue(value);

                         if (last_caller->type == JOBJECT) {
                             last_caller->lookingForValue = false;
                         }
                     }
                     else {
                         return NULL;
                     }
                 }

                 //parse false
                 if (lines.at(c) == 'f') {
                     std::string tmp_str = lines.substr(c, 5);

                     if (tmp_str.compare("false") == 0) {
                         JSONValue* value = new JSONValue();
                         value->setFalse();
                         last_caller->addValue(value);

                         if (last_caller->type == JOBJECT) {
                             last_caller->lookingForValue = false;
                         }
                     }
                     else {
                         return NULL;
                     }
                 }

                 //parse null
                 if (lines.at(c) == 'n') {
                     std::string tmp_str = lines.substr(c, 4);

                     if (tmp_str.compare("null") == 0) {
                         JSONValue* value = new JSONValue();
                         value->setNull();
                         last_caller->addValue(value);

                         if (last_caller->type == JOBJECT) {
                             last_caller->lookingForValue = false;
                         }
                     }
                     else {
                         return NULL;
                     }
                 }

                 //parse numbers values
                 std::string number(1, lines.at(c));
                 std::regex j_integer("(\\+|-)?[[:digit:]]*");
                 if (regex_match(number, j_integer) && last_caller->lookingForValue) {
                     int c2 = c;
                     while (c2 < n) {
                         c2++;
                         auto current = lines.at(c2);
                         if (checkWhitespace(current) || (current == ',') || (current == '}') || (current == ']')) {
                             break;
                             c2--;
                         }
                     }

                     JSONNumber *out = new JSONNumber();
                     std::string number_only_str = lines.substr(c, c2 - c);
                     parseNumber(number_only_str, out);
                     last_caller->addValue(out);

                     if (last_caller->type == JOBJECT) {
                         last_caller->lookingForValue = false;
                     }
                     c--;
                     c += int(number_only_str.size());
                 }

                 if ((last_caller->type == JOBJECT) && (!last_caller->lookingForValue)) {
                     int code = 0;
                     std::string name = parseString(lines, c, code);
                     if (code == 0) {
                         last_caller->addName(name);
                         find(lines, ':', c);
                         last_caller->lookingForValue = true;
                     }
                 }
             }

             if ((lines.at(c) == '}') || (lines.at(c) == ']')) {


                 if (!stack.empty()) {

                     last_caller->lookingForValue = false;
                     last_caller = stack.top();
                     stack.pop();

                     if (last_caller->type == JOBJECT) {
                         last_caller->lookingForValue = false;
                     }

                     if (last_caller->type == JARRAY) {
                         last_caller->lookingForValue = true;
                     }
                 }
                 else {
                     last_caller = NULL;
                 }
             }

             c++;
         }

         return root;
     }
};

} // end namespace pic

#endif /* PIC_IO_JSON */

