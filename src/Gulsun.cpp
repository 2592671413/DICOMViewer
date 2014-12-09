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

#include "Gulsun.h"
#include "LeafFinder.h"
#include <Carna/base/view/Polyline.h>



// ----------------------------------------------------------------------------------
// Gulsun :: DijkstraTreeAdapter
// ----------------------------------------------------------------------------------

struct Gulsun::DijkstraTreeAdapter
{

    typedef Dijkstra::Node Node;
    typedef std::set< Node, MedialnessGraphSupport::less > NodeSet;

    const Node& root;
    const Dijkstra& dijkstra;

    void fetchChildren( NodeSet& dst, const Node& of ) const;

    DijkstraTreeAdapter( Dijkstra& dijkstra );

};  // Gulsun :: DijkstraTreeAdapter


Gulsun::DijkstraTreeAdapter::DijkstraTreeAdapter( Dijkstra& dijkstra )
    : root( dijkstra.rootNode() )
    , dijkstra( dijkstra )
{
}


void Gulsun::DijkstraTreeAdapter::fetchChildren( NodeSet& dst, const Node& of ) const
{
    dijkstra.visitSuccessors( of,
        [&]( const Node& child )->bool
        {
            dst.insert( child );
            return true;
        }
    );
}



// ----------------------------------------------------------------------------------
// Gulsun
// ----------------------------------------------------------------------------------

Gulsun::Gulsun( MedialnessGraph& graph, const MedialnessGraph::Node& root, QObject* parent )
    : QObject( parent )
    , graph( graph )
    , canceled( false )
    , dijkstra( new Dijkstra( graph, root ) )
    , radiuses( graph )
    , currentMinimumLengthToRadiusRatio( 2. )
    , paths( 0 )
    , longestPathLength( 0 )
{
}


Gulsun::~Gulsun()
{
    deleteCenterlines();
}


void Gulsun::close()
{
    dijkstra.reset();
    deleteCenterlines();

 // ----------------------------------------------------------------------------------

    std::set< unsigned int > nodes_of_significant_paths;
    for( auto path_itr = significantPaths.begin(); path_itr != significantPaths.end(); ++path_itr )
    {
        for( auto node_itr = path_itr->begin(); node_itr != path_itr->end(); ++node_itr )
        {
            nodes_of_significant_paths.insert( *node_itr );
        }
    }

    const auto isNotSignificant = [&]( unsigned int from, unsigned int to )->bool
    {
        return
            nodes_of_significant_paths.find( from ) == nodes_of_significant_paths.end() ||
            nodes_of_significant_paths.find( to ) == nodes_of_significant_paths.end();
    };

    radiuses.removeRadiuses( isNotSignificant );
}


bool Gulsun::isClosed() const
{
    return dijkstra.get() == nullptr;
}


bool Gulsun::doNext()
{
    bool result;
    if( isClosed() )
    {
        result = false;
    }
    else
    {
        if( dijkstra->next() )
        {
            emit nodesExpanded( countExpandedNodes() );

            fetchCenterlines();
            result = true;
        }
        else
        {
            result = false;
        }
    }
    emit finished();
    return result;
}


void Gulsun::doUpTo( unsigned int maxSteps )
{
    if( !isClosed() )
    {
        bool iterated = false;
        canceled = false;
        for( unsigned int i = 0; i < maxSteps && !canceled && dijkstra->next(); ++i )
        {
            emit nodesExpanded( countExpandedNodes() );

            qDebug( "%d nodes expanded.\n", i + 1 );
            iterated = true;

            QApplication::processEvents();
        }
        if( iterated )
        {
            fetchCenterlines();
        }
    }
    emit finished();
}


void Gulsun::doAll()
{
    if( !isClosed() )
    {
        bool iterated = false;
        canceled = false;
        for( unsigned int i = 0; !canceled && dijkstra->next(); ++i )
        {
            emit nodesExpanded( countExpandedNodes() );

            qDebug( "%d nodes expanded.\n", i + 1 );
            iterated = true;

            QApplication::processEvents();
        }
        if( iterated )
        {
            fetchCenterlines();
        }
    }
    emit finished();
}


void Gulsun::cancel()
{
    this->canceled = true;
}


void Gulsun::updatePaths()
{
    if( !isClosed() )
    {
        fetchCenterlines();
        createCenterlines();
    }
}


void Gulsun::fetchCenterlines()
{
    CARNA_ASSERT( !isClosed() );

    DijkstraTreeAdapter tree( *dijkstra );
    typedef LeafFinder< DijkstraTreeAdapter > LeafFinder;
    LeafFinder leafFinder( tree );
    const LeafFinder::PathSet& paths = leafFinder.leafPaths();

 // update / reset stats

    this->paths = paths.size();
    this->longestPathLength = -std::numeric_limits< double >::infinity();

 // sort paths by length

    struct EvaluatedPath
    {
        LeafFinder::Path& path()
        {
            return *pathPtr;
        }

        double length;

        EvaluatedPath( LeafFinder::Path& path, double length = 0 )
            : pathPtr( &path )
            , length( length )
        {
        }

        struct shorter
        {
            bool operator()( const EvaluatedPath& p1, const EvaluatedPath& p2 ) const
            {
                return p1.length < p2.length;
            }
        };

    private:

        LeafFinder::Path* pathPtr;
    };
    std::priority_queue< EvaluatedPath, std::vector< EvaluatedPath >, EvaluatedPath::shorter > pathQueue;

    for( auto path_ptr_itr = paths.begin(); path_ptr_itr != paths.end(); ++path_ptr_itr )
    {
        LeafFinder::Path& path = **path_ptr_itr;
        if( path.size() >= 2 )
        {
            const double pathLength = computePathLength( path );
            pathQueue.push( EvaluatedPath( path, pathLength ) );
        }
    }

 // remove overlapping path segments

    std::set< unsigned int > occupiedNodes;
    double lastLength = std::numeric_limits< double >::infinity();
    while( !pathQueue.empty() )
    {
        CARNA_ASSERT( pathQueue.top().length <= lastLength ); // asserts the paths are sorted properly
        lastLength = pathQueue.top().length;

        LeafFinder::Path& path = pathQueue.top().path();
        pathQueue.pop();
        for( auto nodes_itr = path.rbegin(); nodes_itr != path.rend(); ++nodes_itr )
        {
            const LeafFinder::Node& node = *nodes_itr;
            const unsigned int nodeIndex = graph.computeNodeIndex( node );
            if( !occupiedNodes.insert( nodeIndex ).second )
            {
                const unsigned int overlapping_nodes = path.size() - ( nodes_itr - path.rbegin() );
                if( overlapping_nodes > 1 )
                {
                    path.erase( path.begin(), path.begin() + overlapping_nodes - 1 );
                }
                break;
            }
        }
    }

 // extract significant paths

    occupiedNodes.clear();
    significantPaths.clear();
    for( auto path_ptr_itr = paths.rbegin(); path_ptr_itr != paths.rend(); ++path_ptr_itr )
    {
        const LeafFinder::Path& path = **path_ptr_itr;

        if( path.size() < 2 )
        {
            continue;
        }

        const double averageRadius = computeAverageRadiusAlongPath( path );
        const double pathLength = computePathLength( path );

        longestPathLength = std::max( longestPathLength, pathLength );

        if( pathLength / averageRadius > minimumLengthToRadiusRatio() )
        {
            std::vector< unsigned int > compressed_path( path.size() );
            for( unsigned int i = 0; i < compressed_path.size(); ++i )
            {
                compressed_path[ i ] = compressNode( path[ i ] );
            }
            significantPaths.push_back( compressed_path );
        }
    }

 // delete paths

    std::for_each( paths.begin(), paths.end(), std::default_delete< LeafFinder::Path >() );
}


void Gulsun::createCenterlines()
{
    deleteCenterlines();
    for( auto path_itr = significantPaths.begin(); path_itr != significantPaths.end(); ++path_itr )
    {
        createCenterline( *path_itr );
    }
}


const GulsunRadiusStore& Gulsun::edgeRadiuses() const
{
    return radiuses;
}


double Gulsun::computeAverageRadiusAlongPath( const std::vector< MedialnessGraph::Node >& path ) const
{
    CARNA_ASSERT( path.size() >= 2 );

    double mean = 0;
    const unsigned int edges = path.size() - 1;
    walkPath( path, [&]( const MedialnessGraph::Node& from, const MedialnessGraph::Node& to )
        {
            const double radius = radiuses.radius( from, to );
            mean += radius / edges;
        }
    );

    return mean;
}


double Gulsun::computePathLength( const std::vector< MedialnessGraph::Node >& path ) const
{
    CARNA_ASSERT( path.size() >= 2 );

    double length = 0;
    walkPath( path, [&]( const MedialnessGraph::Node& from, const MedialnessGraph::Node& to )
        {
            const Carna::base::Vector edgeVector
                    = graph.getNodePosition( from ).toMillimeters()
                    - graph.getNodePosition(   to ).toMillimeters();

            const double edgeLength = edgeVector.norm();
            length += edgeLength;
        }
    );

    return length;
    return 0;
}


void Gulsun::deleteCenterlines()
{
    std::for_each( centerlines.begin(), centerlines.end(), std::default_delete< Carna::base::view::Polyline >() );
    centerlines.clear();
}


void Gulsun::createCenterline( const std::vector< unsigned int >& path )
{
    CARNA_ASSERT( path.size() >= 2 );

    centerlines.push_back( new Carna::base::view::Polyline( graph.model, Carna::base::view::Polyline::lineStrip ) );

    for( auto itr = path.begin(); itr != path.end(); ++itr )
    {
        MedialnessGraph::Node node;
        decompressNode( node, *itr );
        const Carna::base::model::Position position = graph.getNodePosition( node );
        ( *centerlines.back() ) << position;
    }
}


void Gulsun::reportRadius( const MedialnessGraph::Node& n1, const MedialnessGraph::Node& n2, double radius )
{
    radiuses.putRadius( n1, n2, radius );
}


void Gulsun::walkPath
    ( const std::vector< MedialnessGraph::Node >& path
    , const std::function< void
        ( const MedialnessGraph::Node& from
        , const MedialnessGraph::Node& to ) >& visit ) const
{
    CARNA_ASSERT( path.size() >= 2 );

    MedialnessGraph::Node from = *++path.rbegin();
    MedialnessGraph::Node to   =   *path.rbegin();
    visit( from, to );

    for( auto itr = path.rbegin() + 2; itr != path.rend(); ++itr )
    {
        to = from;
        from = *itr;
        visit( from, to );
    }
}


unsigned int Gulsun::countEnqueuedNodes() const
{
    return isClosed() ? 0 : dijkstra->countEnqueuedNodes();
}


unsigned int Gulsun::countExpandedNodes() const
{
    return isClosed() ? 0 : dijkstra->countExpandedNodes();
}


unsigned int Gulsun::countPaths() const
{
    return paths;
}


const std::vector< std::vector< unsigned int > >& Gulsun::vesselBranches() const
{
    return significantPaths;
}


double Gulsun::getLongestPathLength() const
{
    return longestPathLength;
}


void Gulsun::setMinimumLengthToRadiusRatio( double newMinimumLengthToRadiusRatio )
{
    this->currentMinimumLengthToRadiusRatio = newMinimumLengthToRadiusRatio;
}


double Gulsun::minimumLengthToRadiusRatio() const
{
    return currentMinimumLengthToRadiusRatio;
}


void Gulsun::showCenterlines( bool visible )
{
    for( auto centerline_itr = centerlines.begin(); centerline_itr != centerlines.end(); ++centerline_itr )
    {
        Carna::base::view::Polyline& centerline = **centerline_itr;
        centerline.setVisible( visible );
    }
}


void Gulsun::hideCenterlines()
{
    showCenterlines( false );
}


void Gulsun::saveTo( QDataStream& out ) const
{
    CARNA_ASSERT( !isClosed() );

    out << minimumLengthToRadiusRatio();

    struct DijkstraSerializer
    {
        DijkstraSerializer( QDataStream& out ) : out( out )
        {
        }

        QDataStream& out;

        void createList( unsigned int size )
        {
            out << size;
        }

        void serializeNodeHash( Dijkstra::NodeHash hash )
        {
            out << hash;
        }

        void serializeNode( const Dijkstra::Node& node )
        {
            out << node.x << node.y << node.z;
        }

        void serializeDistance( double distance )
        {
            out << distance;
        }
    };

    DijkstraSerializer dijkstraSerializer( out );

    dijkstra->saveTo< DijkstraSerializer >( dijkstraSerializer );
    radiuses.saveTo( out );
}


void Gulsun::loadFrom( QDataStream& in )
{
    double newMinimumLengthToRadiusRatio;
    in >> newMinimumLengthToRadiusRatio;
    setMinimumLengthToRadiusRatio( newMinimumLengthToRadiusRatio );

    struct DijkstraDeserializer
    {
        DijkstraDeserializer( QDataStream& in ) : in( in )
        {
        }

        QDataStream& in;

        unsigned int readListLength()
        {
            unsigned int listLength;
            in >> listLength;
            return listLength;
        }

        Dijkstra::NodeHash deserializeNodeHash()
        {
            Dijkstra::NodeHash hash;
            in >> hash;
            return hash;
        }

        void deserializeNode( Dijkstra::Node& node )
        {
            in >> node.x >> node.y >> node.z;
        }

        double deserializeDistance()
        {
            double distance;
            in >> distance;
            return distance;
        }
    };

    DijkstraDeserializer dijkstraDeserializer( in );

    if( isClosed() )
    {
        dijkstra.reset( new Dijkstra( graph, Dijkstra::Node() ) );
    }

    dijkstra->loadFrom< DijkstraDeserializer >( dijkstraDeserializer );
    radiuses.loadFrom( in );

    fetchCenterlines();
}


unsigned int Gulsun::compressNode( const MedialnessGraph::Node& node ) const
{
    return graph.computeNodeIndex( node );
}


void Gulsun::decompressNode( MedialnessGraph::Node& node, const unsigned int hash ) const
{
    graph.fetchNodeByIndex( node, hash );
}
