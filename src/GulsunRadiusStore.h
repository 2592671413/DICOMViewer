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



// ----------------------------------------------------------------------------------
// GulsunRadiusStore
// ----------------------------------------------------------------------------------

class GulsunRadiusStore
{

public:

 // ----------------------------------------------------------------------------------

    GulsunRadiusStore( const MedialnessGraph& );

    const MedialnessGraph& graph;

 // ----------------------------------------------------------------------------------

    void putRadius( const MedialnessGraph::Node& from, const MedialnessGraph::Node& to, double radius );

    double radius( const MedialnessGraph::Node& from, const MedialnessGraph::Node& to ) const;

    void removeRadiuses( std::function< bool( unsigned int from, unsigned int to ) > );

 // ----------------------------------------------------------------------------------

    void saveTo( QDataStream& ) const;

    void loadFrom( QDataStream& );

 // ----------------------------------------------------------------------------------

private:

    typedef std::pair< unsigned int, unsigned int > NodePair;

    struct less
    {
        bool operator()( const NodePair& e1, const NodePair& e2 ) const;
    };

    typedef std::map< NodePair, double, less > RadiusesMap;

    mutable std::unique_ptr< RadiusesMap > radiuses;

    struct Edge
    {
        
        unsigned int from;
        unsigned int to;
        double radius;

        Edge( unsigned int from, unsigned int to, double radius );

    };  // Edge

    std::vector< Edge > edges;

    void updateRadiuses() const;

}; // GulsunRadiusStore
