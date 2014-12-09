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

#include "IntegerFormatChooser.h"
#include <QComboBox>
#include <QVBoxLayout>



// ----------------------------------------------------------------------------------
// IntegerFormatChooser
// ----------------------------------------------------------------------------------

IntegerFormatChooser::IntegerFormatChooser( QWidget* parent )
    : QWidget( parent )
    , format( native16bit )
{
    this->setLayout( new QVBoxLayout() );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );

    QComboBox* const cbFormat = new QComboBox();
    cbFormat->addItem( "16bit system native byte order" );
    cbFormat->addItem( "32bit system native byte order" );
    cbFormat->setCurrentIndex( 0 );
    this->layout()->addWidget( cbFormat );
    connect( cbFormat, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setCurrentFormat( int ) ) );
}


void IntegerFormatChooser::setCurrentFormat( int index )
{
    format = static_cast< IntegerFormat >( index );
}
