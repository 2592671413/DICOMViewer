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

#include "ToolCalibrator.h"
#include <Carna/base/Transformation.h>
#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>



// ----------------------------------------------------------------------------------
// ToolCalibrator
// ----------------------------------------------------------------------------------

ToolCalibrator::ToolCalibrator( Record::Server& server, CRA::Tool& tool )
    : server( server )
    , tool( tool )
    , laState( new QLabel( "Click 'capture' to perform pivot calibration." ) )
{
    QPushButton* const buCapture = new QPushButton( "Capture" );
    buCapture->setCheckable( true );
    buCapture->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    connect( buCapture, SIGNAL( toggled( bool ) ), this, SLOT( setCapturing( bool ) ) );

    QVBoxLayout* const global = new QVBoxLayout();
    this->setLayout( global );
    global->addWidget( laState );
    global->addWidget( buCapture );
    global->addStretch( 1 );
}


ToolCalibrator::~ToolCalibrator()
{
}


void ToolCalibrator::capture()
{
    const Carna::base::Transformation& bearing = tool.getOrientation();

    if( !tool.isVisible() )
    {
        return;
    }

    const Matrix rotation = bearing.getTransformationMatrix().topLeftCorner( 3, 3 );
    const Vector location = bearing.getTransformationMatrix().topRightCorner( 3, 1 );

    rotations.push_back( rotation );
    locations.push_back( location );

    laState->setText( "Captured: " + QString::number( locations.size() ) );
}


void ToolCalibrator::begin()
{
    shotTimer.reset( new QTimer() );
    shotTimer->setInterval( 50 );

    locations.clear();
    rotations.clear();

    connect( shotTimer.get(), SIGNAL( timeout() ), this, SLOT( capture() ) );
    tool.setOffset( Carna::base::Vector( 0, 0, 0 ) );

    shotTimer->start();
}


void ToolCalibrator::end()
{
    shotTimer->stop();

    if( !locations.empty() || locations.size() != rotations.size() )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        Calibration pivotCalibration( rotations, locations );
        
        pivotCalibration.compute();

        const Vector translation = pivotCalibration.getTranslation();

        tool.setOffset( Carna::base::Vector( translation ) );

        laState->setText( "Offset: " + QString::number( translation.norm() ) + " mm" );

        QApplication::restoreOverrideCursor();
    }
}


void ToolCalibrator::setCapturing( bool on )
{
    if( on )
    {
        begin();
    }
    else
    {
        end();
    }
}
