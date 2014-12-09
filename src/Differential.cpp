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

#include "Differential.h"
#include <Carna/common.h>



// ----------------------------------------------------------------------------------
// pow3
// ----------------------------------------------------------------------------------

template< typename T >
inline T pow3( const T x )
{
    return x * x * x;
}



// ----------------------------------------------------------------------------------
// Differential
// ----------------------------------------------------------------------------------

Differential::Differential( Differential::Sampler* sampler )
    : scale( 1. )
    , currentGamma( 1. )
    , sampler( sampler )
    , minHUV( -1024 )
    , maxHUV(  3071 )
{
    updateGaussianDerivative( 5 );
}


void Differential::setGamma( double gamma )
{
    this->currentGamma = gamma;
}


double Differential::gamma() const
{
    return currentGamma;
}


void Differential::setMinimumHUV( int huv )
{
    this->minHUV = huv;
}


void Differential::setMaximumHUV( int huv )
{
    this->maxHUV = huv;
}


int Differential::minimumHUV() const
{
    return minHUV;
}


int Differential::maximumHUV() const
{
    return maxHUV;
}


void Differential::setScale( double scale )
{
    CARNA_ASSERT( scale > 0 );

    this->scale = scale;
    updateGaussianDerivative( gaussianDerivativeSamples.size() );
}


void Differential::updateGaussianDerivative( unsigned int samples )
{
    const static double PI = std::acos( -1. );

 // define mirrored derivative of Gaussian kernel

    const double scale_sq = Carna::Tools::sq( scale );
    const double normalization = 1. / std::sqrt( 2 * PI * scale_sq );
    const auto mirroredGaussianDerivativeAt = [&]( double x )->double
    {
        return normalization * std::exp( -Carna::Tools::sq( -x ) / ( 2 * scale_sq ) ) * x / scale_sq;
    };

 // define samples

    gaussianDerivativePositions.clear();
    gaussianDerivativeSamples.clear();

    gaussianDerivativePositions.push_back( -scale * 4 );
    gaussianDerivativeSamples.push_back( 0 );

    gaussianDerivativePositions.push_back( -scale );
    gaussianDerivativeSamples.push_back( mirroredGaussianDerivativeAt( -scale ) );

    gaussianDerivativePositions.push_back( 0 );
    gaussianDerivativeSamples.push_back( 0 );

    gaussianDerivativePositions.push_back( +scale );
    gaussianDerivativeSamples.push_back( mirroredGaussianDerivativeAt( +scale ) );

    gaussianDerivativePositions.push_back( +scale * 4 );
    gaussianDerivativeSamples.push_back( 0 );
}


double Differential::partialDerivativeAt( const Carna::Tools::Vector& p0, const Carna::Tools::Vector& _direction ) const
{
    CARNA_ASSERT( sampler.get() != nullptr );

    const Carna::Tools::Vector direction = _direction.normalized();
    const unsigned int samples = gaussianDerivativeSamples.size();

 // compute convolution in p0

    double sum = 0;
    double f0 = 0;
    double g0 = 0;
    double t0 = gaussianDerivativePositions[ 0 ];

    for( unsigned int i = 1; i < samples; ++i )
    {
        const double g1 = gaussianDerivativeSamples[ i ];
        const double t1 = gaussianDerivativePositions[ i ];
        const double dt = t1 - t0;

        const Carna::Tools::Vector p = p0 + t1 * direction;
        const double sampled_value = Carna::Tools::isEqual( g1, 0. ) ? 0 : sampler->valueAt( p );
        const double clamped_value = Carna::Tools::clamp< double >( sampled_value, minHUV, maxHUV );

        const double f1 = clamped_value * g1;
        const double trapeze = ( f0 + f1 ) * dt / 2;
        sum += trapeze;

        f0 = f1;
        g0 = g1;
        t0 = t1;
    }

 // finish

    return sum * std::pow( scale, gamma() );
}


void Differential::setSampler( Differential::Sampler* sampler )
{
    this->sampler.reset( sampler );
}



// ----------------------------------------------------------------------------------
// Differential :: Sampler
// ----------------------------------------------------------------------------------

Differential::Sampler::~Sampler()
{
}
