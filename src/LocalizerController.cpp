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

#include "LocalizerController.h"
#include "LocalizerProvider.h"
#include "ToolSetEditor.h"
#include <QDoubleSpinBox>
#include <QIcon>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QToolBar>
#include <QTimer>
#include <QFileDialog>
#include <QApplication>
#include <QMessageBox>



// ----------------------------------------------------------------------------------
// Types & Globals
// ----------------------------------------------------------------------------------

const static QString connectLocalizerLabel    = "&Connect";
const static QString disconnectLocalizerLabel = "&Disconnect";



// ----------------------------------------------------------------------------------
// LocalizerController
// ----------------------------------------------------------------------------------

LocalizerController::LocalizerController( LocalizerComponent& component, Record::Server& server )
    : server( server )
    , component( component )
    , sbCacheIntegrityTimeout( new QDoubleSpinBox() )
    , toolSetEditor( 0 )
    , connectionToggling( new QAction( QIcon( ":/icons/connect.png" ), connectLocalizerLabel, this ) )
    , addingTool        ( new QAction( QIcon( ":/icons/add.png" )    , "&Add Tool"          , this ) )
    , saving            ( new QAction( QIcon( ":/icons/save.png" )   , "&Save Setup"        , this ) )
    , loading           ( new QAction( QIcon( ":/icons/load.png" )   , "L&oad Setup"        , this ) )
    , pausing           ( new QAction( QIcon( ":/icons/pause.png" )  , "&Pause"             , this ) )
{
    addingTool->setEnabled( false );
    saving    ->setEnabled( false );
    loading   ->setEnabled( false );
    pausing   ->setEnabled( false );

    pausing->setCheckable( true );

    QVBoxLayout* const root = new QVBoxLayout();
    QHBoxLayout* const header = new QHBoxLayout();
    QFormLayout* const cache = new QFormLayout();

    connect( connectionToggling, SIGNAL( triggered() ), this, SLOT( toggleConnection() ) );

    QToolBar* toolBar = new QToolBar();
    toolBar->setIconSize( QSize( 24, 24 ) );
    toolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    header->addWidget( toolBar );

    toolBar->addAction( connectionToggling );

    toolBar->addAction( addingTool );
    toolBar->addAction( loading );
    toolBar->addAction( saving );
    toolBar->addAction( pausing );

    sbCacheIntegrityTimeout->setSingleStep( 0.05 );
    sbCacheIntegrityTimeout->setMaximum( 10. );
    sbCacheIntegrityTimeout->setMinimum( sbCacheIntegrityTimeout->singleStep() );
    sbCacheIntegrityTimeout->setSuffix( " s" );
    sbCacheIntegrityTimeout->setEnabled( false );
    connect( sbCacheIntegrityTimeout, SIGNAL( valueChanged( double ) ), this, SLOT( setCacheIntegrityTimeout( double ) ) );
    cache->addRow( "Query Interval:", sbCacheIntegrityTimeout );

    QWidget* cacheWidget = new QWidget();
    cacheWidget->setLayout( cache );
    toolBar->addSeparator();
    toolBar->addWidget( cacheWidget );

    root->addLayout( header );
    root->addStretch();
    this->setLayout( root );
}


LocalizerController::~LocalizerController()
{
    shutDown();
}


void LocalizerController::toggleConnection()
{
    if( !viewUpdateTimer.get() )
    {
        loadFromIni();
    }
    else
    {
        shutDown();
    }
}


void LocalizerController::load()
{
    if( toolSetEditor )
    {
        toolSetEditor->load();
    }

    if( sbCacheIntegrityTimeout )
    {
        sbCacheIntegrityTimeout->setValue( localizer->getLazynessThreshold() / 1000. );
    }
}


void LocalizerController::requestState( bool ready )
{
    if( ready )
    {

        connectionToggling->setText( disconnectLocalizerLabel );

        sbCacheIntegrityTimeout->setValue( localizer->getLazynessThreshold() / 1000. );
        sbCacheIntegrityTimeout->setEnabled( true );

        this->layout()->removeItem( this->layout()->itemAt( this->layout()->count() - 1 ) );

        toolSetEditor = new ToolSetEditor( component, server, *localizer );
        this->layout()->addWidget( toolSetEditor );

        addingTool->setEnabled( true );
        connect( addingTool, SIGNAL( triggered() ), toolSetEditor, SLOT( addTool() ) );

        saving->setEnabled( true );
        connect( saving, SIGNAL( triggered() ), toolSetEditor, SLOT( save() ) );

        loading->setEnabled( true );
        connect( loading, SIGNAL( triggered() ), this, SLOT( load() ) );

        pausing->setEnabled( true );
        connect( pausing, SIGNAL( toggled( bool ) ), toolSetEditor, SLOT( setPaused( bool ) ) );

        viewUpdateTimer.reset( new QTimer() );
        viewUpdateTimer->setInterval( localizer->getLazynessThreshold() );
        connect( viewUpdateTimer.get(), SIGNAL( timeout() ), toolSetEditor, SLOT( updateViews() ) );
        viewUpdateTimer->start();

    }
    else
    {

        connectionToggling->setText( connectLocalizerLabel );

        sbCacheIntegrityTimeout->setEnabled( false );

        if( toolSetEditor )
        {
            toolSetEditor->hide();
            this->layout()->removeWidget( toolSetEditor );
            delete toolSetEditor;
            toolSetEditor = 0;

            static_cast< QVBoxLayout* >( this->layout() )->addStretch();
        }

        addingTool->setEnabled( false );
        saving->setEnabled( false );
        loading->setEnabled( false );
        pausing->setEnabled( false );
        pausing->setChecked( false );

        if( viewUpdateTimer.get() )
        {
            viewUpdateTimer->stop();
            viewUpdateTimer.reset();
        }

    }
}


void LocalizerController::shutDown()
{
    requestState( false );

    localizer.reset();
}


void LocalizerController::loadFromIni()
{
    const QString errorDetailsFileName = "DetectedErrors.ini";
    QString errorDetails = "";

    const QString iniFile
        = QFileDialog::getOpenFileName( this
                                      , "Load from INI"
                                      , ""
                                      , "INI files (*.ini)"
                                      , NULL
                                      , QFileDialog::ReadOnly
                                      | QFileDialog::HideNameFilterDetails );

    if( iniFile.isEmpty() )
    {
        return;
    }

    try
    {
        try
        {
            if( QFile::exists( errorDetailsFileName ) )
            {
                QFile::remove( errorDetailsFileName );
            }

            // reset UI

            requestState( false );

            QApplication::processEvents();

            // update registration provider

            if( localizer.get() )
            {
                localizer->loadFromIni( iniFile.toStdString() );
            }
            else
            {
                localizer.reset( new LocalizerProvider( server, iniFile.toStdString() ) );
                localizer->setLazynessThreshold( 250 );
            }

            // update UI

            requestState( true );
        }
        catch( const CRA::DefaultLocalizer::illegalIniFile& )
        {
            QFile detailsFile( errorDetailsFileName );
            if( detailsFile.exists() )
            {
                detailsFile.open( QIODevice::ReadOnly | QIODevice::Text );
                errorDetails = detailsFile.readAll();
                detailsFile.close();
            }

            throw std::runtime_error( "Selected INI file has unexpected format or COM port is not ready." );
        }
        catch( const CRA::DefaultLocalizer::missingIniFile& )
        {
            throw std::runtime_error( "Selected INI file does not exist." );
        }
    }
    catch( const std::exception& ex )
    {
        std::stringstream ss;
        ss << "An error has occurred:\n\n" << ex.what();

        QMessageBox msgBox( this );
        msgBox.setWindowTitle( "Localizer" );
        msgBox.setText( QString::fromStdString( ss.str() ) );
        msgBox.setIcon( QMessageBox::Critical );
        if( !errorDetails.isEmpty() )
        {
            msgBox.setDetailedText( errorDetails );
        }
        msgBox.exec();
    }
}


void LocalizerController::setCacheIntegrityTimeout( double seconds )
{
    if( localizer.get() )
    {
        localizer->setLazynessThreshold( static_cast< unsigned int >( seconds * 1000 + 0.5 ) );

        if( viewUpdateTimer.get() )
        {
            viewUpdateTimer->setInterval( localizer->getLazynessThreshold() );
        }
    }
}
