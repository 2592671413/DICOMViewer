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

#include "MultiscaleDifferential.h"



// ----------------------------------------------------------------------------------
// MultiscaleDifferential
// ----------------------------------------------------------------------------------

MultiscaleDifferential::MultiscaleDifferential( Differential::Sampler* sampler )
    : differential( sampler )
{
}


void MultiscaleDifferential::partialDerivativeAt
    ( const Carna::Tools::Vector& position
    , const Carna::Tools::Vector& direction
    , double& partialDerivative
    , double& scale ) const
{
    CARNA_ASSERT( !scales.empty() );

    partialDerivative = 0;
    for( auto scale_it = scales.begin(); scale_it != scales.end(); ++scale_it )
    {
        const double current_scale = *scale_it;
        differential.setScale( current_scale );
        const double current_partial_derivative = differential.partialDerivativeAt( position, direction );

        if( std::abs( current_partial_derivative ) >= std::abs( partialDerivative ) )
        {
            partialDerivative = current_partial_derivative;
            scale = current_scale;
        }
    }
}


double MultiscaleDifferential::partialDerivativeAt
    ( const Carna::Tools::Vector& position
    , const Carna::Tools::Vector& direction ) const
{
    double partialDerivative;
    double scale;

    partialDerivativeAt( position, direction, partialDerivative, scale );
    return partialDerivative;
}


void MultiscaleDifferential::setSampler( Differential::Sampler* sampler )
{
    differential.setSampler( sampler );
}


void MultiscaleDifferential::setScales( double minScale, double maxScale, unsigned int scaleSteps )
{
    CARNA_ASSERT( scaleSteps >= 2 );
    CARNA_ASSERT( maxScale > minScale );

    scales.clear();

    const double scaleStep = ( maxScale - minScale ) / ( scaleSteps - 1 );
    for( double scale = minScale; scale < maxScale || Carna::Tools::isEqual( scale, maxScale ); scale += scaleStep )
    {
        scales.insert( scale );
    }
}


void MultiscaleDifferential::setMinimumHUV( int huv )
{
    differential.setMinimumHUV( huv );
}


void MultiscaleDifferential::setMaximumHUV( int huv )
{
    differential.setMaximumHUV( huv );
}


int MultiscaleDifferential::minimumHUV() const
{
    return differential.minimumHUV();
}


int MultiscaleDifferential::maximumHUV() const
{
    return differential.maximumHUV();
}


void MultiscaleDifferential::setGamma( double gamma )
{
    differential.setGamma( gamma );
}


double MultiscaleDifferential::gamma() const
{
    return differential.gamma();
}
