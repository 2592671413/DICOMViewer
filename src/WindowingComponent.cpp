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

#include "WindowingComponent.h"
#include "WindowingController.h"



// ----------------------------------------------------------------------------------
// WindowingComponent
// ----------------------------------------------------------------------------------

WindowingComponent::WindowingComponent( Record::Server& server, ComponentWindowFactory& factory )
    : Component( server, factory, "Global Windowing" )
{
    WindowingController* controller = new WindowingController( server );

    createDockable( controller
        , Qt::BottomDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable );
}
