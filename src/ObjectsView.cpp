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

#include "ObjectsView.h"
#include "CarnaContextClient.h"
#include <Carna/Object3DEvent.h>
#include <Carna/Model.h>
#include <QApplication>
#include <algorithm>



// ----------------------------------------------------------------------------------
// ObjectsView
// ----------------------------------------------------------------------------------

ObjectsView::ObjectsView( Record::Server& server, QWidget* parent )
    : ObjectsList( server, parent )
{
    completeList();
}


void ObjectsView::addObject( Carna::Object3D& )
{
}


void ObjectsView::removeObject( Carna::Object3D& )
{
}


void ObjectsView::clear()
{
}


void ObjectsView::processObjectsEvent( Objects3DEvent& ev )
{
    ObjectsList::processObjectsEvent( ev );

    if( ev.mightAffect( Carna::Object3DEvent::existence ) )
    {
        completeList();
    }
}


void ObjectsView::completeList()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    std::vector< Carna::Object3D* > objects;
    fetchObjects( objects );

    Carna::Model& model = CarnaContextClient( server ).model();
    for( unsigned int i = 0; i < model.objectsCount(); ++i )
    {
        Carna::Object3D& object = model.objectByIndex( i );
        if( std::find( objects.begin(), objects.end(), &object ) == objects.end() )
        {
            ObjectsList::addObject( object );
        }
    }

    QApplication::restoreOverrideCursor();
}
