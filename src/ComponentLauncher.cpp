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

#include "ComponentLauncher.h"
#include "RegistredComponent.h"
#include "ViewWindow.h"
#include <QAction>



// ----------------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------------

static inline QString getIconPath( const std::string& name )
{
    QString result = ":/icons/components/";
    result.append( QString::fromStdString( name ) );
    result.append( ".png" );
    return result;
}



// ----------------------------------------------------------------------------------
// ComponentLauncher
// ----------------------------------------------------------------------------------

ComponentLauncher::ComponentLauncher( const ComponentFactory& factory
                                    , ViewWindow& view_window
                                    , QObject* parent )
    : QObject( parent )
    , action( new QAction( QIcon( getIconPath( factory.getName() ) )
                         , QString::fromStdString( factory.getName() ), this ) )
    , factory( factory )
    , view_window( view_window )
{
    connect( action, SIGNAL( triggered() ), this, SLOT( launch() ) );
}


void ComponentLauncher::launch()
{
    view_window.launch( *this );
}
