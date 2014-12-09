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

#include "ObjectsComponent.h"
#include "ObjectsController.h"



// ----------------------------------------------------------------------------------
// ObjectsComponent
// ----------------------------------------------------------------------------------

ObjectsComponent::ObjectsComponent( Record::Server& server, ComponentWindowFactory& factory )
    : Component( server, factory, "Objects" )
{
    ObjectsController* controller = new ObjectsController( server, *this );

    ComponentDockable& dockable = createDockable( controller
        , Qt::LeftDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable );
}


ObjectsComponent::~ObjectsComponent()
{
}
