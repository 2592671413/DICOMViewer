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

#include "glew.h"
#include "VolumeViewCameraController.h"
#include <Carna/VolumeVisualization.h>
#include <Carna/Renderer.h>
#include <Carna/DefaultCamera.h>
#include <QEvent>



// ----------------------------------------------------------------------------------
// VolumeViewCameraController
// ----------------------------------------------------------------------------------

VolumeViewCameraController::VolumeViewCameraController( Carna::VolumeVisualization& view, QObject* parent )
    : VolumeVisualizationCameraController( view, parent )
    , autoRotate( false )
    , secondsPerRotation( 10. )
{
    setStepsPerSecond( 15 );

    QObject::connect( &autoRotationTimer, SIGNAL( timeout() ), this, SLOT( doRotationStep() ) );
}


void VolumeViewCameraController::event( Carna::Visualization& sourceModule, QEvent* event )
{
    if( autoRotate )
    {
        DefaultCameraController::event( sourceModule, event );

        if( event->type() == QEvent::MouseButtonPress )
        {
            stopAutoRotate();
        }
        else
        if( event->type() == QEvent::MouseButtonRelease )
        {
            startAutoRotate();
        }
    }
    else
    {
        VolumeVisualizationCameraController::event( sourceModule, event );
    }
}


void VolumeViewCameraController::setStepsPerSecond( int stepsPerSecond )
{
    this->stepsPerSecond = stepsPerSecond;

    autoRotationTimer.setInterval( static_cast< int >( 1000. / stepsPerSecond ) );
}


void VolumeViewCameraController::setAutoRotate( bool autoRotate )
{
    if( this->autoRotate != autoRotate )
    {
        if( autoRotate )
        {
            startAutoRotate();
        }
        else
        {
            stopAutoRotate();
        }

        this->autoRotate = autoRotate;
    }
}


void VolumeViewCameraController::setSecondsPerRotation( double secondsPerRotation )
{
    this->secondsPerRotation = secondsPerRotation;
}


int VolumeViewCameraController::getStepsPerSecond() const
{
    return stepsPerSecond;
}


bool VolumeViewCameraController::hasAutoRotate() const
{
    return autoRotate;
}


double VolumeViewCameraController::getSecondsPerRotation() const
{
    return secondsPerRotation;
}


void VolumeViewCameraController::startAutoRotate()
{
    frameTimer.start();
    autoRotationTimer.start();
}


void VolumeViewCameraController::stopAutoRotate()
{
    autoRotationTimer.stop();
}


void VolumeViewCameraController::doRotationStep()
{
    const double secondsPerStep = frameTimer.restart() / 1000.;
    const double degreesPerSecond = 360. / secondsPerRotation;
    const double degreesPerStep = degreesPerSecond * secondsPerStep;

    Carna::DefaultCamera* const camera = dynamic_cast< Carna::DefaultCamera* >( &module.renderer().camera() );
    if( camera != nullptr )
    {
        Carna::Tools::Transformation rotation;
        rotation.rotateY( degreesPerStep, Carna::Tools::Transformation::DEGREES );
        camera->rotate( rotation );
    }
}
