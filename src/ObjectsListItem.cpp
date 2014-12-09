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

/** \file   ObjectsList.h
  * \brief  Defines ObjectsList.
  */

#include "ObjectsListItem.h"
#include "CarnaContextClient.h"
#include <Carna/Object3D.h>
#include <Carna/Object3DEvent.h>
#include <Carna/Model.h>



// ----------------------------------------------------------------------------------
// ObjectsListItem
// ----------------------------------------------------------------------------------

ObjectsListItem::ObjectsListItem( Record::Server& server, Carna::Object3D& object )
    : object( object )
{
    setText( QString::fromStdString( object.name() ) );
    setIcon( object.icon() );

    connect( &object, SIGNAL( renamed( const QString& ) ), this, SLOT( setText( const QString& ) ) );

    using namespace Carna;
    connect( &( CarnaContextClient( server ).model() ), SIGNAL( objectsChanged( Object3DEvent& ) ), this, SLOT( processObjectsEvent( Object3DEvent& ) ) );
}


void ObjectsListItem::setText( const QString& text )
{
    QListWidgetItem::setText( text );
}


void ObjectsListItem::processObjectsEvent( Carna::Object3DEvent& ev )
{
    if( ev.mightAffect( Carna::Object3DEvent::shape ) )
    {
        setIcon( object.icon() );
    }
}
