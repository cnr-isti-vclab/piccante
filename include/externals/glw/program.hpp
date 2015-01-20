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

#ifndef GLW_PROGRAM_HPP
#define GLW_PROGRAM_HPP

#include <string>

#include "externals/glw/base.hpp"
#include "externals/glw/utility.hpp"

namespace glw {

/* program interface */
/**********************************************************/

class program
{
    //GLW_DISABLE_COPY(program)

public:

    typedef program this_type;
    typedef void    base_type;

    inline  program(void);
    inline ~program(void);

    inline bool setup(const char *prefix, const char *vertex_src,
                      const char *fragment_src);
    inline bool setup(const char *prefix, const char *vertex_src,
                      const char *geometry_src, const char *fragment_src, GLenum primitive_input_type,
                      GLenum primitive_output_type, int max_output_vertices);
    inline bool setup(const std::string &prefix, const std::string &vertex_src,
                      const std::string &fragment_src);
    inline bool setup(const std::string &prefix, const std::string &vertex_src,
                      const std::string &geometry_src, const std::string &fragment_src,
                      GLenum primitive_input_type, GLenum primitive_output_type,
                      int max_output_vertices);

    inline bool loadup(const char *prefix, const char *vertex_file_name,
                       const char  *fragment_file_name);
    inline bool loadup(const std::string &prefix,
                       const std::string &vertex_file_name, const std::string &fragment_file_name);
    inline bool loadup(const char *prefix, const char *vertex_file_name,
                       const char  *geometry_file_name, const char *fragment_file_name,
                       GLenum primitive_input_type, GLenum primitive_output_type,
                       int max_output_vertices);
    inline bool loadup(const std::string &prefix,
                       const std::string &vertex_file_name, const std::string &geometry_file_name,
                       const std::string &fragment_file_name, GLenum primitive_input_type,
                       GLenum primitive_output_type, int max_output_vertices);

    inline void cleanup(void);
    inline bool valid(void) const;
    inline void reset(void);

    inline bool relink(void);

    inline std::string &log(void);

    inline GLuint id(void) const;

    inline GLuint program_id(void) const;
    inline GLuint vertex_shader_id(void) const;
    inline GLuint geometry_shader_id(void) const;
    inline GLuint fragment_shader_id(void) const;

    inline GLint attribute_location(const char *name);
    inline void  attribute_source(const char *name, unsigned int index);

    inline GLint fragment_location(const char *name);
    inline void  fragment_target(const char *name, unsigned int color_index);

    inline GLint uniform_location(const char *name);

    inline void uniform_buffer(GLint location, GLuint buffer);

    inline void uniform(GLint location, int v0);
    inline void uniform(GLint location, int v0, int v1);
    inline void uniform(GLint location, int v0, int v1, int v2);
    inline void uniform(GLint location, int v0, int v1, int v2, int v3);

    inline void uniform1(GLint location, const int *v, int count = 1);
    inline void uniform2(GLint location, const int *v, int count = 1);
    inline void uniform3(GLint location, const int *v, int count = 1);
    inline void uniform4(GLint location, const int *v, int count = 1);

    inline void uniform(GLint location, float v0);
    inline void uniform(GLint location, float v0, float v1);
    inline void uniform(GLint location, float v0, float v1, float v2);
    inline void uniform(GLint location, float v0, float v1, float v2, float v3);

    inline void uniform1(GLint location, const float *v, int count = 1);
    inline void uniform2(GLint location, const float *v, int count = 1);
    inline void uniform3(GLint location, const float *v, int count = 1);
    inline void uniform4(GLint location, const float *v, int count = 1);

    inline void uniform3x3(GLint location, const float *values, bool transpose,
                           int count = 1);
    inline void uniform4x4(GLint location, const float *values, bool transpose,
                           int count = 1);

    inline void uniform_buffer(const char *name, GLuint buffer);

    inline void uniform(const char *name, int v0);
    inline void uniform(const char *name, int v0, int v1);
    inline void uniform(const char *name, int v0, int v1, int v2);
    inline void uniform(const char *name, int v0, int v1, int v2, int v3);

    inline void uniform1(const char *name, const int *v, int count = 1);
    inline void uniform2(const char *name, const int *v, int count = 1);
    inline void uniform3(const char *name, const int *v, int count = 1);
    inline void uniform4(const char *name, const int *v, int count = 1);

    inline void uniform(const char *name, float v0);
    inline void uniform(const char *name, float v0, float v1);
    inline void uniform(const char *name, float v0, float v1, float v2);
    inline void uniform(const char *name, float v0, float v1, float v2, float v3);

    inline void uniform1(const char *name, const float *v, int count = 1);
    inline void uniform2(const char *name, const float *v, int count = 1);
    inline void uniform3(const char *name, const float *v, int count = 1);
    inline void uniform4(const char *name, const float *v, int count = 1);

    inline void uniform3x3(const char *name, const float *values, bool transpose,
                           int count = 1);
    inline void uniform4x4(const char *name, const float *values, bool transpose,
                           int count = 1);

protected:

    GLuint  m_vshd;
    GLuint  m_gshd;
    GLuint  m_fshd;

    GLuint  m_prog;

    std::string m_log;

    inline void init_state(void);
    inline void term_state(void);

    inline void silent_cleanup(void);

    inline bool do_setup(const char *prefix, const char *vertex_src,
                         const char *geometry_src, const char *fragment_src, GLenum primitive_input_type,
                         GLenum primitive_output_type, int max_output_vertices);

    inline GLuint create_shader(GLenum type, const char *prefix, const char *src,
                                std::string &compile_log);
    inline GLuint create_program(GLuint vs, GLuint gs, GLuint fs,
                                 GLenum primitive_input_type, GLenum primitive_output_type,
                                 int max_output_vertices, std::string &compile_log);
};

inline bool has_program(void);

inline void bind_program(program &p);
inline void bind_program(program *p);

/**********************************************************/



/* program implementation */
/**********************************************************/

#if GLW_ASSERT_VALID_UNIFORM
#define GLW_ASSERT_ULOC(X)  GLW_ASSERT(location >= 0)
#else
#define GLW_ASSERT_ULOC(X)  (void)(X)
#endif

inline program::program(void)
{
    this->init_state();
}

inline program::~program(void)
{
    this->term_state();
}

inline bool program::setup(const char *prefix, const char *vertex_src,
                           const char *fragment_src)
{
    GLW_ASSERT(vertex_src   != 0);
    GLW_ASSERT(fragment_src != 0);

    const bool r = this->do_setup(prefix, vertex_src, 0, fragment_src, GL_TRIANGLES,
                                  GL_TRIANGLE_STRIP, 3);

    return r;
}

inline bool program::setup(const std::string &prefix,
                           const std::string &vertex_src, const std::string &fragment_src)
{
    return this->setup(prefix.c_str(), vertex_src.c_str(), fragment_src.c_str());
}

inline bool program::setup(const char *prefix, const char *vertex_src,
                           const char *geometry_src, const char *fragment_src, GLenum primitive_input_type,
                           GLenum primitive_output_type, int max_output_vertices)
{
    GLW_ASSERT(vertex_src   != 0);
    GLW_ASSERT(geometry_src != 0);
    GLW_ASSERT(fragment_src != 0);

    const bool r = this->do_setup(prefix, vertex_src, geometry_src, fragment_src,
                                  primitive_input_type, primitive_output_type, max_output_vertices);

    return r;
}

inline bool program::setup(const std::string &prefix,
                           const std::string &vertex_src, const std::string &geometry_src,
                           const std::string &fragment_src, GLenum primitive_input_type,
                           GLenum primitive_output_type, int max_output_vertices)
{
    return this->setup(prefix.c_str(), vertex_src.c_str(), geometry_src.c_str(),
                       fragment_src.c_str(), primitive_input_type, primitive_output_type,
                       max_output_vertices);
}

inline bool program::loadup(const char *prefix, const char *vertex_file_name,
                            const char *fragment_file_name)
{
    return this->setup(prefix, read_text_file(vertex_file_name).c_str(),
                       read_text_file(fragment_file_name).c_str());
}

inline bool program::loadup(const std::string &prefix,
                            const std::string &vertex_file_name, const std::string &fragment_file_name)
{
    return this->loadup(prefix.c_str(), vertex_file_name.c_str(),
                        fragment_file_name.c_str());
}

inline bool program::loadup(const char *prefix, const char *vertex_file_name,
                            const char *geometry_file_name, const char *fragment_file_name,
                            GLenum primitive_input_type, GLenum primitive_output_type,
                            int max_output_vertices)
{
    return this->setup(prefix, read_text_file(vertex_file_name).c_str(),
                       read_text_file(geometry_file_name).c_str(),
                       read_text_file(fragment_file_name).c_str(), primitive_input_type,
                       primitive_output_type, max_output_vertices);
}

inline bool program::loadup(const std::string &prefix,
                            const std::string &vertex_file_name, const std::string &geometry_file_name,
                            const std::string &fragment_file_name, GLenum primitive_input_type,
                            GLenum primitive_output_type, int max_output_vertices)
{
    return this->loadup(prefix.c_str(), vertex_file_name.c_str(),
                        geometry_file_name.c_str(), fragment_file_name.c_str(), primitive_input_type,
                        primitive_output_type, max_output_vertices);
}

inline void program::cleanup(void)
{
#if GLW_ASSERT_SETUP_CLEANUP
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
#endif

    this->silent_cleanup();
}

inline bool program::valid(void) const
{
    return (this->m_prog != GLW_NULL_OBJECT);
}

inline void program::reset(void)
{
    this->silent_cleanup();
    this->m_log.clear();
}

inline bool program::relink(void)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);

    glLinkProgram(this->m_prog);

    GLint ls = 0;
    glGetProgramiv(this->m_prog, GL_LINK_STATUS, &ls);
    return (ls != GL_FALSE);
}

inline std::string &program::log(void)
{
    return this->m_log;
}

inline GLuint program::id(void) const
{
    return this->program_id();
}

inline GLuint program::program_id(void) const
{
    return this->m_prog;
}

inline GLuint program::vertex_shader_id(void) const
{
    return this->m_vshd;
}

inline GLuint program::geometry_shader_id(void) const
{
    return this->m_gshd;
}

inline GLuint program::fragment_shader_id(void) const
{
    return this->m_fshd;
}

inline GLint program::attribute_location(const char *name)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(name         != 0);

    const GLint loc = glGetAttribLocation(this->m_prog, name);
    return loc;
}

inline void program::attribute_source(const char *name, unsigned int index)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(name         != 0);

    glBindAttribLocation(this->m_prog, GLuint(index), name);
}

inline GLint program::fragment_location(const char *name)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(name         != 0);

    const GLint loc = glGetFragDataLocation(this->m_prog, name);
    return loc;
}

inline void program::fragment_target(const char *name, unsigned int color_index)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(name         != 0);

    glBindFragDataLocation(this->m_prog, GLuint(color_index), name);
}

inline GLint program::uniform_location(const char *name)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(name         != 0);

    const GLint loc = glGetUniformLocation(this->m_prog, name);
    return loc;
}

inline void program::uniform_buffer(GLint location, GLuint buffer)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(buffer       != 0);
    GLW_ASSERT_ULOC(location);

    //glUniformBufferEXT(this->m_prog, location, buffer);
}

inline void program::uniform(GLint location, int v0)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform1i(location, GLint(v0));
}

inline void program::uniform(GLint location, int v0, int v1)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform2i(location, GLint(v0), GLint(v1));
}

inline void program::uniform(GLint location, int v0, int v1, int v2)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform3i(location, GLint(v0), GLint(v1), GLint(v2));
}

inline void program::uniform(GLint location, int v0, int v1, int v2, int v3)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform4i(location, GLint(v0), GLint(v1), GLint(v2), GLint(v3));
}

inline void program::uniform1(GLint location, const int *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        >  0);
    GLW_ASSERT_ULOC(location);

    glUniform1iv(location, GLsizei(count), (const GLint *)(v));
}

inline void program::uniform2(GLint location, const int *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        >  0);
    GLW_ASSERT_ULOC(location);

    glUniform2iv(location, GLsizei(count), (const GLint *)(v));
}

inline void program::uniform3(GLint location, const int *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        >  0);
    GLW_ASSERT_ULOC(location);

    glUniform3iv(location, GLsizei(count), (const GLint *)(v));
}

inline void program::uniform4(GLint location, const int *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        >  0);
    GLW_ASSERT_ULOC(location);

    glUniform4iv(location, GLsizei(count), (const GLint *)(v));
}

inline void program::uniform(GLint location, float v0)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform1f(location, GLfloat(v0));
}

inline void program::uniform(GLint location, float v0, float v1)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform2f(location, GLfloat(v0), GLfloat(v1));
}

inline void program::uniform(GLint location, float v0, float v1, float v2)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform3f(location, GLfloat(v0), GLfloat(v1), GLfloat(v2));
}

inline void program::uniform(GLint location, float v0, float v1, float v2,
                             float v3)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT_ULOC(location);

    glUniform4f(location, GLfloat(v0), GLfloat(v1), GLfloat(v2), GLfloat(v3));
}

inline void program::uniform1(GLint location, const float *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        > 0);
    GLW_ASSERT_ULOC(location);

    glUniform1fv(location, GLsizei(count), (const GLfloat *)(v));
}

inline void program::uniform2(GLint location, const float *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        > 0);
    GLW_ASSERT_ULOC(location);

    glUniform2fv(location, GLsizei(count), (const GLfloat *)(v));
}

inline void program::uniform3(GLint location, const float *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        > 0);
    GLW_ASSERT_ULOC(location);

    glUniform3fv(location, GLsizei(count), (const GLfloat *)(v));
}

inline void program::uniform4(GLint location, const float *v, int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        > 0);
    GLW_ASSERT_ULOC(location);

    glUniform4fv(location, GLsizei(count), (const GLfloat *)(v));
}

inline void program::uniform3x3(GLint location, const float *v, bool transpose,
                                int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        > 0);
    GLW_ASSERT_ULOC(location);

    const GLboolean t = (transpose) ? (GL_TRUE) : (GL_FALSE);

    glUniformMatrix3fv(location, GLsizei(count), t, (const GLfloat *)(v));
}

inline void program::uniform4x4(GLint location, const float *v, bool transpose,
                                int count)
{
    GLW_ASSERT(this->m_prog != GLW_NULL_OBJECT);
    GLW_ASSERT(v            != 0);
    GLW_ASSERT(count        > 0);
    GLW_ASSERT_ULOC(location);

    const GLboolean t = (transpose) ? (GL_TRUE) : (GL_FALSE);

    glUniformMatrix4fv(location, GLsizei(count), t, (const GLfloat *)(v));
}

inline void program::uniform_buffer(const char *name, GLuint buffer)
{
    this->uniform_buffer(this->uniform_location(name), buffer);
}

inline void program::uniform(const char *name, int v0)
{
    this->uniform(this->uniform_location(name), v0);
}

inline void program::uniform(const char *name, int v0, int v1)
{
    this->uniform(this->uniform_location(name), v0, v1);
}

inline void program::uniform(const char *name, int v0, int v1, int v2)
{
    this->uniform(this->uniform_location(name), v0, v1, v2);
}

inline void program::uniform(const char *name, int v0, int v1, int v2, int v3)
{
    this->uniform(this->uniform_location(name), v0, v1, v2, v3);
}

inline void program::uniform1(const char *name, const int *v, int count)
{
    this->uniform1(this->uniform_location(name), v, count);
}

inline void program::uniform2(const char *name, const int *v, int count)
{
    this->uniform2(this->uniform_location(name), v, count);
}

inline void program::uniform3(const char *name, const int *v, int count)
{
    this->uniform3(this->uniform_location(name), v, count);
}

inline void program::uniform4(const char *name, const int *v, int count)
{
    this->uniform4(this->uniform_location(name), v, count);
}

inline void program::uniform(const char *name, float v0)
{
    this->uniform(this->uniform_location(name), v0);
}

inline void program::uniform(const char *name, float v0, float v1)
{
    this->uniform(this->uniform_location(name), v0, v1);
}

inline void program::uniform(const char *name, float v0, float v1, float v2)
{
    this->uniform(this->uniform_location(name), v0, v1, v2);
}

inline void program::uniform(const char *name, float v0, float v1, float v2,
                             float v3)
{
    this->uniform(this->uniform_location(name), v0, v1, v2, v3);
}

inline void program::uniform1(const char *name, const float *v, int count)
{
    this->uniform1(this->uniform_location(name), v, count);
}

inline void program::uniform2(const char *name, const float *v, int count)
{
    this->uniform2(this->uniform_location(name), v, count);
}

inline void program::uniform3(const char *name, const float *v, int count)
{
    this->uniform3(this->uniform_location(name), v, count);
}

inline void program::uniform4(const char *name, const float *v, int count)
{
    this->uniform4(this->uniform_location(name), v, count);
}

inline void program::uniform3x3(const char *name, const float *v,
                                bool transpose, int count)
{
    this->uniform3x3(this->uniform_location(name), v, transpose, count);
}

inline void program::uniform4x4(const char *name, const float *v,
                                bool transpose, int count)
{
    this->uniform4x4(this->uniform_location(name), v, transpose, count);
}

inline void program::init_state(void)
{
    this->m_vshd = GLW_NULL_OBJECT;
    this->m_gshd = GLW_NULL_OBJECT;
    this->m_fshd = GLW_NULL_OBJECT;
    this->m_prog = GLW_NULL_OBJECT;
}

inline void program::term_state(void)
{
    if(this->m_prog != GLW_NULL_OBJECT) {
        glDeleteProgram(this->m_prog);
        this->m_prog = GLW_NULL_OBJECT;
    }

    if(this->m_fshd != GLW_NULL_OBJECT) {
        glDeleteShader(this->m_fshd);
        this->m_fshd = GLW_NULL_OBJECT;
    }

    if(this->m_gshd != GLW_NULL_OBJECT) {
        glDeleteShader(this->m_gshd);
        this->m_gshd = GLW_NULL_OBJECT;
    }

    if(this->m_vshd != GLW_NULL_OBJECT) {
        glDeleteShader(this->m_vshd);
        this->m_vshd = GLW_NULL_OBJECT;
    }
}

inline void program::silent_cleanup(void)
{
    this->term_state();
    this->init_state();
}

inline bool program::do_setup(const char *prefix, const char *vertex_src,
                              const char *geometry_src, const char *fragment_src, GLenum primitive_input_type,
                              GLenum primitive_output_type, int max_output_vertices)
{
#if GLW_ASSERT_SETUP_CLEANUP
    GLW_ASSERT(this->m_prog == GLW_NULL_OBJECT);
#endif

    this->silent_cleanup();

    this->m_log.clear();
    this->m_log += "----------------------------\n";
    this->m_log += "\n";

    this->m_vshd = this->create_shader(GL_VERTEX_SHADER, prefix, vertex_src,
                                       this->m_log);

    if(geometry_src != 0) {
        this->m_gshd = this->create_shader(GL_GEOMETRY_SHADER, prefix, geometry_src,
                                           this->m_log);
    }

    this->m_fshd = this->create_shader(GL_FRAGMENT_SHADER, prefix, fragment_src,
                                       this->m_log);

    this->m_prog = this->create_program(this->m_vshd, this->m_gshd, this->m_fshd,
                                        primitive_input_type, primitive_output_type, max_output_vertices, this->m_log);

    this->m_log += "----------------------------\n";
    this->m_log += "\n";

    if(this->m_prog == 0) {
        this->silent_cleanup();
        return false;
    }

    return true;
}

inline GLuint program::create_shader(GLenum type, const char *prefix,
                                     const char *src, std::string &compile_log)
{
    GLuint shd = glCreateShader(type);

    std::string ss;

    if(prefix != 0) {
        ss += prefix;
        ss += "\n\n";
    }

    ss += src;

    const char *s = ss.c_str();

    glShaderSource(shd, 1, &s, 0);
    glCompileShader(shd);

    compile_log += "*********************\n";
    compile_log += "[";

    switch(type) {
    case GL_VERTEX_SHADER:
        compile_log += "VERTEX";
        break;

    case GL_GEOMETRY_SHADER:
        compile_log += "GEOMETRY";
        break;

    case GL_FRAGMENT_SHADER:
        compile_log += "FRAGMENT";
        break;

    default:
        compile_log += "UNKNOWN";
        break;
    }

    compile_log += " SHADER LOG]\n";

    GLint cs = 0;
    glGetShaderiv(shd, GL_COMPILE_STATUS, &cs);

    if(cs != GL_FALSE) {
        compile_log += "SUCCESS";
    } else {
        compile_log += "FAILED";
    }

    compile_log += "\n";

    GLint len = 0;
    glGetShaderiv(shd, GL_INFO_LOG_LENGTH, &len);

    if(len > 0) {
        char *cl = new char[len + 1];
        glGetShaderInfoLog(shd, len, &len, cl);

        if(len > 0) {
            cl[len - 1] = '\0';
            compile_log += cl;
        }

        delete [] cl;
    } else {
        compile_log += "--- NO LOG ---";
    }

    compile_log += "\n";

    compile_log += "*********************\n";
    compile_log += "\n";

    if(cs == GL_FALSE) {
        glDeleteShader(shd);
        return 0;
    }

    return shd;
}

inline GLuint program::create_program(GLuint vs, GLuint gs, GLuint fs,
                                      GLenum primitive_input_type, GLenum primitive_output_type,
                                      int max_output_vertices, std::string &compile_log)
{
    GLuint prog = glCreateProgram();

    if(vs != 0) {
        glAttachShader(prog, vs);
    }

    if(gs != 0) {
        glAttachShader(prog, gs);
    }

    if(fs != 0) {
        glAttachShader(prog, fs);
    }

    if(gs != 0) {
        /*
        glProgramParameteriEXT(prog, GL_GEOMETRY_INPUT_TYPE_EXT,
                               GLint(primitive_input_type));
        glProgramParameteriEXT(prog, GL_GEOMETRY_OUTPUT_TYPE_EXT,
                               GLint(primitive_output_type));
        glProgramParameteriEXT(prog, GL_GEOMETRY_VERTICES_OUT_EXT,
                               GLint(max_output_vertices));
        */
    }

    glLinkProgram(prog);

    compile_log += "*********************\n";
    compile_log += "[PROGRAM LOG]\n";

    GLint ls = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ls);

    if(ls != GL_FALSE) {
        compile_log += "SUCCESS";
    } else {
        compile_log += "FAILED";
    }

    compile_log += "\n";

    GLint len = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);

    if(len > 0) {
        char *cl = new char[len + 1];
        glGetProgramInfoLog(prog, len, &len, cl);

        if(len > 0) {
            cl[len - 1] = '\0';
            compile_log += cl;
        }

        delete [] cl;
    } else {
        compile_log += "--- NO LOG ---";
    }

    compile_log += "\n";

    compile_log += "*********************\n";
    compile_log += "\n";

    if(ls == GL_FALSE) {
        glDeleteProgram(prog);
        return 0;
    }

    return prog;
}

inline bool has_program(void)
{
/*    if(GLEW_VERSION_2_0) {
        return true;
    }
    return false;*/
    return true;

}

inline void bind_program(program &p)
{
    GLuint obj = p.program_id();
    GLW_ASSERT(obj != GLW_NULL_OBJECT);
    glUseProgram(obj);
}

inline void bind_program(program *p)
{
    if(p != 0) {
        bind_program(*p);
    } else {
        glUseProgram(0);
    }
}

/**********************************************************/

} // end namespace

#endif // GLW_PROGRAM_HPP
