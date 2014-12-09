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
#include <Carna/base/model/Object3DEvent.h>
#include <Carna/base/model/Scene.h>
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


void ObjectsView::addObject( Carna::base::model::Object3D& )
{
}


void ObjectsView::removeObject( Carna::base::model::Object3D& )
{
}


void ObjectsView::clear()
{
}


void ObjectsView::processObjectsEvent( Objects3DEvent& ev )
{
    ObjectsList::processObjectsEvent( ev );

    if( ev.mightAffect( Carna::base::model::Object3DEvent::existence ) )
    {
        completeList();
    }
}


void ObjectsView::completeList()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    std::vector< Carna::base::model::Object3D* > objects;
    fetchObjects( objects );

    Carna::base::model::Scene& model = CarnaContextClient( server ).model();
    for( unsigned int i = 0; i < model.objectsCount(); ++i )
    {
        Carna::base::model::Object3D& object = model.objectByIndex( i );
        if( std::find( objects.begin(), objects.end(), &object ) == objects.end() )
        {
            ObjectsList::addObject( object );
        }
    }

    QApplication::restoreOverrideCursor();
}
