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

#include "Differential.h"
#include <set>



// ----------------------------------------------------------------------------------
// MultiscaleDifferential
// ----------------------------------------------------------------------------------

class MultiscaleDifferential
{

public:

    MultiscaleDifferential( Differential::Sampler* = nullptr );

    void setSampler( Differential::Sampler* );

    void setMinimumHUV( int );

    void setMaximumHUV( int );

    int minimumHUV() const;

    int maximumHUV() const;

    void setGamma( double );

    double gamma() const;


    typedef std::set< double > Scales;

    Scales scales;

    void setScales( double minScale, double maxScale, unsigned int scaleSamples );


    void partialDerivativeAt
        ( const Carna::Tools::Vector& position
        , const Carna::Tools::Vector& direction
        , double& partialDerivative
        , double& scale ) const;

    double partialDerivativeAt
        ( const Carna::Tools::Vector& position
        , const Carna::Tools::Vector& direction ) const;


private:

    mutable Differential differential;

}; // MultiscaleDifferential
