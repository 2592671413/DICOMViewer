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

/** \file   GulsunComponent.h
  * \brief  Defines GulsunComponent
  */

#include "Server.h"
#include "Component.h"
#include "SuccessiveMedialness.h"
#include "Gulsun.h"
#include "GulsunSegmentation.h"
#include <QWidget>

class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;



// ----------------------------------------------------------------------------------
// GulsunComponent
// ----------------------------------------------------------------------------------

class GulsunComponent : public Component
{

public:

    GulsunComponent( Record::Server& server, ComponentWindowFactory& );

    virtual ~GulsunComponent();

}; // GulsunComponent



// ----------------------------------------------------------------------------------
// GulsunController
// ----------------------------------------------------------------------------------

class GulsunController : public QWidget
{

    Q_OBJECT

public:

    GulsunController
        ( Record::Server& server
        , Carna::base::VisualizationEnvironment* acceleration = nullptr
        , QWidget* parent = nullptr );

    virtual ~GulsunController();

 // ----------------------------------------------------------------------------------

private:

    Record::Server& server;

    Carna::base::qt::Object3DChooser* const seedChooser;

    QLabel* const laSeedHUV;

    QComboBox* const cbEdgeEvaluation;

    QPushButton* const buResetGulsun;

    QPushButton* const buResetSuccessiveMedialness;

    QPushButton* const buSingleDijkstraStep;

    QPushButton* const buClose;

    QPushButton* const buLimitedDijkstraSteps;

    QPushButton* const buFinishDijkstra;

    QPushButton* const buNextMedialness;

    QPushButton* const buSegment;

    QPushButton* const buSave;

    QPushButton* const buLoad;

    QPushButton* const buFetch;

    QDoubleSpinBox* const sbMinimumLengthToRadiusRatio;

    QSpinBox* const sbMaxDijkstraSteps;

    QCheckBox* const cbHideCenterlines;

    QLabel* const laEnqueuedNodesCount;

    QLabel* const laExpandedNodesCount;

    QLabel* const laPathsCount;

    QLabel* const laVesselBranchesCount;

    QLabel* const laLongestPathLength;

    QDoubleSpinBox* const sbMinScale;

    QDoubleSpinBox* const sbMaxScale;

    QSpinBox* const sbScaleSamples;

    QSpinBox* const sbMinimumHUV;

    QSpinBox* const sbMaximumHUV;

    QDoubleSpinBox* const sbGamma;

    QDoubleSpinBox* const sbMinRadius;

    QDoubleSpinBox* const sbMaxRadius;

    QSpinBox* const sbRadiusSamples;

    QDoubleSpinBox* const sbMinimumContrast;

    QDoubleSpinBox* const sbMinimumMedialness;

    QCheckBox* const cbAllowMedialnessEarlyOut;

    QDoubleSpinBox* const sbIntensityTolerance;

    QSpinBox* const sbNodesPerInterval;

    QDoubleSpinBox* const sbRadiusMultiplier;

    QCheckBox* const cbSmoothedRadiuses;

    Carna::base::model::Object3D* selectedSeed;

    MedialnessGraph graph;

    std::unique_ptr< SuccessiveMedialness > successiveMedialness;

    std::unique_ptr< Gulsun > gulsun;

    std::unique_ptr< GulsunSegmentation > maskFactory;

 // ----------------------------------------------------------------------------------

    MedialnessGraph::Setup* getSetup() const;

    MedialnessGraph::Node fetchRoot() const;

    void initializeGulsun();

    void initializeGulsun( const MedialnessGraph::Node& root );

    void fetchResults();

    void updateGulsunStats();

 // ----------------------------------------------------------------------------------

signals:

    void doLimitedDijkstraSteps( unsigned int );

 // ----------------------------------------------------------------------------------

private slots:

    void resetGulsun();

    void resetSuccessiveMedialness();

    void doSingleDijkstraStep();

    void doLimitedDijkstraSteps();

    void close();

    void finishDijkstra();

    void computeNextMedialness();

    void setup( int = 0 );

    void segment();

    void setCenterlinesVisibility( bool hide );

    void save();

    void load();

    void fetch();

    void updateCurrentSeed();

    void updateSeedHUV();

    void releaseSelectedSeed();

}; // GulsunController
