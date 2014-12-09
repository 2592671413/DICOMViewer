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

#include "NormalizedEdgeResponse.h"
#include <vector>
#include <algorithm>



// ----------------------------------------------------------------------------------
// NormalizedEdgeResponse :: RadialSampler :: reallyLess
// ----------------------------------------------------------------------------------

bool NormalizedEdgeResponse::RadialSampler::reallyLess::operator()( const double x, const double y ) const
{
    return !Carna::base::Math::isEqual( x, y ) && x < y;
}



// ----------------------------------------------------------------------------------
// NormalizedEdgeResponse :: RadialSampler
// ----------------------------------------------------------------------------------

NormalizedEdgeResponse::RadialSampler::RadialSampler
    ( const NormalizedEdgeResponse& context
    , const Carna::base::Vector& position
    , const Carna::base::Vector& radialVector
    , double minimumContrast )

    : context( context )
    , position( position )
    , radialDirection( radialVector.normalized() )
    , maximumResponse(
        [&]()->double
        {
            double maximumResponse = minimumContrast;
            for( auto radius_it = context.radiuses.begin(); radius_it != context.radiuses.end(); ++radius_it )
            {
                const double probed_r = *radius_it;
                const double response = -context.differential.partialDerivativeAt( position + probed_r * radialDirection, radialDirection );
                maximumResponse = response > maximumResponse ? response : maximumResponse;
            }
            return maximumResponse;
        }
        () )
{
}


double NormalizedEdgeResponse::RadialSampler::sample( double radius ) const
{
    const auto sample_itr = samples.find( radius );
    if( sample_itr != samples.end() )
    {
        return sample_itr->second;
    }

    const double sample = context.differential.partialDerivativeAt
        ( position + radius * radialDirection
        , radialDirection );

    samples[ radius ] = sample;
    return sample;
}



// ----------------------------------------------------------------------------------
// NormalizedEdgeResponse
// ----------------------------------------------------------------------------------

NormalizedEdgeResponse::NormalizedEdgeResponse( Differential::Sampler* sampler )
    : differential( sampler )
    , currentRadiusSampleDistance( 0 )
{
}


void NormalizedEdgeResponse::setSampler( Differential::Sampler* sampler )
{
    differential.setSampler( sampler );
}


void NormalizedEdgeResponse::setRadiuses( double minRadius, double maxRadius, unsigned int radiusSamples )
{
    CARNA_ASSERT( radiusSamples >= 2 );
    CARNA_ASSERT( maxRadius > minRadius );

    radiuses.clear();

    currentRadiusSampleDistance = ( maxRadius - minRadius ) / ( radiusSamples - 1 );
    for( double radius = minRadius; radius < maxRadius || Carna::base::Math::isEqual( radius, maxRadius ); radius += currentRadiusSampleDistance )
    {
        radiuses.insert( radius );
    }
}


void NormalizedEdgeResponse::setScales( double minScale, double maxScale, unsigned int scaleSamples )
{
    differential.setScales( minScale, maxScale, scaleSamples );
}


const MultiscaleDifferential::Scales& NormalizedEdgeResponse::scales() const
{
    return differential.scales;
}


double NormalizedEdgeResponse::radiusSampleDistance() const
{
    return currentRadiusSampleDistance;
}


double NormalizedEdgeResponse::compute( const RadialSampler& sampler, double r ) const
{
    CARNA_ASSERT( &sampler.context == this );

    const Carna::base::Vector& p0 = sampler.position;
    const Carna::base::Vector& u  = sampler.radialDirection;

 // compute response at p0

    const double response_at_p0 = -sampler.sample( r );
    if( response_at_p0 <= 0 )
    {
        return 0;
    }

 // compute maximum rising edge response up to current radius

    double max_rising_boundary_up_to_r = 0;
    const double dr = radiusSampleDistance();
    for( double smaller_r = r - dr; smaller_r >= 0; smaller_r -= dr )
    {
        const double response = sampler.sample( smaller_r );
        max_rising_boundary_up_to_r = std::max( max_rising_boundary_up_to_r, response );
    }

 // compute dividend

    const double dividend = std::max( 0., response_at_p0 - max_rising_boundary_up_to_r );

 // finish

    return dividend / sampler.maximumResponse;
}


void NormalizedEdgeResponse::setMinimumHUV( int huv )
{
    differential.setMinimumHUV( huv );
}


void NormalizedEdgeResponse::setMaximumHUV( int huv )
{
    differential.setMaximumHUV( huv );
}


int NormalizedEdgeResponse::minimumHUV() const
{
    return differential.minimumHUV();
}


int NormalizedEdgeResponse::maximumHUV() const
{
    return differential.maximumHUV();
}


void NormalizedEdgeResponse::setGamma( double gamma )
{
    differential.setGamma( gamma );
}


double NormalizedEdgeResponse::gamma() const
{
    return differential.gamma();
}
