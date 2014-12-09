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

#pragma once

/** \file   WindowingComponent.h
  * \brief  Defines WindowingComponent
  */

#include "Component.h"

class QSlider;
class QLabel;



// ----------------------------------------------------------------------------------
// WindowingComponent
// ----------------------------------------------------------------------------------

/** \brief  Defines windowing configuration component.
  *
  * Provides a WindowingController.
  *
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class WindowingComponent : public Component
{

public:

    /** \brief  Instantiates.
      */
    WindowingComponent( Record::Server& server, ComponentWindowFactory& );

}; // WindowingComponent
