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

#include "GulsunComponent.h"
#include "CarnaContextClient.h"
#include "FlowLayout.h"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QThread>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <Carna/base/qt/Object3DChooser.h>
#include <Carna/base/qt/CarnaProgressDialog.h>
#include <Carna/base/qt/ExpandableGroupBox.h>
#include <Carna/base/Composition.h>



// ----------------------------------------------------------------------------------
// GulsunComponent
// ----------------------------------------------------------------------------------

GulsunComponent::GulsunComponent( Record::Server& server, ComponentWindowFactory& factory )
    : Component( server, factory, "Gulsun Vessel Segmentation" )
{
    GulsunController* controller = new GulsunController( server );

    ComponentDockable& dockable = createDockable( controller
        , Qt::LeftDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable );
}


GulsunComponent::~GulsunComponent()
{
}



// ----------------------------------------------------------------------------------
// GulsunController
// ----------------------------------------------------------------------------------

GulsunController::GulsunController
    ( Record::Server& server
    , Carna::base::VisualizationEnvironment* acceleration
    , QWidget* parent )

    : QWidget( parent )
    , server( server )
    , seedChooser( new Carna::base::qt::Object3DChooser( CarnaContextClient( server ).model() ) )
    , laSeedHUV( new QLabel() )
    , cbEdgeEvaluation( new QComboBox() )
    , sbMinScale( new QDoubleSpinBox() )
    , sbMaxScale( new QDoubleSpinBox() )
    , sbScaleSamples( new QSpinBox() )
    , sbMinimumHUV( new QSpinBox() )
    , sbMaximumHUV( new QSpinBox() )
    , sbGamma( new QDoubleSpinBox() )
    , sbMinRadius( new QDoubleSpinBox() )
    , sbMaxRadius( new QDoubleSpinBox() )
    , sbRadiusSamples( new QSpinBox() )
    , sbMinimumContrast( new QDoubleSpinBox() )
    , sbMinimumMedialness( new QDoubleSpinBox() )
    , cbAllowMedialnessEarlyOut( new QCheckBox( "Allow early-out for medialness computation" ) )
    , buResetGulsun( new QPushButton( "Reset" ) )
    , buResetSuccessiveMedialness( new QPushButton( "Reset" ) )
    , buSingleDijkstraStep( new QPushButton( "Single Step" ) )
    , buLimitedDijkstraSteps( new QPushButton( "N Steps" ) )
    , buClose( new QPushButton( "Stop" ) )
    , buFinishDijkstra( new QPushButton( "Finish" ) )
    , buNextMedialness( new QPushButton( "Next Medialness" ) )
    , buSegment( new QPushButton( "Segment" ) )
    , buSave( new QPushButton( "Save" ) )
    , buLoad( new QPushButton( "Load" ) )
    , buFetch( new QPushButton( "Fetch" ) )
    , sbMinimumLengthToRadiusRatio( new QDoubleSpinBox() )
    , sbMaxDijkstraSteps( new QSpinBox() )
    , cbHideCenterlines( new QCheckBox( "Hide Centerlines" ) )
    , laEnqueuedNodesCount( new QLabel( "0" ) )
    , laExpandedNodesCount( new QLabel( "0" ) )
    , laPathsCount( new QLabel( "0" ) )
    , laVesselBranchesCount( new QLabel( "0" ) )
    , laLongestPathLength( new QLabel( "-" ) )
    , sbIntensityTolerance( new QDoubleSpinBox() )
    , sbNodesPerInterval( new QSpinBox() )
    , sbRadiusMultiplier( new QDoubleSpinBox() )
    , cbSmoothedRadiuses( new QCheckBox( "Smoothed Radiuses" ) )
    , selectedSeed( nullptr )
    , graph( acceleration == nullptr
                ? static_cast< Differential::Sampler* >( new IntensitySampler( CarnaContextClient( server ).model() ) )
                : static_cast< Differential::Sampler* >( new GpuIntensitySampler( *acceleration, CarnaContextClient( server ).scene() ) )
            , CarnaContextClient( server ).model()
            , MedialnessGraph::Setup
                ( 0.1       /* minimum scale */
                , 3.0       /* maximum scale */
                , 3         /* scale samples */
                , -1024     /* minimum HUV */
                , +3071     /* maximum HUV */
                , 1.        /* normalization gamma */
                , 0.1       /* minimum radius */
                , 3.0       /* maximum radius */
                , 15        /* radius samples */
                , 1.        /* minimum contrast */
                , MedialnessGraph::Setup::byDestination
                , 0.65      /* minimum medialness */
                , true      /* allow early-out on medialness computation */ ),
            [&]( const MedialnessGraph::Node& n1, const MedialnessGraph::Node& n2, double radius )
                {
                    if( gulsun.get() != nullptr )
                    {
                        gulsun->reportRadius( n1, n2, radius );
                    }
                }
            )
{
    QVBoxLayout* const global = new QVBoxLayout();
    this->setLayout( global );

    QFormLayout* const multiscale   = new QFormLayout();
    QFormLayout* const medialness   = new QFormLayout();
    QFormLayout* const graphControl = new QFormLayout();

    Carna::base::qt::ExpandableGroupBox* const multiscaleFrame   = new Carna::base::qt::ExpandableGroupBox( "Multiscale Processing", false );
    Carna::base::qt::ExpandableGroupBox* const medialnessFrame   = new Carna::base::qt::ExpandableGroupBox( "Medialness Filter", false );
    Carna::base::qt::ExpandableGroupBox* const graphControlFrame = new Carna::base::qt::ExpandableGroupBox( "Graph Control", true );

    multiscaleFrame  ->child()->setLayout(   multiscale );
    medialnessFrame  ->child()->setLayout(   medialness );
    graphControlFrame->child()->setLayout( graphControl );

    multiscaleFrame  ->child()->setContentsMargins( 0, 0, 0, 0 );
    medialnessFrame  ->child()->setContentsMargins( 0, 0, 0, 0 );
    graphControlFrame->child()->setContentsMargins( 0, 0, 0, 0 );

    global->addWidget(   multiscaleFrame );
    global->addWidget(   medialnessFrame );
    global->addWidget( graphControlFrame );
    global->addStretch( 1 );

    const auto setupMillimetersSpinBox = []( QDoubleSpinBox* sb )
    {
        sb->setMinimum( 0. );
        sb->setMaximum( std::numeric_limits< double >::max() );
        sb->setSuffix( " mm" );
        sb->setDecimals( 1 );
        sb->setSingleStep( 0.1 );
    };

    const auto setupHuvSpinBox = []( QSpinBox* sb )
    {
        sb->setMinimum( -1024 );
        sb->setMaximum( +3071 );
        sb->setSingleStep( 1 );
        sb->setSuffix( " HU" );
    };

    const auto setupSamplesSpinBox = []( QSpinBox* sb )
    {
        sb->setMinimum( 2 );
        sb->setMaximum( std::numeric_limits< int >::max() );
        sb->setSingleStep( 1 );
    };

 // multiscale processing

    setupHuvSpinBox( sbMinimumHUV );
    setupHuvSpinBox( sbMaximumHUV );

    sbGamma->setMinimum( -std::numeric_limits< double >::max() );
    sbGamma->setMaximum( +std::numeric_limits< double >::max() );
    sbGamma->setSingleStep( 1. );
    sbGamma->setDecimals( 2 );

    sbGamma       ->setValue( graph.setup().gamma );
    sbMinScale    ->setValue( graph.setup().minimumScale );
    sbMaxScale    ->setValue( graph.setup().maximumScale );
    sbScaleSamples->setValue( graph.setup().scaleSamples );
    sbMinimumHUV  ->setValue( graph.setup().minimumHUV );
    sbMaximumHUV  ->setValue( graph.setup().maximumHUV );

    setupMillimetersSpinBox( sbMinScale );
    setupMillimetersSpinBox( sbMaxScale );

    setupSamplesSpinBox( sbScaleSamples );

    multiscale->addRow( "Normalization Gamma:", sbGamma );
    multiscale->addRow( "Minimum Scale:", sbMinScale );
    multiscale->addRow( "Maximum Scale:", sbMaxScale );
    multiscale->addRow( "Scale Samples:", sbScaleSamples );
    multiscale->addRow( "Minimum Intensity:", sbMinimumHUV );
    multiscale->addRow( "Maximum Intensity:", sbMaximumHUV );

    connect( sbGamma              , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbMinScale           , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbMaxScale           , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbScaleSamples       , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbMinimumHUV         , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbMaximumHUV         , SIGNAL( editingFinished() ), this, SLOT( setup() ) );

 // medialness

    setupMillimetersSpinBox( sbMinRadius );
    setupMillimetersSpinBox( sbMaxRadius );

    setupSamplesSpinBox( sbRadiusSamples );

    sbMinimumContrast->setMinimum( 1. );
    sbMinimumContrast->setMaximum( std::numeric_limits< double >::max() );
    sbMinimumContrast->setDecimals( 1 );
    sbMinimumContrast->setSingleStep( 100 );

    sbMinRadius      ->setValue( graph.setup().minimumRadius );
    sbMaxRadius      ->setValue( graph.setup().maximumRadius );
    sbRadiusSamples  ->setValue( graph.setup().radiusSamples );
    sbMinimumContrast->setValue( graph.setup().minimumContrast );

    medialness->addRow(   "Minimum Radius:", sbMinRadius );
    medialness->addRow(   "Maximum Radius:", sbMaxRadius );
    medialness->addRow(   "Radius Samples:", sbRadiusSamples );
    medialness->addRow( "Minimum Contrast:", sbMinimumContrast );

    connect( sbMinRadius      , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbMaxRadius      , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbRadiusSamples  , SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( sbMinimumContrast, SIGNAL( editingFinished() ), this, SLOT( setup() ) );

 // graph control

    graphControl->addRow( "Seed:", seedChooser );
    /*
    graphControl->addRow( "Seed HUV:", laSeedHUV );
    */
    graphControl->addRow( "Edge Evaluation:", cbEdgeEvaluation );
    graphControl->addRow( "Minimum Medialness:", sbMinimumMedialness );
    graphControl->addRow( cbAllowMedialnessEarlyOut );

    connect( seedChooser, SIGNAL( selectionChanged() ), this, SLOT( updateCurrentSeed() ) );
    updateSeedHUV();

    QStringList edgeEvaluators;
    edgeEvaluators << "By Destination (1 sample)" << "Gaussian (1 sample)" << "Trapeze (2 samples)" << "Simpson (3 samples)";

    cbEdgeEvaluation->setInsertPolicy( QComboBox::NoInsert );
    cbEdgeEvaluation->addItems( edgeEvaluators );
    cbEdgeEvaluation->setCurrentIndex( graph.setup().edgeEvaluator );

    sbMinimumMedialness->setMinimum( 0. );
    sbMinimumMedialness->setMaximum( 1. );
    sbMinimumMedialness->setSingleStep( 0.05 );
    sbMinimumMedialness->setDecimals( 2 );
    sbMinimumMedialness->setValue( graph.setup().minimumMedialness );

    cbAllowMedialnessEarlyOut->setChecked( graph.setup().allowMedialnessEarlyOut );

    connect( cbEdgeEvaluation, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setup( int ) ) );
    connect( sbMinimumMedialness, SIGNAL( editingFinished() ), this, SLOT( setup() ) );
    connect( cbAllowMedialnessEarlyOut, SIGNAL( clicked() ), this, SLOT( setup() ) );

    QFormLayout* const dijkstra = new QFormLayout();
    FlowLayout* const dijkstraButtons = new FlowLayout();
    QHBoxLayout* const successiveMedialnessButtons = new QHBoxLayout();

    QGroupBox* const successiveMedialnessButtonsFrame = new QGroupBox( "Successive Medialness" );
    successiveMedialnessButtonsFrame->setLayout( successiveMedialnessButtons );

    sbMinimumLengthToRadiusRatio->setMinimum( 0. );
    sbMinimumLengthToRadiusRatio->setMaximum( std::numeric_limits< double >::max() );
    sbMinimumLengthToRadiusRatio->setSingleStep( 0.5 );
    sbMinimumLengthToRadiusRatio->setDecimals( 2 );
    sbMinimumLengthToRadiusRatio->setValue( 10. );

    connect( sbMinimumLengthToRadiusRatio, SIGNAL( editingFinished() ), this, SLOT( setup() ) );

    sbMaxDijkstraSteps->setMinimum( 1 );
    sbMaxDijkstraSteps->setMaximum( std::numeric_limits< int >::max() );
    sbMaxDijkstraSteps->setSingleStep( 500 );
    sbMaxDijkstraSteps->setValue( 50000 );
    sbMaxDijkstraSteps->setSuffix( " steps" );

    cbHideCenterlines->setChecked( false );

    connect( cbHideCenterlines, SIGNAL( clicked( bool ) ), this, SLOT( setCenterlinesVisibility( bool ) ) );

    QWidget* const dijkstraButtonsWidget = new QWidget();
    dijkstraButtonsWidget->setLayout( dijkstraButtons );
    dijkstraButtonsWidget->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
    dijkstraButtons->setContentsMargins( 0, 0, 0, 0 );

    sbIntensityTolerance->setMinimum( 0. );
    sbIntensityTolerance->setMaximum( std::numeric_limits< int >::max() );
    sbIntensityTolerance->setSingleStep( 0.1 );
    sbIntensityTolerance->setValue( 2. );
    sbIntensityTolerance->setDecimals( 2 );
    sbIntensityTolerance->setSuffix( " stand. dev." );

    sbNodesPerInterval->setMinimum( 2 );
    sbNodesPerInterval->setMaximum( std::numeric_limits< int >::max() );
    sbNodesPerInterval->setSingleStep( 1 );
    sbNodesPerInterval->setSuffix( " nodes" );
    sbNodesPerInterval->setValue( 20 );

    sbRadiusMultiplier->setMinimum( 0. );
    sbRadiusMultiplier->setMaximum( std::numeric_limits< int >::max() );
    sbRadiusMultiplier->setSingleStep( 0.1 );
    sbRadiusMultiplier->setValue( 0.5 );
    sbRadiusMultiplier->setDecimals( 2 );
    sbRadiusMultiplier->setSuffix( " times" );

    cbSmoothedRadiuses->setChecked( false );

    Carna::base::qt::ExpandableGroupBox* const segmentationFrame = new Carna::base::qt::ExpandableGroupBox( "Segmentation", false );
    QFormLayout* const segmentation = new QFormLayout();
    segmentationFrame->child()->setLayout( segmentation );
    segmentationFrame->child()->setContentsMargins( 0, 0, 0, 0 );

    segmentation->addRow( "Tolerance:", sbIntensityTolerance );
    segmentation->addRow( "Radius Multiplier:", sbRadiusMultiplier );
    segmentation->addRow( "Interval Size:", sbNodesPerInterval );
    segmentation->addRow( cbSmoothedRadiuses );

    QGroupBox* const dijkstraFrame = new QGroupBox( "Dijkstra / Segmentation" );
    dijkstraFrame->setLayout( dijkstra );
    dijkstra->addRow( "Minimum Length/Radius:", sbMinimumLengthToRadiusRatio );
    dijkstra->addRow( "Pause After:", sbMaxDijkstraSteps );
    dijkstra->addRow( cbHideCenterlines );
    dijkstra->addRow( dijkstraButtonsWidget );
    dijkstra->addRow( segmentationFrame );
    dijkstra->addRow( "Expanded Nodes:", laExpandedNodesCount );
    dijkstra->addRow( "Enqueued Nodes:", laEnqueuedNodesCount );
    dijkstra->addRow( "Paths:", laPathsCount );
    dijkstra->addRow( "Vessel Branches:", laVesselBranchesCount );
    dijkstra->addRow( "Longest Path:", laLongestPathLength );

    graphControl->addRow( dijkstraFrame );
    graphControl->addRow( successiveMedialnessButtonsFrame );

    buResetGulsun              ->setEnabled( false );
    buResetSuccessiveMedialness->setEnabled( false );
    buSegment                  ->setEnabled( false );
    buSave                     ->setEnabled( false );
    buClose                    ->setEnabled( false );

    dijkstraButtons->addWidget( buResetGulsun );
    dijkstraButtons->addWidget( buSingleDijkstraStep );
    dijkstraButtons->addWidget( buLimitedDijkstraSteps );
    dijkstraButtons->addWidget( buFinishDijkstra );
    dijkstraButtons->addWidget( buFetch );
    dijkstraButtons->addWidget( buClose );
    dijkstraButtons->addWidget( buSegment );
    dijkstraButtons->addWidget( buSave );
    dijkstraButtons->addWidget( buLoad );

    successiveMedialnessButtons->addWidget( buResetSuccessiveMedialness );
    successiveMedialnessButtons->addWidget( buNextMedialness );

    connect( buResetGulsun              , SIGNAL( clicked() ), this, SLOT(               resetGulsun() ) );
    connect( buResetSuccessiveMedialness, SIGNAL( clicked() ), this, SLOT( resetSuccessiveMedialness() ) );
    connect( buSingleDijkstraStep       , SIGNAL( clicked() ), this, SLOT(      doSingleDijkstraStep() ) );
    connect( buLimitedDijkstraSteps     , SIGNAL( clicked() ), this, SLOT(    doLimitedDijkstraSteps() ) );
    connect( buClose                    , SIGNAL( clicked() ), this, SLOT(                     close() ) );
    connect( buFinishDijkstra           , SIGNAL( clicked() ), this, SLOT(            finishDijkstra() ) );
    connect( buNextMedialness           , SIGNAL( clicked() ), this, SLOT(     computeNextMedialness() ) );
    connect( buSegment                  , SIGNAL( clicked() ), this, SLOT(                   segment() ) );
    connect( buSave                     , SIGNAL( clicked() ), this, SLOT(                      save() ) );
    connect( buLoad                     , SIGNAL( clicked() ), this, SLOT(                      load() ) );
    connect( buFetch                    , SIGNAL( clicked() ), this, SLOT(                     fetch() ) );
}


GulsunController::~GulsunController()
{
}


MedialnessGraph::Setup* GulsunController::getSetup() const
{
    typedef MedialnessGraph::Setup::EdgeEvaluator EdgeEvaluator;

    const double gamma = sbGamma->value();
    const double minScale = sbMinScale->value();
    const double maxScale = sbMaxScale->value();
    const int scaleSamples = sbScaleSamples->value();
    const int minimumHUV = sbMinimumHUV->value();
    const int maximumHUV = sbMaximumHUV->value();

    const double minRadius = sbMinRadius->value();
    const double maxRadius = sbMaxRadius->value();
    const int radiusSamples = sbRadiusSamples->value();
    const double minimumContrast = sbMinimumContrast->value();

    const EdgeEvaluator edgeEvaluator = static_cast< EdgeEvaluator >( cbEdgeEvaluation->currentIndex() );

    const double minimumMedialness = sbMinimumMedialness->value();
    const bool allowMedialnessEarlyOut = cbAllowMedialnessEarlyOut->isChecked();

    if( minRadius > maxRadius || Carna::base::Math::isEqual( minRadius, maxRadius ) ||
        minScale > maxScale   || Carna::base::Math::isEqual( minScale ,  maxScale ) )
    {
        return nullptr;
    }
    else
    {
        return new MedialnessGraph::Setup
            ( minScale
            , maxScale
            , scaleSamples
            , minimumHUV
            , maximumHUV
            , gamma
            , minRadius
            , maxRadius
            , radiusSamples
            , minimumContrast
            , edgeEvaluator
            , minimumMedialness
            , allowMedialnessEarlyOut );
    }
}


void GulsunController::setup( int )
{
    const std::unique_ptr< MedialnessGraph::Setup > setup( getSetup() );
    if( setup.get() == nullptr )
    {
        buSingleDijkstraStep->setEnabled( false );
        buFinishDijkstra    ->setEnabled( false );
        buNextMedialness    ->setEnabled( false );
    }
    else
    {
        buSingleDijkstraStep->setEnabled( true );
        buFinishDijkstra    ->setEnabled( true );
        buNextMedialness    ->setEnabled( true );

        graph.configure( *setup );

        if( gulsun.get() != nullptr )
        {
            gulsun->setMinimumLengthToRadiusRatio( sbMinimumLengthToRadiusRatio->value() );
        }
    }
}


void GulsunController::resetGulsun()
{
    gulsun.reset();

    buResetGulsun->setEnabled( false );
    buSegment    ->setEnabled( false );
    buSave       ->setEnabled( false );
    buClose      ->setEnabled( false );
    buFetch      ->setEnabled( false );

    buSingleDijkstraStep->setEnabled( true );
    buLimitedDijkstraSteps->setEnabled( true );
    buFinishDijkstra->setEnabled( true );

    laEnqueuedNodesCount->setText( "0" );
    laExpandedNodesCount->setText( "0" );
    laPathsCount->setText( "0" );
    laVesselBranchesCount->setText( "0" );
    laLongestPathLength->setText( "-" );
}


void GulsunController::resetSuccessiveMedialness()
{
    successiveMedialness.reset();

    buResetSuccessiveMedialness->setEnabled( false );
}


MedialnessGraph::Node GulsunController::fetchRoot() const
{
    CARNA_ASSERT( seedChooser->isObject3DSelected() );

    return graph.pickNode( seedChooser->selectedObject3D().position() );
}


void GulsunController::doSingleDijkstraStep()
{
    if( gulsun.get() == nullptr )
    {
        initializeGulsun();
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    buSegment->setEnabled( false );

    const bool detailedDebug = graph.hasDetailedDebug();
    graph.setDetailedDebug( true );

    gulsun->doNext();
    fetchResults();

    graph.setDetailedDebug( detailedDebug );

    buResetGulsun->setEnabled( true );

    QApplication::restoreOverrideCursor();
}


void GulsunController::close()
{
    if( gulsun.get() == nullptr )
    {
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    gulsun->close();

    buClose               ->setEnabled( false );
    buSingleDijkstraStep  ->setEnabled( false );
    buLimitedDijkstraSteps->setEnabled( false );
    buFinishDijkstra      ->setEnabled( false );
    buSave                ->setEnabled( false );

    updateGulsunStats();

    QApplication::restoreOverrideCursor();
}


void GulsunController::doLimitedDijkstraSteps()
{
    if( gulsun.get() == nullptr )
    {
        initializeGulsun();
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    buSegment->setEnabled( false );

    const bool detailedDebug = graph.hasDetailedDebug();
    graph.setDetailedDebug( false );

    const unsigned int steps = static_cast< unsigned int >(sbMaxDijkstraSteps->value() );

    Carna::base::qt::CarnaProgressDialog progress
        ( "Performing Dijkstra..."
        , "Abort"
        , gulsun->countExpandedNodes()
        , gulsun->countExpandedNodes() + steps + 1  //< +1 prevents dialog from dismissing after last update
        , this );                                   //  This is necessary due to several operations (updating
                                                    //  statistics in particular) being performed by Gulsun
    progress.setWindowModality( Qt::WindowModal );  //  class _after_ the last after, but before the 'finished'
    progress.setModal( true );                      //  signal is emitted.
    progress.setWindowTitle( "Gulsun Vessel Segmentation" );
    progress.setCancelButtonText( "Pause" );

    connect( gulsun.get(), SIGNAL( finished() ), &progress, SLOT( close() ) );
    connect( gulsun.get(), SIGNAL( nodesExpanded( unsigned int ) ), &progress, SLOT( setValue( unsigned int ) ) );
    connect( &progress, SIGNAL( canceled() ), gulsun.get(), SLOT( cancel() ) );

    emit doLimitedDijkstraSteps( steps );
    progress.exec();
    fetchResults();

    graph.setDetailedDebug( detailedDebug );

    QApplication::restoreOverrideCursor();
}


void GulsunController::finishDijkstra()
{
    if( gulsun.get() == nullptr )
    {
        initializeGulsun();
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    buSegment->setEnabled( false );

    const bool detailedDebug = graph.hasDetailedDebug();
    graph.setDetailedDebug( false );

    Carna::base::qt::CarnaProgressDialog progress
        ( "Performing Dijkstra..."
        , "Abort"
        , 0
        , 0
        , this );

    progress.setWindowModality( Qt::WindowModal );
    progress.setModal( true );
    progress.setWindowTitle( "Gulsun Vessel Segmentation" );
    progress.setCancelButtonText( "Pause" );

    connect( gulsun.get(), SIGNAL( finished() ), &progress, SLOT( close() ) );
    connect( &progress, SIGNAL( canceled() ), gulsun.get(), SLOT( cancel() ) );

    QTimer::singleShot( 0, gulsun.get(), SLOT( doAll() ) );
    progress.exec();
    fetchResults();

    graph.setDetailedDebug( detailedDebug );

    QApplication::restoreOverrideCursor();
}


void GulsunController::initializeGulsun()
{
    initializeGulsun( fetchRoot() );
}


void GulsunController::initializeGulsun( const MedialnessGraph::Node& root )
{
    CARNA_ASSERT( gulsun.get() == nullptr );

    gulsun.reset( new Gulsun( graph, root ) );
    gulsun->setMinimumLengthToRadiusRatio( sbMinimumLengthToRadiusRatio->value() );

    QThread* const gulsunThread = new QThread();
    gulsun->moveToThread( gulsunThread );

    connect( this, SIGNAL( doLimitedDijkstraSteps( unsigned int ) ), gulsun.get(), SLOT( doUpTo( unsigned int ) ) );
    connect( gulsun.get(), SIGNAL( destroyed() ), gulsunThread, SLOT( deleteLater() ) );
    gulsunThread->start();

    buSave->setEnabled( true );
}


void GulsunController::fetchResults()
{
    gulsun->createCenterlines();
    cbHideCenterlines->setChecked( false );

    updateGulsunStats();
}


void GulsunController::updateGulsunStats()
{
    laEnqueuedNodesCount->setText( QString::number( gulsun->countEnqueuedNodes() ) );
    laExpandedNodesCount->setText( QString::number( gulsun->countExpandedNodes() ) );
    laPathsCount->setText( QString::number( gulsun->countPaths() ) );
    laVesselBranchesCount->setText( QString::number( gulsun->vesselBranches().size() ) );
    laLongestPathLength->setText
        ( gulsun->getLongestPathLength() > -std::numeric_limits< double >::infinity()
        ? QString::number( gulsun->getLongestPathLength(), 'f', 2 ) + " mm"
        : "-" );

    buResetGulsun->setEnabled( true );
    buSegment->setEnabled( !gulsun->vesselBranches().empty() );
    buClose->setEnabled( !gulsun->isClosed() && buSegment->isEnabled() );
    buFetch->setEnabled( gulsun->countExpandedNodes() > 0 && !gulsun->isClosed() );
}


void GulsunController::computeNextMedialness()
{
    if( successiveMedialness.get() == nullptr )
    {
        const Carna::base::Vector3ui root = fetchRoot();
        successiveMedialness.reset( new SuccessiveMedialness( graph, root ) );
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    successiveMedialness->doNext();

    buResetSuccessiveMedialness->setEnabled( true );

    QApplication::restoreOverrideCursor();
}


void GulsunController::segment()
{
    CARNA_ASSERT( gulsun.get() != nullptr );

    const static unsigned int max_itervalls_count = 21;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    Carna::base::qt::CarnaProgressDialog progress
        ( "Segmenting..."
        , "Abort"
        , 0
        , 0
        , this );

    progress.setWindowModality( Qt::WindowModal );
    progress.setModal( true );
    progress.setWindowTitle( "Gulsun Vessel Segmentation" );
    progress.setAutoClose( false );

    graph.model.setVolumeMask( nullptr );

    maskFactory.reset( new GulsunSegmentation( *gulsun ) );

    QThread* const segmentationThread = new QThread();
    maskFactory->moveToThread( segmentationThread );

    connect( maskFactory.get(), SIGNAL( destroyed() ), segmentationThread, SLOT( deleteLater() ) );
    connect( maskFactory.get(), SIGNAL(  finished() ),          &progress, SLOT(       close() ) );
    connect( &progress        , SIGNAL(  canceled() ),  maskFactory.get(), SLOT(      cancel() ) );

    connect( maskFactory.get(), SIGNAL( progressChanged( int ) ), &progress, SLOT(   setValue( int ) ) );
    connect( maskFactory.get(), SIGNAL(  minimumChanged( int ) ), &progress, SLOT( setMinimum( int ) ) );
    connect( maskFactory.get(), SIGNAL(  maximumChanged( int ) ), &progress, SLOT( setMaximum( int ) ) );

    segmentationThread->start();

    maskFactory->setSmoothedRadiuses( cbSmoothedRadiuses->isChecked() );
    maskFactory->setIntensityTolerance( static_cast< float >( sbIntensityTolerance->value() ) );
    maskFactory->setNodesPerInterval( sbNodesPerInterval->value() );
    maskFactory->setRadiusMultiplier( sbRadiusMultiplier->value() );

    QTimer::singleShot( 0, maskFactory.get(), SLOT( compute() ) );

    progress.exec();

    if( maskFactory->hasMask() )
    {
        graph.model.setVolumeMask(
            new Carna::base::model::BufferedMaskAdapter(
                new Carna::base::Composition< Carna::base::model::BufferedMaskAdapter::BinaryMask >(
                    maskFactory->takeMask() ) ) );
    }

    QApplication::restoreOverrideCursor();
}


void GulsunController::setCenterlinesVisibility( bool hide )
{
    if( gulsun.get() != nullptr )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        gulsun->showCenterlines( !hide );

        QApplication::restoreOverrideCursor();
    }
}


void GulsunController::save()
{
    const QString filename
        = QFileDialog::getSaveFileName
        ( this
        , "Save Gulsun Vessel Segmentation State"
        , ""
        , "Gulsun State Files (*.gulsun)"
        , 0
        , QFileDialog::DontResolveSymlinks
        | QFileDialog::HideNameFilterDetails );

    if( filename.isEmpty() )
    {
        return;
    }

    QFile file( filename );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        QMessageBox::critical( this, "Gulsun Vessel Segmentation", "Failed opening file for writing." );
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );
    QDataStream out( &file );

    if( gulsun.get() != nullptr )
    {
        gulsun->saveTo( out );
    }

    file.close();
    QApplication::restoreOverrideCursor();
}


void GulsunController::load()
{
    const QString filename
        = QFileDialog::getOpenFileName
        ( this
        , "Save Gulsun Vessel Segmentation State"
        , ""
        , "Gulsun State Files (*.gulsun)"
        , 0
        , QFileDialog::ReadOnly
        | QFileDialog::HideNameFilterDetails );

    if( filename.isEmpty() )
    {
        return;
    }

    QFile file( filename );
    if( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::critical( this, "Gulsun Vessel Segmentation", "Failed opening file for reading." );
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );
    QDataStream in( &file );

    if( gulsun.get() == nullptr )
    {
        initializeGulsun( MedialnessGraph::Node() );
    }
    gulsun->loadFrom( in );
    this->sbMinimumLengthToRadiusRatio->setValue( gulsun->minimumLengthToRadiusRatio() );

    fetchResults();

    buSingleDijkstraStep  ->setEnabled( true );
    buLimitedDijkstraSteps->setEnabled( true );
    buFinishDijkstra      ->setEnabled( true );

    file.close();
    QApplication::restoreOverrideCursor();
}


void GulsunController::updateCurrentSeed()
{
    if( selectedSeed != nullptr )
    {
        disconnect( selectedSeed, SIGNAL( moved() ), this, SLOT( updateSeedHUV() ) );
        disconnect( selectedSeed, SIGNAL( destroyed() ), this, SLOT( releaseSelectedSeed() ) );
    }

    if( seedChooser->isObject3DSelected() )
    {
        selectedSeed = &seedChooser->selectedObject3D();

        connect( selectedSeed, SIGNAL( moved() ), this, SLOT( updateSeedHUV() ) );
        connect( selectedSeed, SIGNAL( destroyed() ), this, SLOT( releaseSelectedSeed() ) );
    }

    updateSeedHUV();
}


void GulsunController::updateSeedHUV()
{
    if( selectedSeed != nullptr )
    {
        const double huv = graph.sampler.valueAt( selectedSeed->position().toMillimeters() );
        laSeedHUV->setText( QString::number( huv, 'f', 2 ) );
    }
    else
    {
        laSeedHUV->setText( "-" );
    }
}


void GulsunController::releaseSelectedSeed()
{
    selectedSeed = nullptr;
}


void GulsunController::fetch()
{
    if( gulsun.get() )
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        gulsun->updatePaths();
        updateGulsunStats();

        QApplication::restoreOverrideCursor();
    }
}
