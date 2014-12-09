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

#include "SuccessiveMedialness.h"
#include <Carna/Polyline.h>



// ----------------------------------------------------------------------------------
// SuccessiveMedialness
// ----------------------------------------------------------------------------------

SuccessiveMedialness::SuccessiveMedialness( MedialnessGraph& graph, const MedialnessGraph::Node& root )
    : graph( graph )
    , node( root )
    , centerline( graph.model, Carna::Polyline::lineStrip )
{
    visitedNodeIndices.insert( graph.computeNodeIndex( root ) );

    qDebug() << "Starting at Node: (" << root.x << "," << root.y << "," << root.z << ")";

    centerline << graph.getNodePosition( root );
}


SuccessiveMedialness::~SuccessiveMedialness()
{
}


double SuccessiveMedialness::doNext()
{
    const Carna::Tools::Vector3ui& size = graph.model.volume().size;

    MedialnessGraph::OrderedEdges edges;
    graph.expand( node, edges );
    
    for( auto edge_it = edges.begin(); edge_it != edges.end(); ++edge_it )
    {
        const unsigned int nodeIndex = graph.computeNodeIndex( edge_it->second );
        if( visitedNodeIndices.find( nodeIndex ) == visitedNodeIndices.end() )
        {
            visitedNodeIndices.insert( nodeIndex );
            node = edge_it->second;

            const Carna::Position nodePosition = graph.getNodePosition( node );
            const double medialness = 1. / edge_it->first;

            qDebug() << "Moved to Node: (" << this->node.x << "," << this->node.y << "," << this->node.z << ") - Medialness:" << medialness;

            centerline << nodePosition;

            return medialness;
        }
    }

    qDebug() << "All nodes skipped.";

    return 0;
}
