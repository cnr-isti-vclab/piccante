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

#ifndef PIC_GL_BUFFER_OPS_GL_HPP
#define PIC_GL_BUFFER_OPS_GL_HPP

#include <vector>
#include <mutex>
#include "util/gl/buffer_op.hpp"

namespace pic {

typedef std::vector<BufferOpGL*> BufferOperatorsGL;

class BufferOpsGL{
public:
    std::lock_guard<std::mutex> lock(mutex);

    BufferOperatorsGL list;

    static BufferOpsGL* getInstance()
    {
        if(!flag) {
            buffer_ops_gl = new BufferOpsGL();
            flag = true;
        }

        return buffer_ops_gl;
    }

    ~BufferOpsGL()
    {
        flag = false;
    }

private:
    static bool flag;
    static BufferOpsGL *buffer_ops_gl;

    BufferOpsGL()
    {
        list.push_back(new BufferOpGL("I0 +  I1", true));
        list.push_back(new BufferOpGL("I0 *  I1", true));
        list.push_back(new BufferOpGL("I0 /  I1", true));
        list.push_back(new BufferOpGL("I0 -  I1", true));

        list.push_back(new BufferOpGL("I0 +  C0", true));
        list.push_back(new BufferOpGL("I0 *  C0", true));
        list.push_back(new BufferOpGL("I0 /  C0", true));
        list.push_back(new BufferOpGL("I0 -  C0", true));
    }

};

bool BufferOpsGL::flag = false;
BufferOpsGL* BufferOpsGL::buffer_ops_gl = NULL;

} // end namespace pic

#endif /* PIC_GL_BUFFER_OPS_GL_HPP */
