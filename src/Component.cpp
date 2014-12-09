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

#include "Component.h"
#include "ComponentsClient.h"
#include <QTimer>
#include <QDockWidget>
#include <QApplication>
#include <QCloseEvent>



// ----------------------------------------------------------------------------------
// ComponentWindowFactory
// ----------------------------------------------------------------------------------

const QDockWidget::DockWidgetFeatures ComponentWindowFactory::defaultDockableFeatures
    = QDockWidget::DockWidgetMovable;



// ----------------------------------------------------------------------------------
// ComponentDockable
// ----------------------------------------------------------------------------------

void ComponentDockable::closeEvent( QCloseEvent* ev )
{
    this->hide();
    this->deleteLater();
}



// ----------------------------------------------------------------------------------
// Component
// ----------------------------------------------------------------------------------

Component::Component( Record::Server& server, ComponentWindowFactory& factory, const QString& name )
    : windowFactory( factory )
    , server( server )
    , name( name )
{
}


Component::~Component()
{
    for( auto it = embeddables.begin(); it != embeddables.end(); ++it )
    {
        ( **it ).close();
    }

    for( auto it = dockables.begin(); it != dockables.end(); ++it )
    {
        ( **it ).close();
    }
}


void Component::setName( const QString& name )
{
    this->name = name;

    for( EmbedableSet::iterator it = embeddables.begin(); it != embeddables.end(); ++it )
    {
        ComponentEmbeddable* p = *it;
        p->setBaseTitle( name );
    }
    for( DockableSet::iterator it = dockables.begin(); it != dockables.end(); ++it )
    {
        QDockWidget* p = *it;
        p->setWindowTitle( name );
    }

    nameChangeEvent( name );
    emit renamed( name );
}


void Component::nameChangeEvent( const QString& )
{
}


void Component::close()
{
    releaseMyself();
}


void Component::removeDestroyedEmbeddable( QObject* object )
{
    ComponentEmbeddable* const embeddable = static_cast< ComponentEmbeddable* >( object );
    embeddables.erase( embeddable );
}


void Component::removeDestroyedDockable( QObject* object )
{
    ComponentDockable* const dockable = static_cast< ComponentDockable* >( object );
    dockables.erase( dockable );
}


ComponentEmbeddable& Component::createEmbeddable( QWidget* child, EmbeddablePlacer& placer, const QString& suffix, const QString& suffixConnector, MenuFactory& menuFactory )
{
    ComponentEmbeddable* p = windowFactory.createEmbedable( child, placer, suffix, suffixConnector, menuFactory );

    connect( p, SIGNAL( destroyed( QObject* ) ), this, SLOT( removeDestroyedEmbeddable( QObject* ) ) );
    connect( p, SIGNAL( renamingRequested( const QString& ) ), this, SLOT( setName( const QString& ) ) );

    embeddables.insert( p );
    p->setBaseTitle( this->name );
    p->show();
    QApplication::processEvents();
    return *p;
}


ComponentEmbeddable& Component::createVitalEmbeddable( QWidget* child, EmbeddablePlacer& placer, const QString& suffix, const QString& suffixConnector, MenuFactory& menuFactory )
{
    ComponentEmbeddable& em = createEmbeddable( child, placer, suffix, suffixConnector, menuFactory );
    QObject::connect( &em, SIGNAL( destroyed() ), this, SLOT( releaseMyself() ) );
    return em;
}


ComponentDockable& Component::createDockable( QWidget* child, Qt::DockWidgetArea area, QDockWidget::DockWidgetFeatures features )
{
    ComponentDockable* p = windowFactory.createDockable( child, area, features );

    connect( p, SIGNAL( destroyed( QObject* ) ), this, SLOT( removeDestroyedDockable( QObject* ) ) );
    QObject::connect( p, SIGNAL( destroyed() ), this, SLOT( releaseMyself() ) );

    dockables.insert( p );
    p->setWindowTitle( this->name );
    return *p;
}


void Component::releaseMyself()
{
    ComponentsClient( server ).release( *this );
}
