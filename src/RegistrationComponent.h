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

/** \file   RegistrationComponent.h
  * \brief  Defines Registration, RegistrationController
  */

#include "Server.h"
#include "Component.h"

class RegistrationController;



// ----------------------------------------------------------------------------------
// RegistrationComponent
// ----------------------------------------------------------------------------------

/** \brief  %Registration management component.
  *
  * Provides \ref ::RegistrationController "RegistrationController" instance.
  */
class RegistrationComponent : public Component
{

public:

    /** \brief  Instantiates.
      */
    RegistrationComponent( Record::Server& server, ComponentWindowFactory& );

    /** \brief  Does nothing.
      */
    virtual ~RegistrationComponent();

}; // RegistrationComponent
