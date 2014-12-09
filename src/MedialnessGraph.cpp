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

#include "MedialnessGraph.h"
#include <Carna/Scene.h>
#include <Carna/Model.h>
#include <Carna/Volume.h>
#include <Carna/VisualizationEnvironment.h>
#include <Carna/glError.h>
#include <QGLFramebufferObject>



// ----------------------------------------------------------------------------------
// IntensitySampler
// ----------------------------------------------------------------------------------

IntensitySampler::IntensitySampler( const Carna::Model& model )
    : model( model )
{
}


double IntensitySampler::valueAt( const Carna::Tools::Vector& millimeters ) const
{
#if 1
    return model.intensityAt( Carna::Position::fromMillimeters( model, millimeters ) );
#else
    /*
    const double x = millimeters.x() / model.spacingX();
    const double y = millimeters.y() / model.spacingY();
    const double z = millimeters.z() / model.spacingZ();
    */

    const Carna::Tools::Vector vu = Carna::Position::fromMillimeters( model, millimeters ).toVolumeUnits();
    const double x = vu.x() * ( model.volume().size.x - 1 );
    const double y = vu.y() * ( model.volume().size.y - 1 );
    const double z = vu.z() * ( model.volume().size.z - 1 );

    /*
    const auto position = Carna::Position::fromMillimeters( model, millimeters );
    const double x = position.toVolumeUnits().x() * ( model.volume().size.x - 1 );
    const double y = position.toVolumeUnits().y() * ( model.volume().size.y - 1 );
    const double z = position.toVolumeUnits().z() * ( model.volume().size.z - 1 );
    */

    const auto sampleAt = [&]( int i, int j, int k )->signed short
    {
        if( i < 0 || j < 0 || k < 0 ||
            i >= static_cast< signed >( model.volume().size.x ) ||
            j >= static_cast< signed >( model.volume().size.y ) ||
            k >= static_cast< signed >( model.volume().size.z ) )
        {
            return -1024;
        }
        else
        {
            /*
            return model.volume()
                ( static_cast< unsigned int >( i )
                , static_cast< unsigned int >( j )
                , static_cast< unsigned int >( k ) );
                */

            /*
            return static_cast< signed short >( static_cast< const Carna::UInt16Volume& >( model.volume() ).getBuffer()[ i + model.volume().size.x * j + model.volume().size.y * model.volume().size.x * k ] >> 4 ) - 1024;
            */

            unsigned short value = static_cast< const Carna::UInt16Volume& >( model.volume() ).getBuffer()
                [ i + model.volume().size.x * j + model.volume().size.y * model.volume().size.x * k ];

            return int( ( value / double( std::numeric_limits< unsigned short >::max() ) ) * (1024+3071)  ) - 1024;
        }
    };

    return sampleAt
        ( static_cast< int >( x + 0.5 )
        , static_cast< int >( y + 0.5 )
        , static_cast< int >( z + 0.5 ) );
#endif
}



// ----------------------------------------------------------------------------------
// GpuIntensitySampler
// ----------------------------------------------------------------------------------

GpuIntensitySampler::GpuIntensitySampler( Carna::VisualizationEnvironment& environment, Carna::Scene& scene )
    : Carna::Scene::ResourceContext( environment, scene )
    , fbo( new QGLFramebufferObject // the proper GL context is activated by the above constructor
        ( 1, 1
        , QGLFramebufferObject::NoAttachment
        , GL_TEXTURE_2D, GL_RGBA16F ) )
{
}


GpuIntensitySampler::~GpuIntensitySampler()
{
    environment.activateContext();
    fbo.reset();
}


double GpuIntensitySampler::valueAt( const Carna::Tools::Vector& millimeters ) const
{
    const Carna::Tools::Vector vu = Carna::Position::fromMillimeters( scene.model, millimeters ).toVolumeUnits();

    if( vu.x() < 0 || vu.y() < 0 || vu.z() < 0 || vu.x() >= 1 || vu.y() >= 1 || vu.z() >= 1  )
    {
        return -1024;
    }

    environment.activateContext();

    CARNA_ASSERT( fbo->bind() );

    glBindTexture( GL_TEXTURE_3D, this->volumeTextureID );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0, 1, 0, 1, -1, 1 );

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib( GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT );
    glViewport( 0, 0, 1, 1 );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glColor4f( 1, 1, 1, 1 );
    glEnable( GL_TEXTURE_3D );
    glBegin( GL_POINTS );
        glTexCoord3f( vu.x(), vu.y(), vu.z() );
        glVertex3f( 0, 0, 0 );
    glEnd();
    glPopAttrib();

    glPopMatrix();
    glMatrixMode( GL_PROJECTION );

    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    CARNA_ASSERT( fbo->release() );

    float intensity;
    glBindTexture( GL_TEXTURE_2D, fbo->texture() );
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, &intensity );
    intensity = intensity * 4095 - 1024;

    glBindTexture( GL_TEXTURE_3D, this->volumeTextureID );

    REPORT_GL_ERROR;

    return intensity;
}



// ----------------------------------------------------------------------------------
// MedialnessGraph
// ----------------------------------------------------------------------------------

const double MedialnessGraph::millimetersPerNode = 1./3;
/*
const double MedialnessGraph::millimetersPerNode = 0.25;
*/


MedialnessGraph::MedialnessGraph
    ( Differential::Sampler* sampler
    , Carna::Model& model
    , const Setup& setup
    , const EdgeRadiusConsumer& produceRadius )

    : model( model )
    , currentSetup( setup )
    , medialnessFilter( sampler )
    , produceRadius( produceRadius )
    , detailedDebug( true )
    , size( [&]()->Carna::Tools::Vector3ui
            {
                const unsigned int max_x = static_cast< unsigned int >( ( model.volume().size.x - 1 ) * model.spacingX() / millimetersPerNode );
                const unsigned int max_y = static_cast< unsigned int >( ( model.volume().size.y - 1 ) * model.spacingY() / millimetersPerNode );
                const unsigned int max_z = static_cast< unsigned int >( ( model.volume().size.z - 1 ) * model.spacingZ() / millimetersPerNode );
                return Carna::Tools::Vector3ui( max_x + 1, max_y + 1, max_z + 1 );
            }
        () )
    , sampler( [&]()->Differential::Sampler&
            {
                CARNA_ASSERT( sampler != nullptr );
                return *sampler;
            }
        () )
{
    CARNA_ASSERT( size.x * size.y * size.z <= std::numeric_limits< unsigned int >::max() );

    this->configure( setup );
}


MedialnessGraph::~MedialnessGraph()
{
}


MedialnessGraph::Node MedialnessGraph::pickNode( const Carna::Position& position ) const
{
    const unsigned int x = static_cast< unsigned int >( position.toMillimeters().x() / millimetersPerNode + 0.5 );
    const unsigned int y = static_cast< unsigned int >( position.toMillimeters().y() / millimetersPerNode + 0.5 );
    const unsigned int z = static_cast< unsigned int >( position.toMillimeters().z() / millimetersPerNode + 0.5 );

    return Carna::Tools::Vector3ui( x, y, z );
}


Carna::Position MedialnessGraph::getNodePosition( const Carna::Tools::Vector3ui& node ) const
{
    return Carna::Position::fromMillimeters( model, node.x * millimetersPerNode, node.y * millimetersPerNode, node.z * millimetersPerNode );
}


unsigned int MedialnessGraph::computeNodeIndex( const Carna::Tools::Vector3ui& node ) const
{
    return node.x + size.x * node.y + size.y * size.x * node.z;
}


void MedialnessGraph::configure( const Setup& setup )
{
    medialnessFilter.setScales( setup.minimumScale, setup.maximumScale, setup.scaleSamples );
    medialnessFilter.setRadiuses( setup.minimumRadius, setup.maximumRadius, setup.radiusSamples );
    medialnessFilter.setMinimumContrast( setup.minimumContrast );
    medialnessFilter.setMinimumHUV( setup.minimumHUV );
    medialnessFilter.setMaximumHUV( setup.maximumHUV );
    currentSetup = setup;
}


const MedialnessGraph::Setup& MedialnessGraph::setup() const
{
    return currentSetup;
}


void MedialnessGraph::computeEdge
    ( const Carna::Tools::Vector& p0
    , const Carna::Tools::Vector& p1
    , double& medialness
    , double& radius ) const
{
    const Carna::Tools::Vector direction = p1 - p0;
    const double minimumMedialness = setup().allowMedialnessEarlyOut
        ? setup().minimumMedialness
        : -std::numeric_limits< double >::infinity();

    switch( setup().edgeEvaluator )
    {

        case Setup::byDestination:
        {
            medialnessFilter.compute
                ( p1
                , direction
                , medialness
                , radius
                , minimumMedialness );

            break;
        }

        case Setup::gaussian:
        {
            medialnessFilter.compute
                ( p0 + direction / 2
                , direction
                , medialness
                , radius
                , minimumMedialness );

            medialness *= direction.norm();
            break;
        }

        case Setup::trapeze:
        {
            double m1, m2, r1, r2;

            medialnessFilter.compute
                ( p0
                , direction
                , m1
                , r1
                , minimumMedialness );

            medialnessFilter.compute
                ( p1
                , direction
                , m2
                , r2
                , minimumMedialness );

            medialness = ( m1 + m2 ) * direction.norm() / 2;
            radius = ( r1 + r2 ) / 2;
            break;
        }

        case Setup::simpson:
        {
            double m1, m2, m3, r1, r2, r3;

            medialnessFilter.compute
                ( p0
                , direction
                , m1
                , r1
                , minimumMedialness );

            medialnessFilter.compute
                ( p0 + direction / 2
                , direction
                , m2
                , r2
                , minimumMedialness );

            medialnessFilter.compute
                ( p1
                , direction
                , m3
                , r3
                , minimumMedialness );

            medialness = ( m1 + 4 * m2 + m3 ) * direction.norm() / 6;
            radius = ( r1 + r2 + r3 ) / 3;
        }

        default:
        {
            CARNA_FAIL( "Specified number of samples per edge is not supported." );
        }

    }
}


void MedialnessGraph::expand( const Node& probedNode, OrderedEdges& edges ) const
{
    expand( probedNode, edges,
        []( const Node& )->bool
        {
            return true;
        }
    );
}


void MedialnessGraph::expand
    ( const Node& probedNode
    , OrderedEdges& edges
    , const std::function< bool( const Node& ) >& isReachable ) const
{
    const Carna::Position probedNodePosition = getNodePosition( probedNode );

    qDebug() << "Expanding Node: (" << probedNode.x << "," << probedNode.y << "," << probedNode.z << ")";

    unsigned int processed_edges = 0;
    for( int x = signed( probedNode.x ) - 1; x <= signed( probedNode.x ) + 1; ++x )
    for( int y = signed( probedNode.y ) - 1; y <= signed( probedNode.y ) + 1; ++y )
    for( int z = signed( probedNode.z ) - 1; z <= signed( probedNode.z ) + 1; ++z )
    {
        const Node neighbor( x, y, z );

        ++processed_edges;
        if( x < 0 || y < 0 || z < 0 ||
            x >= signed( size.x ) || y >= signed( size.y ) || z >= signed( size.z ) ||
            ( x == probedNode.x && y == probedNode.y && z == probedNode.z ) ||
            !isReachable( neighbor ) )
        {
            if( detailedDebug )
            {
                qDebug( " (skip) %2d/27 - (%+d, %+d, %+d)", processed_edges, x - probedNode.x, y - probedNode.y, z - probedNode.z );
            }
            continue;
        }

        const int dx = x - signed( probedNode.x );
        const int dy = y - signed( probedNode.y );
        const int dz = z - signed( probedNode.z );
        const Carna::Tools::Vector neighborPosition = probedNodePosition.toMillimeters() + millimetersPerNode * Carna::Tools::Vector( dx, dy, dz );

        double medialness, radius;
        computeEdge( probedNodePosition.toMillimeters(), neighborPosition, medialness, radius );

        QString status;
        if( medialness >= setup().minimumMedialness )
        {
            CARNA_ASSERT( radius > 0 );

            const double weight = 1. / medialness;
            /*
            const double weight = 1. / Carna::Tools::sq( medialness );
            */
            edges.insert( std::pair< double, Node >( weight, neighbor ) );
            produceRadius( probedNode, neighbor, radius );
            status = "( OK )";
        }
        else
        {
            status = "(skip)";
        }

        if( detailedDebug )
        {
            if( medialness > -std::numeric_limits< double >::infinity() )
            {
                QString output = QString( " " ) + status + QString( " %2d/27 - (%+d, %+d, %+d) - Medialness: %f - Radius: %f" );
                qDebug
                    ( output.toStdString().c_str()
                    , processed_edges
                    , x - probedNode.x
                    , y - probedNode.y
                    , z - probedNode.z
                    , medialness
                    , radius );
            }
            else
            {
                QString output = QString( " " ) + status + QString( " %2d/27 - (%+d, %+d, %+d) - early out" );
                qDebug
                    ( output.toStdString().c_str()
                    , processed_edges
                    , x - probedNode.x
                    , y - probedNode.y
                    , z - probedNode.z );
            }
        }
    }
}


bool MedialnessGraph::isBidirectional() const
{
    return setup().edgeEvaluator != Setup::byDestination;
}


void MedialnessGraph::setDetailedDebug( bool detailedDebug )
{
    this->detailedDebug = detailedDebug;
}


bool MedialnessGraph::hasDetailedDebug() const
{
    return detailedDebug;
}


void MedialnessGraph::fetchNodeByIndex( MedialnessGraph::Node& node, const unsigned int index ) const
{
    const unsigned int nodes_per_slice = size.x * size.y;
    node.z = index / nodes_per_slice;
    const unsigned int index_in_slice = index - node.z * nodes_per_slice;

    const unsigned int nodes_per_line = size.x;
    node.y = index_in_slice / nodes_per_line;
    node.x = index_in_slice - node.y * nodes_per_line;

    CARNA_ASSERT( index == computeNodeIndex( node ) );
}
