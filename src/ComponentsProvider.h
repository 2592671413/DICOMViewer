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
#include <set>



// ----------------------------------------------------------------------------------
// ComponentsProvider
// ----------------------------------------------------------------------------------

/** \brief	Defines the components service provider.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class ComponentsProvider : public Record::Provider< Components >
{

public:
    
    /** \copydoc Record::Provider::Provider
      */
    ComponentsProvider( Record::Server& server )
        : Provider( server )
    {
    }
    
    /** \copydoc Record::Provider::~Provider
      *
      * Releases all components.
      */
    virtual ~ComponentsProvider();


    virtual void releaseAll() override;

    virtual void release( const Component& ) override;

    virtual void takeComponent( Component* ) override;


private:

    /** \brief  Defines a set of components.
      */
    typedef std::set< Component* > ComponentSet;

    /** \brief  Holds the components, which have been supplied by \c putComponent.
      */
    ComponentSet set;

}; // ComponentsProvider
