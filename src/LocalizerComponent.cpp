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

#include "LocalizerComponent.h"
#include "LocalizerController.h"



// ----------------------------------------------------------------------------------
// LocalizerComponent
// ----------------------------------------------------------------------------------

LocalizerComponent::LocalizerComponent( Record::Server& server, ComponentWindowFactory& factory )
    : Component( server, factory, "Localizer" )
{
    LocalizerController* controller = new LocalizerController( *this, server );

    ComponentDockable& dockable = createDockable( controller
        , Qt::TopDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable );
}


LocalizerComponent::~LocalizerComponent()
{
}
