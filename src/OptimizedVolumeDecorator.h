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

#include <Carna/Volume.h>
#include <Carna/Carna.h>
#include <Carna/Association.h>
#include <Carna/UInt16Volume.h>



// ----------------------------------------------------------------------------------
// OptimizedVolumeDecorator
// ----------------------------------------------------------------------------------

class OptimizedVolumeDecorator : public Carna::Volume
{

public:

    OptimizedVolumeDecorator
        ( Carna::Tools::Association< const Carna::Volume >*
        , float spacingX
        , float spacingY
        , float spacingZ );

    virtual ~OptimizedVolumeDecorator();
    
    const Carna::Volume& original() const;


    virtual void uploadTexture() const override;

    virtual signed short operator()
        ( unsigned int x
        , unsigned int y
        , unsigned int z ) const override;

    virtual signed short operator()( const Carna::Tools::Vector3ui& at ) const override;


    bool testCacheHit( unsigned int x, unsigned int y, unsigned int z ) const;

    void updateCache( unsigned int x, unsigned int y, unsigned int z ) const;


private:

    const std::unique_ptr< Carna::Tools::Association< const Carna::Volume > > originalPtr;

    mutable std::unique_ptr< Carna::UInt16Volume > buffer;

    mutable bool bufferIsValid;

    mutable unsigned int bufferX;
    mutable unsigned int bufferY;
    mutable unsigned int bufferZ;

    const float spacingX;
    const float spacingY;
    const float spacingZ;

}; // OptimizedVolumeDecorator
