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

#include "OptimizedVolumeDecorator.h"
#include "CarnaContextClient.h"
#include <Carna/Model.h>



// ----------------------------------------------------------------------------------
// OptimizedVolumeDecorator
// ----------------------------------------------------------------------------------

OptimizedVolumeDecorator::OptimizedVolumeDecorator
    ( Carna::Tools::Association< const Carna::Volume >* originalPtr
    , float spacingX
    , float spacingY
    , float spacingZ )

    : Volume( originalPtr->get()->size )
    , originalPtr( originalPtr )
    , bufferIsValid( false )
    , spacingX( spacingX )
    , spacingY( spacingY )
    , spacingZ( spacingZ )
{
    static const double BUFFER_SIZE_MILLIMETERS = 20.;

    const unsigned int sx = static_cast< unsigned int>( std::ceil( BUFFER_SIZE_MILLIMETERS / spacingX ) + 0.1 );
    const unsigned int sy = static_cast< unsigned int>( std::ceil( BUFFER_SIZE_MILLIMETERS / spacingY ) + 0.1 );
    const unsigned int sz = static_cast< unsigned int>( std::ceil( BUFFER_SIZE_MILLIMETERS / spacingZ ) + 0.1 );

    buffer.reset( new Carna::UInt16Volume( Carna::Tools::Vector3ui( sx, sy, sz ) ) );
}


OptimizedVolumeDecorator::~OptimizedVolumeDecorator()
{
}


const Carna::Volume& OptimizedVolumeDecorator::original() const
{
    return *originalPtr->get();
}


void OptimizedVolumeDecorator::uploadTexture() const
{
    original().uploadTexture();
}


signed short OptimizedVolumeDecorator::operator()( const Carna::Tools::Vector3ui& at ) const
{
    return ( *this )( at.x, at.y, at.z );
}


signed short OptimizedVolumeDecorator::operator()
    ( unsigned int x
    , unsigned int y
    , unsigned int z ) const
{
    if( !testCacheHit( x, y, z ) )
    {
        unsigned int dx = 0, dy = 0, dz = 0;
        const unsigned int x0 = bufferX, y0 = bufferY, z0 = bufferZ;
        const unsigned int x1 = x0 + buffer->size.x, y1 = y0 + buffer->size.y, z1 = z0 + buffer->size.z;

        if( x < x0 )
        {
            dx = x0 - x;
        }
        else
        if( x >= x1 )
        {
            dx = 1 + x - x1;
        }
        
        if( y < y0 )
        {
            dy = y0 - y;
        }
        else
        if( y >= y1 )
        {
            dy = 1 + y - y1;
        }
        
        if( z < z0 )
        {
            dz = z0 - z;
        }
        else
        if( z >= z1 )
        {
            dz = 1 + z - z1;
        }

        qWarning
            ( "OptimizedVolumeDecorator: Cache Miss! dx = %f mm, dy = %f mm, dz = %f mm"
            , dx * spacingX
            , dy * spacingY
            , dz * spacingZ );

        updateCache( x, y, z );
    }

    return ( *buffer )( x - bufferX, y - bufferY, z - bufferZ );
}


bool OptimizedVolumeDecorator::testCacheHit( unsigned int x, unsigned int y, unsigned int z ) const
{
    return bufferIsValid && !(
        x < bufferX || x >= bufferX + buffer->size.x ||
        y < bufferY || y >= bufferY + buffer->size.y ||
        z < bufferZ || z >= bufferZ + buffer->size.z );
}


void OptimizedVolumeDecorator::updateCache( unsigned int x, unsigned int y, unsigned int z ) const
{
    bufferX = Carna::Tools::clamp( signed( x ) - signed( buffer->size.x / 2 ), 0, signed( original().size.x - 1 - buffer->size.x ) );
    bufferY = Carna::Tools::clamp( signed( y ) - signed( buffer->size.y / 2 ), 0, signed( original().size.y - 1 - buffer->size.y ) );
    bufferZ = Carna::Tools::clamp( signed( z ) - signed( buffer->size.z / 2 ), 0, signed( original().size.z - 1 - buffer->size.z ) );

    const Carna::UInt16Volume* const original = dynamic_cast< const Carna::UInt16Volume* >( &this->original() );
    if( original != nullptr )
    {
        for( unsigned int z = bufferZ; z < bufferZ + buffer->size.z; ++z )
        for( unsigned int y = bufferY; y < bufferY + buffer->size.y; ++y )
        {
            const unsigned int src_index = bufferX + y * original->size.x + z * original->size.x * original->size.y;
            const unsigned int dst_index = ( y - bufferY ) * buffer->size.x + ( z - bufferZ ) * buffer->size.x * buffer->size.y;

            CARNA_ASSERT( src_index >= 0 && src_index + buffer->size.x <= original->getBuffer().size() );
            CARNA_ASSERT( dst_index >= 0 && dst_index + buffer->size.x <= buffer->getBuffer().size() );

#if 0
            std::copy
                ( original->getBuffer().begin() + src_index
                , original->getBuffer().begin() + src_index + buffer->size.x
                , buffer->getBuffer().begin() + dst_index );
#else
            memcpy
                ( &buffer->getBuffer()[ dst_index ]
                , &original->getBuffer()[ src_index ]
                , buffer->size.x * 2 /*bytes per element*/ );
#endif
        }

        bufferIsValid = true;
    }
}
