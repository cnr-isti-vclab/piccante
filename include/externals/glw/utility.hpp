/*************************************************************************/
/*                                                                       */
/*  GLW                                                                  */
/*  OpenGL Wrapper Library                                               */
/*                                                                       */
/*  Copyright (C) 2009                                                   */
/*  Marco Di Benedetto                                                   */
/*  Visual Computing Laboratory                                          */
/*  ISTI - Italian National Research Council (CNR)                       */
/*                                                                       */
/*  This file is part of GLW.                                            */
/*                                                                       */
/*  GLW is free software; you can redistribute it and/or modify          */
/*            */
/*  published by the Free Software Foundation; either version 2.1 of     */
/*                    */
/*                                                                       */
/*  GLW is distributed in the hope that it will be useful, but           */
/*         */
/*                   */
/*                              */
/*  (http://www.fsf.org/licensing/licenses/lgpl.html) for more details.  */
/*                                                                       */
/*************************************************************************/

#ifndef GLW_UTILITY_HPP
#define GLW_UTILITY_HPP

#include <stdio.h>

#include <string>
#include <sstream>

#include "externals/glw/base.hpp"

namespace glw {

/* utility interface */
/**********************************************************/

#define GLW_FIELD_OFFSET(STUCT_TYPE, FIELD_NAME)  (&(((const STUCT_TYPE *)0) -> FIELD_NAME))

inline std::string read_text_file(const char *file_name);

inline std::string define(const char *token);
inline std::string define(const char *token, int value);
inline std::string define(const char *token, float value);
inline std::string define(const char *token, const char *value);

#if 0
inline std::string constant(const char *token, int v0);
inline std::string constant(const char *token, int v0, int v1);
inline std::string constant(const char *token, int v0, int v1, int v2);
inline std::string constant(const char *token, int v0, int v1, int v2, int v3);

inline std::string constant1(const char *token, const int *v);
inline std::string constant2(const char *token, const int *v);
inline std::string constant3(const char *token, const int *v);
inline std::string constant4(const char *token, const int *v);
#endif

inline std::string constant(const char *token, float v0);
inline std::string constant(const char *token, float v0, float v1);
inline std::string constant(const char *token, float v0, float v1, float v2);
inline std::string constant(const char *token, float v0, float v1, float v2,
                            float v3);

#if 0
inline std::string constant1(const char *token, const float *v);
inline std::string constant2(const char *token, const float *v);
inline std::string constant3(const char *token, const float *v);
inline std::string constant4(const char *token, const float *v);
#endif

inline std::string version(const char *token);

inline std::string ext_enable(const char *token);
inline std::string ext_disable(const char *token);
inline std::string ext_require(const char *token);
inline std::string ext_warning(const char *token);
/**********************************************************/



/* utility implementation */
/**********************************************************/

inline std::string read_text_file(const char *file_name)
{
    assert(file_name != 0);

    std::string r;

    FILE *f = fopen(file_name, "rb");

    if(f == 0) {
        return r;
    }

    fseek(f, 0, SEEK_END);
    const size_t sz = size_t(ftell(f));
    rewind(f);

    char *str = new char [sz + 1];
    fread(str, sizeof(char), sz / sizeof(char), f);
    fclose(f);

    str[sz] = '\0';
    r = str;
    delete [] str;

    return r;
}

inline std::string define(const char *token)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#define " << token << " 1" << std::endl;

    return ss.str();
}

inline std::string define(const char *token, int value)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#define " << token << " (int(" << value << "))" << std::endl;

    return ss.str();
}

inline std::string define(const char *token, float value)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#define " << token << " (float(" << value << "))" << std::endl;

    return ss.str();
}

inline std::string define(const char *token, const char *value)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#define " << token << " (" << value << ")" << std::endl;

    return ss.str();
}

inline std::string constant(const char *token, float v0)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "const float " << token << " = float(" << v0 << ");" << std::endl;

    return ss.str();
}

inline std::string constant(const char *token, float v0, float v1)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "const vec2 " << token << " = vec2(float(" << v0 << "), float(" << v1 <<
       "));" << std::endl;

    return ss.str();
}

inline std::string constant(const char *token, float v0, float v1, float v2)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "const vec3 " << token << " = vec3(float(" << v0 << "), float(" << v1 <<
       "), float(" << v2 << "));" << std::endl;

    return ss.str();
}

inline std::string constant(const char *token, float v0, float v1, float v2,
                            float v3)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "const vec4 " << token << " = vec4(float(" << v0 << "), float(" << v1 <<
       "), float(" << v2 << "), float(" << v3 << "));" << std::endl;

    return ss.str();
}

inline std::string version(const char *token)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#version " << token << std::endl;

    return ss.str();
}

inline std::string ext_enable(const char *token)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#extension " << token << " : enable" << std::endl;

    return ss.str();
}

inline std::string ext_disable(const char *token)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#extension " << token << " : disable" << std::endl;

    return ss.str();
}

inline std::string ext_require(const char *token)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#extension " << token << " : require" << std::endl;

    return ss.str();
}

inline std::string ext_warning(const char *token)
{
    GLW_ASSERT(token != 0);

    std::ostringstream ss;
    ss << "#extension " << token << " : warn" << std::endl;

    return ss.str();
}

/**********************************************************/

} // end namespace

#endif // GLW_UTILITY_HPP
