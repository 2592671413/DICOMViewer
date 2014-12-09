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

#include "SurfaceExtraction.h"
#include <TRTK/SurfaceExtraction3D.hpp>
#include <Carna/base/model/Position.h>
#include <QProgressDialog>
#include <QFuture>
#include <QtConcurrentRun>
#include <QFutureWatcher>



// ----------------------------------------------------------------------------------
// SurfaceExtraction
// ----------------------------------------------------------------------------------

SurfaceExtraction::SurfaceExtraction( QProgressDialog& progress
                                    , Record::Server& server
                                    , const Segmentation::MaskType& segmentationMask )
    : cloud( new PointCloud( server, PointCloud::volumeUnits ) )
{
    const Carna::base::Vector3ui& size = segmentationMask.getSize();

    typedef TRTK::SurfaceExtraction3D< Segmentation::MaskType::VoxelType > MySurfaceExtraction3D;
    MySurfaceExtraction3D surfaceExtraction3D( &( segmentationMask.getData().front() ), size.x, size.y, size.z );

    std::function< void() > extractSurface = [&](){ surfaceExtraction3D.compute(); };
    QFuture< void > extraction = QtConcurrent::run( extractSurface );

    QFutureWatcher< void > watcher;
    watcher.setFuture( extraction );
    QObject::connect( &watcher, SIGNAL( finished() ), &progress, SLOT( reset() ) );
    progress.exec();

    // fetch point cloud

    progress.setWindowTitle( "Surface Extraction" );
    progress.setLabelText( "Fetching point cloud..." );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );
    progress.setValue( 0 );
    progress.setMaximum( size.x - 1 );
    progress.show();

    const MySurfaceExtraction3D::data_type* surfaceData = surfaceExtraction3D.getSurface();

    for( unsigned int x = 0; x < size.x; ++x )
    {
        for( unsigned int y = 0; y < size.y; ++y )
        for( unsigned int z = 0; z < size.z; ++z )
        {
            const unsigned short mask_val = surfaceData[ z * size.x * size.y + y * size.x + x ];

            // process _FIRST_ surface

            if( mask_val == 1 )
            {
                const double ms_x = static_cast< double >( x ) / ( size.x - 1 );
                const double ms_y = static_cast< double >( y ) / ( size.y - 1 );
                const double ms_z = static_cast< double >( z ) / ( size.z - 1 );

                cloud->getList().push_back( PointCloud::Point( ms_x, ms_y, ms_z ) );
            }
        }

        progress.setValue( x );
    }
}
