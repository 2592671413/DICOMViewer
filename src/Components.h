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

#include "Server.h"

class Component;



// ----------------------------------------------------------------------------------
// Components
// ----------------------------------------------------------------------------------

/** \brief	Defines the components service.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class Components : public Record::GenericService< Components >
{

public:

    /** \brief	Closes all components.
      */
    virtual void releaseAll() = 0;

    /** \brief	Closes certain component.
      */
    virtual void release( const Component& ) = 0;
    
    /** \brief	Stores the given component.
      *
      * The ownership of the given component is delegated to the components service provider.
      */
    virtual void takeComponent( Component* ) = 0;
    
}; // CarnaContext


template< >
const std::string Record::GenericService< Components >::serviceID = "Components";
