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

#include "NormalizedEdgeResponse.h"
#include <cstdlib>



// ----------------------------------------------------------------------------------
// Medialness
// ----------------------------------------------------------------------------------

class Medialness
{

public:

    Medialness( Differential::Sampler* = nullptr );

    ~Medialness();

    void setSampler( Differential::Sampler* );

    void setMinimumHUV( int );

    void setMaximumHUV( int );

    int minimumHUV() const;

    int maximumHUV() const;

    void setScales( double minScale, double maxScale, unsigned int scaleSamples );

    void setRadiuses( double minRadius, double maxRadius, unsigned int radiusSamples );

    void setMinimumContrast( double );

    double minimumContrast() const;

    void setGamma( double );

    double gamma() const;

 // ----------------------------------------------------------------------------------

    void compute
        ( const Carna::base::Vector& position
        , const Carna::base::Vector& vesselDirection
        , double& medialness
        , double& radius
        , double minimumMedialness = -std::numeric_limits< double >::infinity() ) const;


private:

    NormalizedEdgeResponse edgeResponse;

    mutable std::vector< NormalizedEdgeResponse::RadialSampler* > directionData;

    double currentMinimumContrast;

    void updateDirectionData
        ( const Carna::base::Vector& position
        , const Carna::base::Vector& vesselDirection ) const;

    void deleteDirectionData() const;

}; // Medialness
