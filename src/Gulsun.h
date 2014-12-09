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

#include "MedialnessGraph.h"
#include "Dijkstra.h"
#include "GulsunRadiusStore.h"
#include <Carna/base/view/Polyline.h>
#include <QObject>

class QDataStream;



// ----------------------------------------------------------------------------------
// Gulsun
// ----------------------------------------------------------------------------------

class Gulsun : public QObject
{

    Q_OBJECT

public:

 // ----------------------------------------------------------------------------------

    Gulsun( MedialnessGraph& graph, const MedialnessGraph::Node& root, QObject* parent = nullptr );

    ~Gulsun();

    MedialnessGraph& graph;

 // ----------------------------------------------------------------------------------

    void setMinimumLengthToRadiusRatio( double );

    double minimumLengthToRadiusRatio() const;

 // ----------------------------------------------------------------------------------

    void reportRadius( const MedialnessGraph::Node& n1, const MedialnessGraph::Node& n2, double radius );

 // ----------------------------------------------------------------------------------

    unsigned int countEnqueuedNodes() const;

    unsigned int countExpandedNodes() const;

    unsigned int countPaths() const;

    double getLongestPathLength() const;

    const std::vector< std::vector< unsigned int > >& vesselBranches() const;

    const GulsunRadiusStore& edgeRadiuses() const;

    bool isClosed() const;

    unsigned int compressNode( const MedialnessGraph::Node& node ) const;

    void decompressNode( MedialnessGraph::Node& node, unsigned int hash ) const;

 // ----------------------------------------------------------------------------------

    void saveTo( QDataStream& ) const;

    void loadFrom( QDataStream& );

 // ----------------------------------------------------------------------------------

public slots:

    bool doNext();

    void doUpTo( unsigned int );

    void doAll();

    void updatePaths();

    void createCenterlines();

    void cancel();

    void showCenterlines( bool = true );

    void hideCenterlines();

    void close();

 // ----------------------------------------------------------------------------------

signals:

    void nodesExpanded( unsigned int );

    void finished();

 // ----------------------------------------------------------------------------------

private:

    bool canceled;

    struct MedialnessGraphSupport
    {

        typedef MedialnessGraph Graph;
        typedef Graph::Node Node;
        typedef Graph::nodeLess less;
        typedef Graph::OrderedEdges OrderedEdges;
        typedef unsigned int NodeHash;

        static NodeHash hash( const Graph& graph, const Node& node )
        {
            return graph.computeNodeIndex( node );
        }

    };  // MedialnessGraphSupport

    struct DijkstraTreeAdapter;

    typedef Dijkstra< MedialnessGraphSupport > Dijkstra;
    std::unique_ptr< Dijkstra > dijkstra;

    GulsunRadiusStore radiuses;

    std::vector< Carna::base::view::Polyline* > centerlines;
    double currentMinimumLengthToRadiusRatio;

    std::vector< std::vector< MedialnessGraphSupport::NodeHash > > significantPaths;

 // ----------------------------------------------------------------------------------

    unsigned int paths;

    double longestPathLength;

 // ----------------------------------------------------------------------------------

    void fetchCenterlines();

    double computeAverageRadiusAlongPath( const std::vector< MedialnessGraph::Node >& ) const;

    double computePathLength( const std::vector< MedialnessGraph::Node >& ) const;

    void createCenterline( const std::vector< unsigned int >& );

    void deleteCenterlines();

    void walkPath
        ( const std::vector< MedialnessGraph::Node >&
        , const std::function< void
            ( const MedialnessGraph::Node& from
            , const MedialnessGraph::Node& to ) >& ) const;

}; // Gulsun
