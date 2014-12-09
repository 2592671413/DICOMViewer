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

#include "WindowingController.h"
#include <Carna/base/model/Scene.h>
#include <QSlider>
#include <QLabel>
#include <QFormLayout>
#include <QDockWidget>
#include <QTimer>



// ----------------------------------------------------------------------------------
// WindowingController
// ----------------------------------------------------------------------------------

WindowingController::WindowingController( Record::Server& server )
    : QWidget()
    , carna( server )
    , slWindowLevel( new QSlider( Qt::Horizontal, this ) )
    , slWindowWidth( new QSlider( Qt::Horizontal, this ) )
    , laWindowLevel( new QLabel( this ) )
    , laWindowWidth( new QLabel( this ) )
{
    QWidget* const levelParent = new QWidget();
    QWidget* const widthParent = new QWidget();

    levelParent->setLayout( new QHBoxLayout() );
    widthParent->setLayout( new QHBoxLayout() );

    levelParent->layout()->addWidget( slWindowLevel );
    levelParent->layout()->addWidget( laWindowLevel );

    widthParent->layout()->addWidget( slWindowWidth );
    widthParent->layout()->addWidget( laWindowWidth );

    QFormLayout* const layout = new QFormLayout();
    layout->addRow( tr( "&Window Level:" ), levelParent );
    layout->addRow( tr( "&Window Width:" ), widthParent );
    this->setLayout( layout );

    slWindowLevel->setMinimum( -1024 );
    slWindowLevel->setMaximum(  3071 );
    slWindowLevel->setValue( carna.model().recommendedWindowingLevel() );

    slWindowWidth->setMinimum(    1 );
    slWindowWidth->setMaximum( 3071 );
    slWindowWidth->setValue( carna.model().recommendedWindowingWidth() );

    laWindowLevel->setMinimumWidth( 30 );
    laWindowWidth->setMinimumWidth( 30 );

    laWindowLevel->setText( QString().setNum( slWindowLevel->value() ) );
    laWindowWidth->setText( QString().setNum( slWindowWidth->value() ) );

    laWindowLevel->setAlignment( Qt::AlignRight );
    laWindowWidth->setAlignment( Qt::AlignRight );

    connect( slWindowLevel, SIGNAL( valueChanged( int ) ), this, SLOT( setWindowingLevel( int ) ) );
    connect( slWindowWidth, SIGNAL( valueChanged( int ) ), this, SLOT( setWindowingWidth( int ) ) );
}


WindowingController::~WindowingController()
{
}


void WindowingController::setWindowingLevel( int val )
{
    laWindowLevel->setText( QString().setNum( val ) );

    carna.model().setRecommendedWindowingLevel( val );
}


void WindowingController::setWindowingWidth( int val )
{
    laWindowWidth->setText( QString().setNum( val ) );

    carna.model().setRecommendedWindowingWidth( val );
}
