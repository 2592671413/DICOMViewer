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

#pragma once

#include "MultiscaleDifferential.h"



// ----------------------------------------------------------------------------------
// NormalizedEdgeResponse
// ----------------------------------------------------------------------------------

class NormalizedEdgeResponse
{

public:

    NormalizedEdgeResponse( Differential::Sampler* = nullptr );

    void setSampler( Differential::Sampler* );

    void setMinimumHUV( int );

    void setMaximumHUV( int );

    int minimumHUV() const;

    int maximumHUV() const;

    void setGamma( double );

    double gamma() const;

 // ----------------------------------------------------------------------------------

    typedef std::set< double > Radiuses;

    Radiuses radiuses;

    void setRadiuses( double minRadius, double maxRadius, unsigned int radiusSamples );

    void setScales( double minScale, double maxScale, unsigned int scaleSamples );

    const MultiscaleDifferential::Scales& scales() const;

    double radiusSampleDistance() const;

 // ----------------------------------------------------------------------------------

    struct RadialSampler
    {
        
        const NormalizedEdgeResponse& context;
        const Carna::Tools::Vector position;
        const Carna::Tools::Vector radialDirection;
        const double maximumResponse;

        RadialSampler
            ( const NormalizedEdgeResponse& context
            , const Carna::Tools::Vector& position
            , const Carna::Tools::Vector& radialVector
            , double minimumContrast = 1. );

        double sample( double radius ) const;

    private:

        struct reallyLess
        {
            bool operator()( const double, const double ) const;
        };

        mutable std::map< double, double, reallyLess > samples;

    };  // RadialSampler

 // ----------------------------------------------------------------------------------

    double compute( const RadialSampler& sampler, double radius ) const;

 // ----------------------------------------------------------------------------------

private:

    MultiscaleDifferential differential;

    double currentRadiusSampleDistance;

}; // NormalizedEdgeResponse
