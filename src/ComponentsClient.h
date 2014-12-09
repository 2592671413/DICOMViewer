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

#include "Components.h"



// ----------------------------------------------------------------------------------
// ComponentsClient
// ----------------------------------------------------------------------------------

/** \brief	Defines the components service provider.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class ComponentsClient : public Record::Client< Components >
{

public:
    
    /** \copydoc Record::Client::Client
      */
    ComponentsClient( Record::Server& server )
        : Client( server )
    {
    }


    virtual void releaseAll() override
    {
        destination.releaseAll();
    }

    virtual void release( const Component& component ) override
    {
        destination.release( component );
    }

    virtual void takeComponent( Component* component ) override
    {
        destination.takeComponent( component );
    }

}; // ComponentsClient
