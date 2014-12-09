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

#include "Pointer3DCalibration.h"
#include "RegistrationClient.h"
#include <Carna/RotatableObject3D.h>
#include <TRTK/FitLine3D.hpp>



// ----------------------------------------------------------------------------------
// Pointer3DCalibration
// ----------------------------------------------------------------------------------

Pointer3DCalibration::Pointer3DCalibration( Carna::RotatableObject3D& pointer )
    : pointer( pointer )
{
}


Pointer3DCalibration::~Pointer3DCalibration()
{
}


const Carna::Tools::Vector& Pointer3DCalibration::getShaftDirection()
{
    if( !shaftDirection.get() )
    {
        compute();
    }
    return *shaftDirection;
}


const Carna::Tools::Vector& Pointer3DCalibration::getShaftDirection() const
{
    if( !shaftDirection.get() )
    {
        throw std::logic_error( "Result has not been computed yet." );
    }
    return *shaftDirection;
}


void Pointer3DCalibration::capture()
{
    const Carna::Tools::Vector& p_captured = pointer.position().toMillimeters();

    capturedPoints.push_back( p_captured );
}


void Pointer3DCalibration::reset()
{
    capturedPoints.clear();
}


void Pointer3DCalibration::compute()
{
    if( capturedPoints.size() < 2 )
    {
        throw std::logic_error( "Too few points captured." );
    }

    const Carna::Tools::Vector& origin = capturedPoints[ 0 ];

    std::vector< Carna::Tools::Vector > object_sapce_points( capturedPoints.size() );
    for( unsigned int i = 0; i < capturedPoints.size(); ++i )
    {
        object_sapce_points[ i ] = capturedPoints[ i ] - origin;
    }

    TRTK::FitLine3D< double > fitLine3D( object_sapce_points );
    fitLine3D.compute();

    Carna::Tools::Vector result = fitLine3D.getDirectionVector().normalized();
    result = pointer.rotation().inverse() * result;

    this->shaftDirection.reset( new Carna::Tools::Vector( result ) );
    emit computed( *shaftDirection );
}
