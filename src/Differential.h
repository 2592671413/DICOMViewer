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

#include <Carna/Transformation.h>



// ----------------------------------------------------------------------------------
// Differential
// ----------------------------------------------------------------------------------

class Differential
{

public:

    class Sampler
    {

    public:

        virtual ~Sampler();

        virtual double valueAt( const Carna::Tools::Vector& millimeters ) const = 0;

    }; // Sampler


    Differential( Sampler* = nullptr );


    void setScale( double scale );

    void setSampler( Sampler* );

    double partialDerivativeAt( const Carna::Tools::Vector& position, const Carna::Tools::Vector& direction ) const;

    void setMinimumHUV( int );

    void setMaximumHUV( int );

    int minimumHUV() const;

    int maximumHUV() const;

    void setGamma( double );

    double gamma() const;


private:

    double scale;

    double currentGamma;

    std::unique_ptr< Sampler > sampler;

    std::vector< double > gaussianDerivativePositions;

    std::vector< double > gaussianDerivativeSamples;

    int minHUV;

    int maxHUV;

    void updateGaussianDerivative( unsigned int samples );

}; // Differential
