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

#ifndef PIC_UTIL_GL_BUFFER_OPS_HPP
#define PIC_UTIL_GL_BUFFER_OPS_HPP

#include <vector>
#include <map>
#include <thread>
#include <mutex>

#include "../../base.hpp"

#include "../../util/gl/buffer_op.hpp"

namespace pic {

enum BOGL{BOGL_ADD, BOGL_SUB, BOGL_MUL, BOGL_DIV,
          BOGL_ADD_CONST, BOGL_SUB_CONST, BOGL_MUL_CONST, BOGL_DIV_CONST,
          BOGL_ADD_S, BOGL_SUB_S, BOGL_MUL_S, BOGL_DIV_S,
          BOGL_CLAMP, BOGL_ID, BOGL_ID_CONST};

typedef std::vector<BufferOpGL*> BufferOperatorsGL;

/**
 * @brief The BufferOpsGL class
 */
class BufferOpsGL
{
public:
    BufferOperatorsGL list;

    /**
     * @brief getInstance
     * @return
     */
    static BufferOpsGL* getInstance()
    {
        std::thread::id this_id = std::this_thread::get_id();

        if(!flag[this_id]) {
            std::lock_guard<std::mutex> lock(mutex);

            if(buffer_ops_gl[this_id] == NULL) {
                buffer_ops_gl[this_id] = new BufferOpsGL();
                flag[this_id] = true;
            }
        }

        return buffer_ops_gl[this_id];
    }

    ~BufferOpsGL()
    {
    }

private:
    static std::mutex mutex;
    static std::map<std::thread::id, bool> flag;
    static std::map<std::thread::id, BufferOpsGL*> buffer_ops_gl;

    /**
     * @brief BufferOpsGL
     */
    BufferOpsGL()
    {
        list.push_back(new BufferOpGL("I0 + I1", true));
        list.push_back(new BufferOpGL("I0 - I1", true));
        list.push_back(new BufferOpGL("I0 * I1", true));
        list.push_back(new BufferOpGL("I0 / I1", true));

        list.push_back(new BufferOpGL("I0 + C0", true));
        list.push_back(new BufferOpGL("I0 - C0", true));
        list.push_back(new BufferOpGL("I0 * C0", true));
        list.push_back(new BufferOpGL("I0 / C0", true));

        list.push_back(new BufferOpGL("I0 + I1x", true));
        list.push_back(new BufferOpGL("I0 - I1x", true));
        list.push_back(new BufferOpGL("I0 * I1x", true));
        list.push_back(new BufferOpGL("I0 / I1x", true));

        list.push_back(new BufferOpGL("clamp(I0, C0, C1)", true));

        list.push_back(new BufferOpGL("I1", true));
        list.push_back(new BufferOpGL("C0", true));
    }

};

std::mutex BufferOpsGL::mutex;

std::map<std::thread::id, bool> BufferOpsGL::flag;

std::map<std::thread::id, BufferOpsGL*> BufferOpsGL::buffer_ops_gl;

} // end namespace pic

#endif /* PIC_UTIL_GL_BUFFER_OPS_HPP */
