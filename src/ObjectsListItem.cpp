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
#include <Carna/base/model/Object3D.h>
#include <Carna/base/model/Object3DEvent.h>
#include <Carna/base/model/Scene.h>



// ----------------------------------------------------------------------------------
// ObjectsListItem
// ----------------------------------------------------------------------------------

ObjectsListItem::ObjectsListItem( Record::Server& server, Carna::base::model::Object3D& object )
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


void ObjectsListItem::processObjectsEvent( Carna::base::model::Object3DEvent& ev )
{
    if( ev.mightAffect( Carna::base::model::Object3DEvent::shape ) )
    {
        setIcon( object.icon() );
    }
}
