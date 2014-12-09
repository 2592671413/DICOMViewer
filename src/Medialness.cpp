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

#include "Medialness.h"



// ----------------------------------------------------------------------------------
// Medialness
// ----------------------------------------------------------------------------------

Medialness::Medialness( Differential::Sampler* sampler )
    : edgeResponse( sampler )
    , currentMinimumContrast( 1. )
{
}


Medialness::~Medialness()
{
    deleteDirectionData();
}


void Medialness::setSampler( Differential::Sampler* sampler )
{
    edgeResponse.setSampler( sampler );
}


void Medialness::setRadiuses( double minRadius, double maxRadius, unsigned int radiusSamples )
{
    edgeResponse.setRadiuses( minRadius, maxRadius, radiusSamples );
}


void Medialness::setScales( double minScale, double maxScale, unsigned int scaleSamples )
{
    edgeResponse.setScales( minScale, maxScale, scaleSamples );
}


void Medialness::compute
    ( const Carna::base::Vector& position
    , const Carna::base::Vector& vesselDirection
    , double& medialness
    , double& radius
    , double minimumMedialness ) const
{
    updateDirectionData( position, vesselDirection );

    medialness = -std::numeric_limits< double >::infinity();
    const double sampleWeight = 1. / directionData.size();
    for( auto radius_it = edgeResponse.radiuses.begin(); radius_it != edgeResponse.radiuses.end(); ++radius_it )
    {
        const double r = *radius_it;

        double response = 0;
        for( unsigned int directionIndex = 0; directionIndex < directionData.size(); ++directionIndex )
        {
            const unsigned int samplesLeft = directionData.size() - directionIndex;
            if( response + samplesLeft * sampleWeight < minimumMedialness )
            {
                response = -std::numeric_limits< double >::infinity();
                break;
            }

            const NormalizedEdgeResponse::RadialSampler& normalization = *directionData[ directionIndex ];
            response += sampleWeight * edgeResponse.compute( normalization, r );
        }

        if( response > medialness )
        {
            medialness = response;
            radius = r;
        }
        if( Carna::base::Math::isEqual( medialness, 1. ) )
        {
            break;
        }
    }
}


void Medialness::deleteDirectionData() const
{
    std::for_each
        ( directionData.begin()
        , directionData.end()
        , std::default_delete< NormalizedEdgeResponse::RadialSampler >() );

    directionData.clear();
}


void Medialness::updateDirectionData
    ( const Carna::base::Vector& position
    , const Carna::base::Vector& vesselDirection ) const
{
    deleteDirectionData();

    const Carna::base::Vector u1 = vesselDirection.orthonormal();
    const Carna::base::Vector u2 = u1.cross( vesselDirection ).normalized();

    CARNA_ASSERT(
        Carna::base::Math::isEqual( u1.cross( u2 ).normalized(), vesselDirection.normalized() ) ||
        Carna::base::Math::isEqual( u1.cross( u2 ).normalized(), vesselDirection.normalized() * -1 ) );

    const unsigned int n = 8;
    const static double PI = std::acos( -1. );
    const double radians_step = 2 * PI / n;
    for( int i = 0; i < n; ++i )
    {
        const double radians = radians_step * i;
        const Carna::base::Vector radialDirection = u1 * std::sin( radians ) + u2 * std::cos( radians );

        directionData.push_back
            ( new NormalizedEdgeResponse::RadialSampler
                ( edgeResponse
                , position
                , radialDirection
                , minimumContrast() ) );
    }
}


double Medialness::minimumContrast() const
{
    return currentMinimumContrast;
}


void Medialness::setMinimumContrast( double newMinimumContrast )
{
    this->currentMinimumContrast = newMinimumContrast;
}


void Medialness::setMinimumHUV( int huv )
{
    edgeResponse.setMinimumHUV( huv );
}


void Medialness::setMaximumHUV( int huv )
{
    edgeResponse.setMaximumHUV( huv );
}


int Medialness::minimumHUV() const
{
    return edgeResponse.minimumHUV();
}


int Medialness::maximumHUV() const
{
    return edgeResponse.maximumHUV();
}


void Medialness::setGamma( double gamma )
{
    edgeResponse.setGamma( gamma );
}


double Medialness::gamma() const
{
    return edgeResponse.gamma();
}
