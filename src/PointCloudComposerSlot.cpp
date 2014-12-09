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

#include "PointCloudComposerSlot.h"
#include "CarnaContextClient.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>



// ----------------------------------------------------------------------------------
// PointCloudComposerSlot
// ----------------------------------------------------------------------------------

PointCloudComposerSlot::PointCloudComposerSlot( Record::Server& server, int number )
    : objectChooser( new Carna::Object3DChooser( CarnaContextClient( server ).model() ) )
    , laNumber( new QLabel( QString::number( number ) + "." ) )
{
    this->setLayout( new QHBoxLayout() );

    QPushButton* const buRelase = new QPushButton( "Remove" );
    connect( buRelase, SIGNAL( clicked() ), this, SLOT( remove() ) );
    buRelase->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    laNumber->setFixedWidth( 30 );
    laNumber->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    this->layout()->addWidget( laNumber );
    this->layout()->addWidget( objectChooser );
    this->layout()->addWidget( buRelase );

    this->layout()->setContentsMargins( 0, 0, 0, 0 );

    this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    this->setFixedHeight( buRelase->sizeHint().height() );
    this->setMinimumHeight( this->sizeHint().height() );
}


QSize PointCloudComposerSlot::sizeHint() const
{
    return QSize( 0, objectChooser->sizeHint().height() );
}


void PointCloudComposerSlot::remove()
{
    emit releaseRequested( this );
}


void PointCloudComposerSlot::setNumber( int number )
{
    laNumber->setText( QString::number( number ) + "." );
}
