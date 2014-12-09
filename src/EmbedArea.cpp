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

#include "EmbedArea.h"
#include "Embeddable.h"
#include "EmbedManager.h"
#include <exception>
#include <QPainter>
#include <QLayout>
#include <stdexcept>



// ----------------------------------------------------------------------------------
// EmbedArea
// ----------------------------------------------------------------------------------

EmbedArea::EmbedArea( QWidget* parent )
    : QFrame( parent )
    , pClient( NULL )
    , bHovering( false )
{
    this->setLayout( new QVBoxLayout() );
    
    this->setContentsMargins( 0, 0, 0, 0 );

    this->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    this->setLineWidth( 1 );
}


EmbedArea::~EmbedArea()
{
    if( hasClient() )
    {
        getClient().close();
    }
    EmbedManager::instance().unregisterEmbedArea( *this );
}


void EmbedArea::setClient( Embeddable& client )
{
    releaseClient();
    pClient = &client;
    pClient->attachedEvent();

    this->layout()->addWidget( &client );
    this->setFrameStyle( QFrame::Box | QFrame::Sunken );

    emit clientEmbedded( client );
}


void EmbedArea::releaseClient()
{
    if( hasClient() )
    {
        const QPoint pos = pClient->mapToGlobal( QPoint( 0, 0 ) );
        this->layout()->removeWidget( pClient );
        pClient->setParent( nullptr );
        pClient->move( pos );
        pClient->show();

        Embeddable& client = *pClient;
        client.detachedEvent();
        pClient = nullptr;
        
        this->setFrameStyle( QFrame::Panel | QFrame::Sunken );

        emit clientReleased( client );
    }
}


Embeddable& EmbedArea::getClient() const
{
    if( !hasClient() )
    {
        throw std::logic_error( "EmbedArea::getClient invoked but no client embedded" );
    }
    else
    {
        return *pClient;
    }
}


bool EmbedArea::hasClient() const
{
    return pClient != NULL;
}


void EmbedArea::showEvent( QShowEvent* )
{
    EmbedManager::instance().registerEmbedArea( *this );
}


void EmbedArea::hideEvent( QShowEvent* )
{
    EmbedManager::instance().unregisterEmbedArea( *this );
}


void EmbedArea::paintEvent( QPaintEvent* ev )
{
    QFrame::paintEvent( ev );

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.fillRect( this->rect().adjusted( 2, 2, -2, -2 ), bHovering
        ? palette().brush( QPalette::Normal, QPalette::Highlight ).color()
        : palette().window().color() );
}


void EmbedArea::embedableEntered( Embeddable& )
{
    bHovering = true;
    this->update();
}


void EmbedArea::embedableLeft( Embeddable& )
{
    bHovering = false;
    this->update();
}
