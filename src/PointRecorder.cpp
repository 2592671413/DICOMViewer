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

#include "PointRecorder.h"
#include "PointCloud.h"
#include "ToolChooser.h"
#include "PointRecorderView.h"
#include "LocalizerClient.h"
#include "ComponentEmbeddable.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QTimer>



// ----------------------------------------------------------------------------------
// Types & Globals
// ----------------------------------------------------------------------------------

const static QString msgBoxTitle = "Point Recorder";



// ----------------------------------------------------------------------------------
// PointRecorder
// ----------------------------------------------------------------------------------

PointRecorder::PointRecorder( Record::Server& server, PointCloud& cloud, QWidget* parent )
    : QWidget( parent )
    , server( server )
    , cloud( cloud )
    , pointerSelector( new ToolChooser( server ) )
    , baseSelector( new ToolChooser( server ) )
    , view( new PointRecorderView( cloud ) )
    , shotting( new QAction ( QIcon( ":/icons/pencil.png" ), "Single Shot"         , this ) )
    , recording( new QAction( QIcon( ":/icons/brush.png"  ), "Continuous Recording", this ) )
    , accepting( new QAction( QIcon( ":/icons/save.png"   ), "Accept"              , this ) )
    , shotTimer( nullptr )
    , minimumPositionDelta( 5. )
{
    QVBoxLayout* global = new QVBoxLayout();
    QHBoxLayout* header = new QHBoxLayout();

    // header

    header->addWidget( new QLabel( "Pointer: " ) );
    header->addWidget( pointerSelector );

    connect( pointerSelector, SIGNAL(   selectionChanged() ), this, SLOT( updateEnability() ) );
    connect( pointerSelector, SIGNAL( selectionDissolved() ), this, SLOT( updateEnability() ) );

    header->addStretch();

    header->addWidget( new QLabel( "Reference: " ) );
    header->addWidget( baseSelector );

    connect( baseSelector, SIGNAL(   selectionChanged() ), this, SLOT( updateEnability() ) );
    connect( baseSelector, SIGNAL( selectionDissolved() ), this, SLOT( updateEnability() ) );

    global->addLayout( header );

    // central

    QFrame* viewFrame = new QFrame();
    viewFrame->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding );
    viewFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    viewFrame->setLineWidth( 1 );
    viewFrame->setLayout( new QHBoxLayout() );
    viewFrame->layout()->setContentsMargins( 1, 1, 1, 1 );
    viewFrame->layout()->addWidget( view );

    global->addWidget( viewFrame );

    // footer

    QToolBar* footer = new QToolBar();
    footer->setIconSize( QSize( 24, 24 ) );
    footer->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    global->addWidget( footer );

    footer->addAction( recording );
    recording->setEnabled( false );
    recording->setCheckable( true );
    connect( recording, SIGNAL( toggled( bool ) ), this, SLOT( toggleRecording( bool ) ) );

    footer->addAction( shotting );
    shotting->setEnabled( false );
    connect( shotting, SIGNAL( triggered() ), this, SLOT( recordSingleShot() ) );

    footer->addSeparator();
    
    footer->addAction( accepting );
    connect( accepting, SIGNAL( triggered() ), this, SLOT( accept() ) );

    // global

    this->setLayout( global );
}


PointRecorder::~PointRecorder()
{
    if( shotTimer.get() )
    {
        shotTimer->stop();
    }
    shotTimer.reset();
}


bool PointRecorder::isToolSelectionValid() const
{
    return
        pointerSelector->isToolSelected() &&
        baseSelector->isToolSelected() &&
        &( pointerSelector->getSelectedTool() ) != &( baseSelector->getSelectedTool() );
}


void PointRecorder::updateEnability()
{
    const bool ok = isToolSelectionValid();

    recording->setEnabled( ok );
    shotting ->setEnabled( ok && !recording->isChecked() );

 // check whether the shot timer has to be turned on

    if( ok && !shotTimer.get() )
    {
        shotTimer.reset( new QTimer() );

        connect( shotTimer.get(), SIGNAL( timeout() ), this, SLOT( shot() ) );
        shotTimer->setInterval( 1. / 60 );
        shotTimer->start();
    }

 // check whether the shot timer has to be stopped

    if( !ok && shotTimer.get() )
    {
        shotTimer->stop();
        shotTimer.reset();

        this->shot();
    }
}


void PointRecorder::shot()
{
    Carna::base::Transformation bearing;

    try
    {
        fetchRelativeBearing( bearing );

        view->setPointerBearing( bearing );
        view->setPointerVisibility( true );
        view->updateGL();
    }
    catch( const std::runtime_error& ex )
    {
        view->setPointerVisibility( false );
        view->updateGL();

        if( recording->isChecked() )
        {
            endRecording();

            std::stringstream ss;
            ss << "Recording ended: ";
            ss << ex.what();

            recording->setChecked( false );

            QMessageBox::warning( this, msgBoxTitle, QString::fromStdString( ss.str() ) );
        }

        return;
    }

    if( recording->isChecked() )
    {
        const Carna::base::Vector pos( bearing.a14(), bearing.a24(), bearing.a34() );

        PointCloud::PointList& data = cloud.getList();

        if( data.empty() || ( data.back() - pos ).norm() >= minimumPositionDelta )
        {
            data.push_back( pos );
        }
    }
}


void PointRecorder::fetchRelativeBearing( Carna::base::Transformation& bearing ) const
{
    // check localizer interface

    if( !server.hasService( Localizer::serviceID ) )
    {
        throw std::runtime_error( "The localizer service is unavailable." );
    }

    // check tool selection

    if( !isToolSelectionValid() )
    {
        throw std::runtime_error( "The tool selection is invalid." );
    }

    // compute

    CRA::Tool& pointer = pointerSelector->getSelectedTool();
    CRA::Tool& referenceBase = baseSelector->getSelectedTool();

    if( !pointer.isVisible() || !referenceBase.isVisible() )
    {
        throw std::runtime_error( "At least one tool is not visible." );
    }

    pointer.computeRelativeOrientation( referenceBase, bearing );
}


void PointRecorder::recordSingleShot()
{
    const QString msgTitle = "Point Recorder";

    // fetch tool state

    Carna::base::Transformation bearing;

    try
    {
        fetchRelativeBearing( bearing );
    }
    catch( const std::runtime_error& ex )
    {
        QMessageBox::critical( this, msgTitle, QString::fromStdString( ex.what() ) );

        return;
    }

    // save

    const Carna::base::Vector millimters( bearing.a14(), bearing.a24(), bearing.a34() );

    PointCloud::PointList& data = cloud.getList();

    data.push_back( millimters );

    // update view

    view->updateGL();
}


void PointRecorder::toggleRecording( bool toggled )
{
    if( toggled )
    {
        beginRecording();
    }
    else
    {
        endRecording();
    }
}


void PointRecorder::beginRecording()
{
    recording->setChecked( true );

    updateEnability();
}


void PointRecorder::endRecording()
{
    recording->setChecked( false );

    updateEnability();
}


void PointRecorder::accept()
{
    QObject* widget = this;

    while( dynamic_cast< ComponentEmbeddable* >( widget ) == 0 )
    {
        widget = widget->parent();
    }

    QTimer::singleShot( 0, widget, SLOT( close() ) );
}
