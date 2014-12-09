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

#include "RegistrationComponent.h"
#include "RegistrationController.h"



// ----------------------------------------------------------------------------------
// RegistrationComponent
// ----------------------------------------------------------------------------------

RegistrationComponent::RegistrationComponent( Record::Server& server, ComponentWindowFactory& factory )
    : Component( server, factory, "Registration" )
{
    RegistrationController* controller = new RegistrationController( server, *this );

    ComponentDockable& dockable = createDockable( controller
        , Qt::LeftDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable );
}


RegistrationComponent::~RegistrationComponent()
{
}
