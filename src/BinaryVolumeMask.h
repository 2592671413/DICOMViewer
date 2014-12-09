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



// ----------------------------------------------------------------------------------
// BinaryVolumeMask
// ----------------------------------------------------------------------------------

class BinaryVolumeMask
{

public:

    BinaryVolumeMask( int min, int max, const Carna::Volume& volume )
        : min( min )
        , max( max )
        , volume( &volume )
    {
    }

    BinaryVolumeMask()
        : min( 0 )
        , max( 0 )
        , volume( 0 )
    {
    }

    virtual ~BinaryVolumeMask()
    {
    }


    bool operator[]( unsigned int index ) const
    {
        if( !volume )
        {
            throw std::logic_error( "Uninitialized binary volume mask." );
        }

        const unsigned int w = volume->size.x;
        const unsigned int h = volume->size.y;
        const unsigned int z = index / ( w * h );
        const unsigned int y = ( index - z * ( w * h ) ) / volume->size.x;
        const unsigned int x = index - ( z * w * h + y * w );

        return test( x, y, z );
    }

    virtual bool test( unsigned int x, unsigned int y, unsigned int z ) const
    {
        const signed short huv = ( *volume )( x, y, z );

        CARNA_ASSERT( huv >= -1024 );
        CARNA_ASSERT( huv <=  3071 );

        const bool result = min <= huv && huv <= max;

        return result;
    }

    BinaryVolumeMask& operator=( const BinaryVolumeMask& cp )
    {
        volume = cp.volume;

        min = cp.min;
        max = cp.max;

        return *this;
    }


private:

    const Carna::Volume* volume;

    int min;

    int max;

}; // BinaryVolumeMask
