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

#include "Gulsun.h"
#include "ClippedVolumeMask.h"
#include <Carna/base/model/BufferedMaskAdapter.h>



// ----------------------------------------------------------------------------------
// GulsunSegmentation
// ----------------------------------------------------------------------------------

class GulsunSegmentation : public QObject
{

    Q_OBJECT

public:

 // ----------------------------------------------------------------------------------

    GulsunSegmentation( const Gulsun& );

    ~GulsunSegmentation();

    const Gulsun& gulsun;

 // ----------------------------------------------------------------------------------

    Carna::base::model::BufferedMaskAdapter::BinaryMask& getMask() const;

    Carna::base::model::BufferedMaskAdapter::BinaryMask* takeMask();

    double intensityTolerance() const;

    bool hasMask() const;

    int nodesPerInterval() const;

    double radiusMultiplier() const;

    bool smoothedRadiuses() const;

 // ----------------------------------------------------------------------------------

public slots:

    void setIntensityTolerance( double );

    void setNodesPerInterval( int );

    void setRadiusMultiplier( double );

    void setSmoothedRadiuses( bool );

    void compute();

    void cancel();

 // ----------------------------------------------------------------------------------

signals:

    void minimumChanged( int );

    void maximumChanged( int );

    void progressChanged( int );

    void finished();

 // ----------------------------------------------------------------------------------

private:

    bool canceled;

    double tolerance;

    int intervalSize;

    double radiusFactor;

    bool smoothedRadii;

    void segmentInterval
        ( const std::vector< MedialnessGraph::Node >& branch
        , unsigned int first
        , unsigned int last );

 // ----------------------------------------------------------------------------------

    struct Result : public Carna::base::model::ScalarField< Carna::base::model::BufferedMaskAdapter::BinaryMask::ValueType >
    {

        struct less
        {
            bool operator()( const Carna::base::Vector3ui&, const Carna::base::Vector3ui& ) const;
        };

        typedef std::set< Carna::base::Vector3ui, less > Container;

        Container container;
    
        virtual ValueType operator()
            ( unsigned int x
            , unsigned int y
            , unsigned int z ) const override;

    };  // Result

 // ----------------------------------------------------------------------------------

    std::unique_ptr< Result > result;

    std::unique_ptr< Carna::base::model::BufferedMaskAdapter::BinaryMask > mask;

}; // GulsunSegmentation
