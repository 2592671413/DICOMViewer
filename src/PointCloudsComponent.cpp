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

#include "PointCloudsComponent.h"
#include "PointCloudsController.h"



// ----------------------------------------------------------------------------------
// PointClouds
// ----------------------------------------------------------------------------------

PointCloudsComponent::PointCloudsComponent( Record::Server& server, ComponentWindowFactory& factory )
: Component( server, factory, "Point Clouds" )
{
    PointCloudsController* controller = new PointCloudsController( server, *this );

    ComponentDockable& dockable = createDockable( controller
        , Qt::LeftDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable );
}


PointCloudsComponent::~PointCloudsComponent()
{
}
