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
#include <Carna/base/view/Polyline.h>



// ----------------------------------------------------------------------------------
// SuccessiveMedialness
// ----------------------------------------------------------------------------------

class SuccessiveMedialness
{

public:


    SuccessiveMedialness( MedialnessGraph& graph, const MedialnessGraph::Node& root );

    ~SuccessiveMedialness();


    double doNext();


private:

    MedialnessGraph& graph;

    MedialnessGraph::Node node;

    Carna::base::view::Polyline centerline;

    std::set< unsigned int > visitedNodeIndices;

}; // SuccessiveMedialness
