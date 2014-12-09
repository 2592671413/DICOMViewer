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

#include "GulsunRadiusStore.h"
#include "MedialnessGraph.h"
#include <Carna/CarnaException.h>



// ----------------------------------------------------------------------------------
// GulsunRadiusStore :: less
// ----------------------------------------------------------------------------------

bool GulsunRadiusStore::less::operator()( const NodePair& e1, const NodePair& e2 ) const
{
    if( e1.first != e2.first )
    {
        return e1.first < e2.first;
    }
    else
    {
        return e1.second < e2.second;
    }
}



// ----------------------------------------------------------------------------------
// GulsunRadiusStore :: Edge
// ----------------------------------------------------------------------------------

GulsunRadiusStore::Edge::Edge( unsigned int from, unsigned int to, double radius )
    : from( from )
    , to( to )
    , radius( radius )
{
}



// ----------------------------------------------------------------------------------
// GulsunRadiusStore
// ----------------------------------------------------------------------------------

GulsunRadiusStore::GulsunRadiusStore( const MedialnessGraph& graph )
    : graph( graph )
{
}


void GulsunRadiusStore::putRadius
    ( const MedialnessGraph::Node& from
    , const MedialnessGraph::Node& to
    , double radius )
{
    CARNA_ASSERT( radius > 0 );

    edges.push_back( Edge( graph.computeNodeIndex( from ), graph.computeNodeIndex( to ), radius ) );
    radiuses.reset();
}


void GulsunRadiusStore::removeRadiuses( std::function< bool( unsigned int, unsigned int ) > toBeRemoved )
{
    std::vector< Edge > left_edges;
    for( auto edge_itr = edges.begin(); edge_itr != edges.end(); ++edge_itr )
    {
        if( !toBeRemoved( edge_itr->from, edge_itr->to ) )
        {
            left_edges.push_back( *edge_itr );
        }
    }

    edges.clear();
    edges = left_edges;

    radiuses.reset();
}


double GulsunRadiusStore::radius( const MedialnessGraph::Node& fromNode, const MedialnessGraph::Node& toNode ) const
{
    if( radiuses.get() == nullptr )
    {
        updateRadiuses();
    }

    const unsigned int from = graph.computeNodeIndex( fromNode );
    const unsigned int to   = graph.computeNodeIndex(   toNode );

    auto itr = radiuses->find( NodePair( from, to ) );
    if( itr == radiuses->end() )
    {
        if( !graph.isBidirectional() || ( itr = radiuses->find( NodePair( to, from ) ) ) == radiuses->end() )
        {
            CARNA_FAIL( "Radius of edge is unknown." );
        }
    }

    const double radius = itr->second;
    CARNA_ASSERT( radius > 0 );
    return radius;
}


void GulsunRadiusStore::updateRadiuses() const
{
    radiuses.reset( new RadiusesMap() );

    for( auto edge_itr = edges.begin(); edge_itr != edges.end(); ++edge_itr )
    {
        CARNA_ASSERT( edge_itr->radius > 0 );

        ( *radiuses )[ NodePair( edge_itr->from, edge_itr->to ) ] = edge_itr->radius;
    }
}


void GulsunRadiusStore::saveTo( QDataStream& out ) const
{
    out << edges.size();
    std::pair< MedialnessGraph::Node, MedialnessGraph::Node > pair;
    for( auto edge_itr = edges.begin(); edge_itr != edges.end(); ++edge_itr )
    {
        graph.fetchNodeByIndex( pair.first, edge_itr->from );
        graph.fetchNodeByIndex( pair.second, edge_itr->to );

        out << pair.first.x << pair.first.y << pair.first.z;
        out << pair.second.x << pair.second.y << pair.second.z;
        out << edge_itr->radius;
    }
}


void GulsunRadiusStore::loadFrom( QDataStream& in )
{
    edges.clear();
    radiuses.reset();

    unsigned int edgesCount;
    in >> edgesCount;

    for( unsigned int i = 0; i < edgesCount; ++i )
    {
        MedialnessGraph::Node from, to;
        double radius;

        in >> from.x >> from.y >> from.z;
        in >> to.x >> to.y >> to.z;
        in >> radius;

        CARNA_ASSERT( radius > 0 );

        edges.push_back( Edge( graph.computeNodeIndex( from ), graph.computeNodeIndex( to ), radius ) );
    }
}
