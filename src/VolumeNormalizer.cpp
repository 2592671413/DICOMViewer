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

#include "VolumeNormalizer.h"
#include <Carna/base/model/Scene.h>
#include <Carna/base/model/Volume.h>
#include <Carna/base/model/UInt16Volume.h>
#include <Carna/base/CarnaException.h>
#include <QProgressDialog>
#include <QApplication>



// ----------------------------------------------------------------------------------
// VolumeNormalizer
// ----------------------------------------------------------------------------------

VolumeNormalizer::VolumeNormalizer( const Carna::base::model::Scene& model, QWidget* parent )
    : model( model )
    , parent( parent )
    , threshold( -1024 )
    , minimalSize( 8 )
    , minX( 0 )
    , minY( 0 )
    , minZ( 0 )
    , maxX( model.volume().size.x - 1 )
    , maxY( model.volume().size.y - 1 )
    , maxZ( model.volume().size.z - 1 )
    , sizeLoss( 1. )
{
}


VolumeNormalizer::~VolumeNormalizer()
{
}


int VolumeNormalizer::getThreshold() const
{
    return threshold;
}


void VolumeNormalizer::setThreshold( int threshold )
{
    CARNA_ASSERT( -1024 <= threshold && threshold <= 3071 );

    if( this->threshold != threshold )
    {
        this->threshold = threshold;

        emit thresholdChanged( this->threshold );
    }
}


int VolumeNormalizer::getMinimalSize() const
{
    return minimalSize;
}


void VolumeNormalizer::setMinimalSize( int minimalSize )
{
    CARNA_ASSERT( minimalSize >= 2 );

    if( this->minimalSize != minimalSize )
    {
        this->minimalSize = minimalSize;

        emit minimalSizeChanged( this->minimalSize );
    }
}


void VolumeNormalizer::compute()
{
    CARNA_ASSERT( -1024 <= threshold && threshold <= 3071 );
    CARNA_ASSERT( minimalSize >= 2 );

    QProgressDialog status( "Shrinking...", "Abort", 0, 0, parent );
    status.setWindowTitle( "Volume Normalization" );
    status.setWindowModality( Qt::WindowModal );
    status.show();

 // reset bounding box

    minX = minY = minZ = 0;
    maxX = model.volume().size.x - 1;
    maxY = model.volume().size.y - 1;
    maxZ = model.volume().size.z - 1;

 // shrink bounding box

    bool repeat;
    do
    {
        repeat = false;

        if( discardsSliceX( minX ) )
        {
            ++minX;
            repeat = true;
        }
        if( discardsSliceY( minY ) )
        {
            ++minY;
            repeat = true;
        }
        if( discardsSliceZ( minZ ) )
        {
            ++minZ;
            repeat = true;
        }

        if( discardsSliceX( maxX ) )
        {
            --maxX;
            repeat = true;
        }
        if( discardsSliceY( maxY ) )
        {
            --maxY;
            repeat = true;
        }
        if( discardsSliceZ( maxZ ) )
        {
            --maxZ;
            repeat = true;
        }

        status.setValue( 0 );
        QApplication::processEvents();
        if( status.wasCanceled() )
        {
            repeat = false;
        }
    }
    while( repeat );

    CARNA_ASSERT( maxX - minX >= minimalSize );
    CARNA_ASSERT( maxY - minY >= minimalSize );
    CARNA_ASSERT( maxZ - minZ >= minimalSize );

 // compute size loss

    unsigned long initial_resolution = model.volume().size.x * model.volume().size.y * model.volume().size.z;
    unsigned long reduced_resolution = ( maxX - minX + 1 ) * ( maxY - minY + 1 ) * ( maxZ - minZ + 1 );

    this->sizeLoss = static_cast< double >( reduced_resolution ) / initial_resolution;

 // finish

    emit computed();
    emit computed( sizeLoss );
}


bool VolumeNormalizer::discardsSliceX( const unsigned x ) const
{
    CARNA_ASSERT( maxX - minX >= minimalSize );

    if( maxX - minX == minimalSize )
    {
        return false;
    }

    for( unsigned int y = minY; y <= maxY; ++y )
    for( unsigned int z = minZ; z <= maxZ; ++z )
    {
        if( model.volume()( x, y, z ) > getThreshold() )
        {
            return false;
        }
    }
    return true;
}


bool VolumeNormalizer::discardsSliceY( const unsigned y ) const
{
    CARNA_ASSERT( maxY - minY >= minimalSize );

    if( maxY - minY == minimalSize )
    {
        return false;
    }

    for( unsigned int x = minX; x <= maxX; ++x )
    for( unsigned int z = minZ; z <= maxZ; ++z )
    {
        if( model.volume()( x, y, z ) > getThreshold() )
        {
            return false;
        }
    }
    return true;
}


bool VolumeNormalizer::discardsSliceZ( const unsigned z ) const
{
    CARNA_ASSERT( maxZ - minZ >= minimalSize );

    if( maxZ - minZ == minimalSize )
    {
        return false;
    }

    for( unsigned int x = minX; x <= maxX; ++x )
    for( unsigned int y = minY; y <= maxY; ++y )
    {
        if( model.volume()( x, y, z ) > getThreshold() )
        {
            return false;
        }
    }
    return true;
}


Carna::base::model::Scene* VolumeNormalizer::getResult() const
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    const Carna::base::Vector3ui size( maxX - minX + 1, maxY - minY + 1, maxZ - minZ + 1 );

    Carna::base::model::UInt16Volume* const new_volume = new Carna::base::model::UInt16Volume( size );

    for( unsigned z = 0; z < size.z; ++z )
    for( unsigned y = 0; y < size.y; ++y )
    for( unsigned x = 0; x < size.x; ++x )
    {
        const short huv = model.volume()( minX + x, minY + y, minZ + z );
        new_volume->setVoxel( x, y, z, huv );
    }

    Carna::base::model::Scene* const new_model = new Carna::base::model::Scene( new Carna::base::Composition< Carna::base::model::Volume >( new_volume )
                                                    , model.spacingX()
                                                    , model.spacingY()
                                                    , model.spacingZ() );

    QApplication::restoreOverrideCursor();

    return new_model;
}


double VolumeNormalizer::getSizeLoss() const
{
    return sizeLoss;
}
