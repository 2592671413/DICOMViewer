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

#include "GulsunSegmentation.h"
#include <TRTK/RegionGrowing3D.hpp>
#include <Carna/base/CarnaException.h>



// ----------------------------------------------------------------------------------
// ceil2uint
// ----------------------------------------------------------------------------------

template< typename T >
unsigned int ceil2uint( T x )
{
    return static_cast< unsigned int >( std::ceil( x ) + Carna::Tools::epsilon< T >() );
}



// ----------------------------------------------------------------------------------
// GulsunSegmentation :: Result :: less
// ----------------------------------------------------------------------------------

bool GulsunSegmentation::Result::less::operator()
    ( const Carna::base::Vector3ui& v1
    , const Carna::base::Vector3ui& v2 ) const
{
    if( v1.x == v2.x )
    {
        if( v1.y == v2.y )
        {
            return v1.z < v2.z;
        }
        else
        {
            return v1.y < v2.y;
        }
    }
    else
    {
        return v1.x < v2.x;
    }
}



// ----------------------------------------------------------------------------------
// GulsunSegmentation :: Result
// ----------------------------------------------------------------------------------

Carna::base::model::BufferedMaskAdapter::BinaryMask::ValueType GulsunSegmentation::Result::operator()
    ( unsigned int x
    , unsigned int y
    , unsigned int z ) const
{
    const auto itr = container.find( Carna::base::Vector3ui( x, y, z ) );

    if( itr == container.end() )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}



// ----------------------------------------------------------------------------------
// GulsunSegmentation
// ----------------------------------------------------------------------------------

GulsunSegmentation::GulsunSegmentation( const Gulsun& gulsun )
    : gulsun( gulsun )
    , canceled( false )
    , tolerance( 2. )
    , intervalSize( 3 )
    , smoothedRadii( false )
{
}


GulsunSegmentation::~GulsunSegmentation()
{
}


void GulsunSegmentation::setIntensityTolerance( double intensityTolerance )
{
    this->tolerance = intensityTolerance;
}


double GulsunSegmentation::intensityTolerance() const
{
    return tolerance;
}


void GulsunSegmentation::cancel()
{
    this->canceled = true;
}


Carna::base::model::BufferedMaskAdapter::BinaryMask& GulsunSegmentation::getMask() const
{
    CARNA_ASSERT( mask.get() != nullptr );

    return *mask;
}


Carna::base::model::BufferedMaskAdapter::BinaryMask* GulsunSegmentation::takeMask()
{
    CARNA_ASSERT( mask.get() != nullptr );

    return mask.release();
}


bool GulsunSegmentation::hasMask() const
{
    return mask.get() != nullptr;
}


void GulsunSegmentation::compute()
{
    const unsigned int progress_per_interval = 100;

    CARNA_ASSERT( nodesPerInterval() >= 2 );

    mask.reset();
    result.reset( new Result() );

    emit minimumChanged( 0 );
    emit maximumChanged( gulsun.vesselBranches().size() * progress_per_interval );
    QApplication::processEvents();

 // compute intervals

    this->canceled = false;
    unsigned int branch_index = 0;
    for( auto branch_itr = gulsun.vesselBranches().begin();
         !canceled && branch_itr != gulsun.vesselBranches().end();
         ++branch_itr )
    {
        const std::vector< unsigned int >& branch = *branch_itr;

     // segment intervals

        for( unsigned int first_node_index = 0; first_node_index < branch.size() - 1; first_node_index += nodesPerInterval() - 1 )
        {
            const unsigned int last_node_index = std::min( first_node_index + nodesPerInterval() - 1, branch.size() - 1 );

         // decompress branch

            std::vector< MedialnessGraph::Node > decompressed_branch( branch.size() );
            for( unsigned int i = 0; i < decompressed_branch.size(); ++i )
            {
                gulsun.decompressNode( decompressed_branch[ i ], branch[ i ] );
            }

         // segment interval

            segmentInterval( decompressed_branch, first_node_index, last_node_index );

            emit progressChanged( branch_index * progress_per_interval + int( progress_per_interval * last_node_index / double( branch.size() ) ) );
            QApplication::processEvents();
        }

        emit progressChanged( ( ++branch_index ) * progress_per_interval );
        QApplication::processEvents();
    }
    
    if( !canceled )
    {
        emit progressChanged( 0 );
        emit maximumChanged( 0 );
        QApplication::processEvents();

        mask.reset( new Carna::base::model::BufferedMaskAdapter::BinaryMask( gulsun.graph.model.volume().size, *result ) );
    }

    emit finished();
}


void GulsunSegmentation::segmentInterval
    ( const std::vector< MedialnessGraph::Node >& branch
    , unsigned int first
    , unsigned int last )
{
    CARNA_ASSERT( first < last );

    const unsigned int nodes_count = last - first + 1;
    std::vector< double > intensities( nodes_count );

    for( unsigned int i = first; i <= last; ++i )
    {
        const Carna::base::model::Position position = gulsun.graph.getNodePosition( branch[ i ] );
        double intensity = gulsun.graph.model.intensityAt( position );
        intensities[ i - first ] = intensity;
    }

 // compute mean intensity

    double mean_intensity = 0;
    std::for_each( intensities.begin(), intensities.end(),
        [&]( double intensity )
        {
            mean_intensity += intensity / nodes_count;
        }
    );

 // compute standard deviation

    double sq_differences_sum = 0;
    std::for_each( intensities.begin(), intensities.end(),
        [&]( double intensity )
        {
            sq_differences_sum += Carna::base::Math::sq( intensity - mean_intensity );
        }
    );

    const double standard_deviation = std::max( 0.5, std::sqrt( sq_differences_sum / ( nodes_count - 1 ) ) );

 // configure binary volume mask

    const int min_huv = static_cast< int >( mean_intensity - intensityTolerance() * standard_deviation );
    ClippedVolumeMask::Setup mask_setup( gulsun.graph.model, min_huv, 3071 );
    ClippedVolumeMask mask( [&]( const Carna::base::Vector3ui& voxel )->bool
        {
            return result->container.find( voxel ) == result->container.end();
        }
        , mask_setup );

    std::deque< TRTK::Coordinate< unsigned > > seed_points;
    Carna::base::Vector edge_support = gulsun.graph.getNodePosition( branch[ first ] ).toMillimeters();

    double lastRadius = -std::numeric_limits< double >::infinity();
    double nextRadius = gulsun.edgeRadiuses().radius( branch[ first ], branch[ first + 1 ] );

    for( unsigned int node_index = first; node_index < last; ++node_index )
    {
        const double radius = nextRadius;
        nextRadius = node_index + 2 <= last
            ?  gulsun.edgeRadiuses().radius( branch[ node_index + 1 ], branch[ node_index + 2 ] )
            : -std::numeric_limits< double >::infinity();

        CARNA_ASSERT( radius > 0 );

     // ----------------------------------------------------------------------------------
     // compute smoothed radius

        std::vector< double > radiusSamples;
        radiusSamples.push_back( radius );
        if( lastRadius > -std::numeric_limits< double >::infinity() )
        {
            radiusSamples.push_back( lastRadius );
        }
        if( nextRadius > -std::numeric_limits< double >::infinity() )
        {
            radiusSamples.push_back( nextRadius );
        }
        double smoothedRadius = 0;
        if( smoothedRadiuses() )
        {
            for( unsigned int i = 0; i < radiusSamples.size(); ++i )
            {
                smoothedRadius += radiusSamples[ i ] / radiusSamples.size();
            }
        }
        else
        {
            smoothedRadius = radius;
        }

        lastRadius = radius;

     // ----------------------------------------------------------------------------------

        const Carna::base::model::Position p0 = gulsun.graph.getNodePosition( branch[ node_index + 1 ] );
        const Carna::base::Vector edge_destination = p0.toMillimeters();
        const Carna::base::Vector edge_way = edge_destination - edge_support;

        mask_setup.addLine( ClippedVolumeMask::Line( edge_support, edge_way, smoothedRadius * radiusMultiplier() ) );

        edge_support = edge_destination;

     // compute and save seed point

        const auto positionToVoxel = [&]( const Carna::base::model::Position& p )->TRTK::Coordinate< unsigned >
        {
            return TRTK::Coordinate< unsigned >
                ( unsigned( p.toVolumeUnits().x() * ( gulsun.graph.model.volume().size.x - 1 ) + 0.5 )
                , unsigned( p.toVolumeUnits().y() * ( gulsun.graph.model.volume().size.y - 1 ) + 0.5 )
                , unsigned( p.toVolumeUnits().z() * ( gulsun.graph.model.volume().size.z - 1 ) + 0.5 ) );
        };
        const TRTK::Coordinate< unsigned > seed_point = positionToVoxel( p0 );

        seed_points.push_back( seed_point );

        /*
     // ----------------------------------------------------------------------------------
     // rasterize edge

        const Carna::base::model::Position p1 = gulsun.graph.getNodePosition( branch[ node_index ] );
        const TRTK::Coordinate< unsigned > sink_point = positionToVoxel( p1 );
        result->container.insert( Carna::base::Vector3ui( seed_point.x(), seed_point.y(), seed_point.z() ) );
        result->container.insert( Carna::base::Vector3ui( sink_point.x(), sink_point.y(), sink_point.z() ) );
        */
    }

 // run region growing

    TRTK::RegionGrowing3D
            < bool
            , uint8_t
            , ClippedVolumeMask& > region_growing
        ( mask
        , gulsun.graph.model.volume().size.x
        , gulsun.graph.model.volume().size.y
        , gulsun.graph.model.volume().size.z );

    region_growing.setNeighborhoodSize( 1 );
    region_growing.compute( seed_points );

    for( unsigned int label = 0; label < region_growing.getRegions().size(); ++label )
    {
        const auto region = region_growing.getRegions()[ label ];

        qDebug( "Obtained %d voxels from region growing.", region.size() );

        unsigned int duplicates = 0;
        for( unsigned int i = 0; i < region.size(); ++i )
        {
            if( !result->container.insert( Carna::base::Vector3ui
                ( region[ i ].x()
                , region[ i ].y()
                , region[ i ].z() ) ).second )
            {
                ++duplicates;
            }
        }

        CARNA_ASSERT( duplicates == 0 );

        qDebug( "Of these, %d were new and %d duplicates.\n", region.size() - duplicates, duplicates );
    }
}


void GulsunSegmentation::setNodesPerInterval( int intervalSize )
{
    CARNA_ASSERT( intervalSize >= 2 );

    this->intervalSize = intervalSize;
}


int GulsunSegmentation::nodesPerInterval() const
{
    return intervalSize;
}


void GulsunSegmentation::setRadiusMultiplier( double radiusFactor )
{
    this->radiusFactor = radiusFactor;
}


double GulsunSegmentation::radiusMultiplier() const
{
    return radiusFactor;
}


void GulsunSegmentation::setSmoothedRadiuses( bool smoothedRadii )
{
    this->smoothedRadii = smoothedRadii;
}


bool GulsunSegmentation::smoothedRadiuses() const
{
    return smoothedRadii;
}
