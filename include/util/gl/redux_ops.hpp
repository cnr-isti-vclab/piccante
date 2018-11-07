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

#ifndef PIC_UTIL_GL_REDUX_OPS_HPP
#define PIC_UTIL_GL_REDUX_OPS_HPP

#include <vector>
#include <map>
#include <thread>
#include <mutex>

#include "../../util/gl/redux.hpp"

namespace pic {

enum REDGL{REDGL_MIN, REDGL_MAX, REDGL_SUM, REDGL_MEAN, REDGL_LOG_MEAN};

typedef std::vector<ReduxGL*> ReduxOperatorsGL;

/**
 * @brief The BufferOpsGL class
 */
class ReduxOpsGL
{
public:
    ReduxOperatorsGL list;

    /**
     * @brief getInstance
     * @return
     */
    static ReduxOpsGL* getInstance()
    {
        std::thread::id this_id = std::this_thread::get_id();

        if(!flag[this_id]) {
            std::lock_guard<std::mutex> lock(mutex);

            if(redux_ops_gl[this_id] == NULL) {
                redux_ops_gl[this_id] = new ReduxOpsGL();
                flag[this_id] = true;
            }
        }

        return redux_ops_gl[this_id];
    }

    ~ReduxOpsGL()
    {
    }

private:
    static std::mutex mutex;
    static std::map<std::thread::id, bool> flag;
    static std::map<std::thread::id, ReduxOpsGL*> redux_ops_gl;

    /**
     * @brief ReduxOpsGL
     */
    ReduxOpsGL()
    {
        list.push_back(ReduxGL::createMin());
        list.push_back(ReduxGL::createMax());
        list.push_back(ReduxGL::createSum());
        list.push_back(ReduxGL::createMean());
        list.push_back(ReduxGL::createLogMean());
    }

};

std::mutex ReduxOpsGL::mutex;

std::map<std::thread::id, bool> ReduxOpsGL::flag;

std::map<std::thread::id, ReduxOpsGL*> ReduxOpsGL::redux_ops_gl;

} // end namespace pic

#endif /* PIC_UTIL_GL_REDUX_OPS_HPP */
