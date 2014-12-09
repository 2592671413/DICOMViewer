/*
 *  Copyright (C) 2010 - 2013 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#ifndef GLEW_H_4351435556
#define GLEW_H_4351435556

/** \file   glew.h
  * \brief  Includes \c GL/glew.h and \c windows.h if \c _WIN32 is defined.
  *
  * \author Leonid Kostrykin
  * \date   24.2.2011
  */

#ifdef _WIN32   // ----------------------- Win32 -----------------------

    #include <windows.h>

#endif          // -----------------------------------------------------
#include <GL/glew.h>



#endif // GLEW_H_4351435556
