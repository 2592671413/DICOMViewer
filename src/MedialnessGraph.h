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

#include "Medialness.h"
#include <Carna/Carna.h>
#include <Carna/base/Vector3.h>
#include <Carna/base/model/Position.h>
#include <map>

class QGLFramebufferObject;



// ----------------------------------------------------------------------------------
// IntensitySampler
// ----------------------------------------------------------------------------------

class IntensitySampler : public Differential::Sampler
{

    const Carna::base::model::Scene& model;

public:

    IntensitySampler( const Carna::base::model::Scene& );


    virtual double valueAt( const Carna::base::Vector& ) const override;

}; // IntensitySampler



// ----------------------------------------------------------------------------------
// GpuIntensitySampler
// ----------------------------------------------------------------------------------

class GpuIntensitySampler : public Carna::base::view::SceneProvider::ResourceClient, public Differential::Sampler
{

    std::unique_ptr< QGLFramebufferObject > fbo;

public:

    GpuIntensitySampler( Carna::base::VisualizationEnvironment&, Carna::base::view::SceneProvider& );

    virtual ~GpuIntensitySampler();


    virtual double valueAt( const Carna::base::Vector& ) const override;

}; // GpuIntensitySampler



// ----------------------------------------------------------------------------------
// MedialnessGraph
// ----------------------------------------------------------------------------------

class MedialnessGraph
{

public:

    Carna::base::model::Scene& model;

    struct Setup
    {

        enum EdgeEvaluator
        {
            byDestination = 0,
            gaussian = 1,
            trapeze = 2,
            simpson = 3
        };

        double minimumScale, maximumScale;
        unsigned int scaleSamples;

        int minimumHUV, maximumHUV;

        double gamma;

        double minimumRadius, maximumRadius;
        unsigned int radiusSamples;

        double minimumContrast;

        EdgeEvaluator edgeEvaluator;

        double minimumMedialness;

        bool allowMedialnessEarlyOut;

        Setup
            ( double minimumScale
            , double maximumScale
            , unsigned int scaleSamples
            , int minimumHUV
            , int maximumHUV
            , double gamma
            , double minimumRadius
            , double maximumRadius
            , unsigned int radiusSamples
            , double minimumContrast
            , EdgeEvaluator edgeEvaluator
            , double minimumMedialness
            , bool allowMedialnessEarlyOut )

            : minimumScale( minimumScale )
            , maximumScale( maximumScale )
            , scaleSamples( scaleSamples )
            , minimumHUV( minimumHUV )
            , maximumHUV( maximumHUV )
            , gamma( gamma )
            , minimumRadius( minimumRadius )
            , maximumRadius( maximumRadius )
            , radiusSamples( radiusSamples )
            , minimumContrast( minimumContrast )
            , edgeEvaluator( edgeEvaluator )
            , minimumMedialness( minimumMedialness )
            , allowMedialnessEarlyOut( allowMedialnessEarlyOut )
        {
        }

    }; // Setup

    typedef Carna::base::Vector3ui Node;
    typedef std::multimap< double, Node > OrderedEdges;

    typedef std::function< void( const Node&, const Node&, double ) > EdgeRadiusConsumer;

    struct nodeLess
    {
        bool operator()( const Node& n1, const Node& n2 ) const
        {
            if( n1.x == n2.x )
            {
                if( n1.y == n2.y )
                {
                    return n1.z < n2.z;
                }
                else
                {
                    return n1.y < n2.y;
                }
            }
            else
            {
                return n1.x < n2.x;
            }
        }
    };

    static const double millimetersPerNode;

 // ----------------------------------------------------------------------------------

    MedialnessGraph( Differential::Sampler*, Carna::base::model::Scene&, const Setup&, const EdgeRadiusConsumer& );

    ~MedialnessGraph();

    Differential::Sampler& sampler;

    void configure( const Setup& );

    const Setup& setup() const;

    bool isBidirectional() const;

 // ----------------------------------------------------------------------------------

    void expand( const Node& node, OrderedEdges& edges ) const;

    void expand
        ( const Node& node
        , OrderedEdges& edges
        , const std::function< bool( const Node& ) >& isReachable ) const;

    void setDetailedDebug( bool );

    bool hasDetailedDebug() const;

 // ----------------------------------------------------------------------------------

    Carna::base::model::Position getNodePosition( const Node& node ) const;

    unsigned int computeNodeIndex( const Node& node ) const;

    void fetchNodeByIndex( Node& node, const unsigned int index ) const;

    Node pickNode( const Carna::base::model::Position& ) const;

 // ----------------------------------------------------------------------------------

private:

    Setup currentSetup;

    Medialness medialnessFilter;

    const EdgeRadiusConsumer produceRadius;

    bool detailedDebug;

    const Carna::base::Vector3ui size;

 // ----------------------------------------------------------------------------------

    void computeEdge( const Carna::base::Vector& p0, const Carna::base::Vector& p1, double& medialness, double& radius ) const;

}; // MedialnessGraph
