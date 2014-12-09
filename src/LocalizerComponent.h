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

/** \file   LocalizerComponent.h
  * \brief  Defines LocalizerComponent
  */

#include "Component.h"



// ----------------------------------------------------------------------------------
// LocalizerComponent
// ----------------------------------------------------------------------------------

/** \brief	Provides a \ref LocalizerController.
  *
  * \date   2012
  * \author Leonid Kostrykin
  */
class LocalizerComponent : public Component
{

public:

    /** \brief  Instantiates.
      */
    LocalizerComponent( Record::Server& server, ComponentWindowFactory& );

    /** \brief  Does nothing.
      */
    virtual ~LocalizerComponent();

}; // LocalizerComponent
