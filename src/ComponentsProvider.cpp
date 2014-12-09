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

#include "ComponentsProvider.h"
#include "Component.h"



// ----------------------------------------------------------------------------------
// ComponentsProvider
// ----------------------------------------------------------------------------------

ComponentsProvider::~ComponentsProvider()
{
    releaseAll();
}


void ComponentsProvider::releaseAll()
{
    for( ComponentSet::iterator it = set.begin(); it != set.end(); ++it )
    {
        delete *it;
    }
    set.clear();
}


void ComponentsProvider::release( const Component& component )
{
    ComponentSet::iterator it = set.find( const_cast< Component* >( &component ) );
    if( it != set.end() )
    {
        delete &component;
        set.erase( it );
    }
}


void ComponentsProvider::takeComponent( Component* component )
{
    set.insert( component );
}