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

#ifndef GLW_BASE_HPP
#define GLW_BASE_HPP

#include <assert.h>

//#include <GL/glew.h>

#define GLW_DISABLE_ASSERT         0
#define GLW_ASSERT_SETUP_CLEANUP   0
#define GLW_ASSERT_VALID_UNIFORM   0
#define GLW_CHECK_PARAMETERS       1
#define GLW_KEEP_BINDING           1

/* base macros */
/**********************************************************/

#if GLW_DISABLE_ASSERT
#define GLW_ASSERT(X)
#else
#define GLW_ASSERT(X)      assert(X)
#endif

#define GLW_STRINGFY(S)    #S

#define GLW_NULL_OBJECT    0

#define GLW_DISABLE_COPY(CLASS_NAME) \
	private: \
		CLASS_NAME (const CLASS_NAME & c); \
		CLASS_NAME & operator = (const CLASS_NAME & c);


/**********************************************************/



namespace glw {

/* base interface */
/**********************************************************/

inline void initialize(void);
inline void terminate(void);

/**********************************************************/



/* base implementation */
/**********************************************************/

inline void initialize(void)
{
    //glewInit();
}

inline void terminate(void)
{
    ;
}

/**********************************************************/

} // end namespace

#endif // GLW_BASE_HPP
