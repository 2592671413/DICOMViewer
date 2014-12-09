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

#include <Carna/base/model/Volume.h>
#include <Carna/Carna.h>
#include <Carna/base/Association.h>
#include <Carna/base/model/UInt16Volume.h>



// ----------------------------------------------------------------------------------
// OptimizedVolumeDecorator
// ----------------------------------------------------------------------------------

class OptimizedVolumeDecorator : public Carna::base::model::Volume
{

public:

    OptimizedVolumeDecorator
        ( Carna::base::Association< const Carna::base::model::Volume >*
        , float spacingX
        , float spacingY
        , float spacingZ );

    virtual ~OptimizedVolumeDecorator();
    
    const Carna::base::model::Volume& original() const;


    virtual void uploadTexture() const override;

    virtual signed short operator()
        ( unsigned int x
        , unsigned int y
        , unsigned int z ) const override;

    virtual signed short operator()( const Carna::base::Vector3ui& at ) const override;


    bool testCacheHit( unsigned int x, unsigned int y, unsigned int z ) const;

    void updateCache( unsigned int x, unsigned int y, unsigned int z ) const;


private:

    const std::unique_ptr< Carna::base::Association< const Carna::base::model::Volume > > originalPtr;

    mutable std::unique_ptr< Carna::base::model::UInt16Volume > buffer;

    mutable bool bufferIsValid;

    mutable unsigned int bufferX;
    mutable unsigned int bufferY;
    mutable unsigned int bufferZ;

    const float spacingX;
    const float spacingY;
    const float spacingZ;

}; // OptimizedVolumeDecorator
