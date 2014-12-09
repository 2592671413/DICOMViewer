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

/** \file   ObjectsComponent.h
  * \brief  Defines ObjectsComponent.
  */

#include "Server.h"
#include "Component.h"



// ----------------------------------------------------------------------------------
// ObjectsComponent
// ----------------------------------------------------------------------------------

/** \brief	\ref Component which allows the user to manage \c Carna::base::model::Object3D instances.
  *
  * Provides a \ref ObjectsController:
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class ObjectsComponent : public Component
{

public:

    /** \brief  Instantiates.
      */
    ObjectsComponent( Record::Server& server, ComponentWindowFactory& );

    /** \brief  Does nothing.
      */
    virtual ~ObjectsComponent();

}; // ObjectsComponent
