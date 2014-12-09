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

#include "MainWindow.h"
#include "CarnaContextProvider.h"
#include "ComponentsProvider.h"
#include "PointCloudsProvider.h"
#include "ViewWindow.h"
#include "WindowingComponent.h"
#include "ModelInfo.h"
#include "LocalizerComponent.h"
#include "PointCloudsComponent.h"
#include "RegistrationComponent.h"
#include "Exporter.h"
#include "Importer.h"
#include "BinaryDumpProcessor.h"
#include "ObjectsComponent.h"
#include "VolumeNormalizer.h"
#include "CarnaModelFactory.h"
#include "MaskingDialog.h"
#include "GulsunComponent.h"
#include <Carna/base/model/SceneFactory.h>
#include <Carna/base/CarnaException.h>
#include <QTabWidget>
#include <QAction>
#include <QMenuBar>
#include <QProgressDialog>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QStackedWidget>



// ----------------------------------------------------------------------------------
// GlobalComponentWindowFactory
// ----------------------------------------------------------------------------------

GlobalComponentWindowFactory::GlobalComponentWindowFactory( MainWindow& window )
    : window( window )
{
}


ComponentEmbeddable* GlobalComponentWindowFactory::createEmbedable( QWidget* child, EmbeddablePlacer&, const QString& suffix, const QString& suffixConnector, MenuFactory& menuFactory )
{
    ComponentEmbeddable* const pEmbedable = new ComponentEmbeddable( child, suffix, suffixConnector, menuFactory );
    pEmbedable->show();
    return pEmbedable;
}


ComponentDockable* GlobalComponentWindowFactory::createDockable( QWidget* child, Qt::DockWidgetArea area, QDockWidget::DockWidgetFeatures features )
{
    ComponentDockable* const dockable = new ComponentDockable();
    dockable->setAllowedAreas( Qt::AllDockWidgetAreas );
    dockable->setFeatures( features );
    dockable->setWidget( child );
    window.addDockWidget( area, dockable );
    return dockable;
}



// ----------------------------------------------------------------------------------
// MainWindow
// ----------------------------------------------------------------------------------

MainWindow::MainWindow( QWidget* parent, Qt::WFlags flags )
    : QMainWindow( parent, flags )
   // Misc  -------------------------------------------------------------------------
    , content( new QStackedWidget() )
    , tabWidget( new QTabWidget() )
    , nextTabId( 1 )
    , carnaModelFactory( new CarnaModelFactory( server ) )
    , componentWindowFactory( *this )
   // File Menu  --------------------------------------------------------------------
    , exporting( new QAction( "&Export...", this ) )
    , normalizing( new QAction( "&Normalize...", this ) )
    , masking( new QAction( "&Mask Dataset...", this ) )
    , closing( new QAction( "&Close", this ) )
   // View Menu  --------------------------------------------------------------------
    , acquiringViewTab( new QAction( "New &Tab", this ) )
    , acquiringLocalizer( new QAction( "&Localizer", this ) )
    , acquiringModelInfo( new QAction( "&Properties", this ) )
    , acquiringObjectsManager( new QAction( "Objects...", this ) )
    , acquiringPointClouds( new QAction( "&Point Clouds...", this ) )
    , acquiringRegistration( new QAction( "&Registration...", this ) )
    , acquiringGulsun( new QAction( "&Gulsun Vessel Segmentation", this ) )
    , maskExporting( new QAction( "&Export Binary Mask...", this ) )
    , maskImporting( new QAction( "&Import Binary Mask...", this ) )
{
    this->setWindowTitle( "DICOM Viewer 3" );
    this->resize( 750, 750 );

    // -----------------------------------------------------------------

    QAction* exiting  = new QAction( "E&xit" , this );

    exiting->setShortcuts( QKeySequence::Quit );

    QMenu* fileMenu = menuBar()->addMenu( "&File" );
    fileMenu->addAction( acquiringModelInfo );
    fileMenu->addSeparator();
    fileMenu->addAction( normalizing );
    fileMenu->addAction( masking );
    fileMenu->addSeparator();
    fileMenu->addAction( maskImporting );
    fileMenu->addAction( maskExporting );
    fileMenu->addSeparator();
    fileMenu->addAction( exporting );
    fileMenu->addAction( closing );
    fileMenu->addSeparator();
    fileMenu->addAction( exiting );

    closing->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_W ) );
    exiting->setShortcut( QKeySequence( Qt::ALT + Qt::Key_F4 ) );

    closing->setEnabled( false );
    exporting->setEnabled( false );
    normalizing->setEnabled( false );
    masking->setEnabled( false );
    maskExporting->setEnabled( false );
    maskImporting->setEnabled( false );

    connect( exporting    , SIGNAL( triggered() ), this, SLOT( exportRecord() ) );
    connect( normalizing  , SIGNAL( triggered() ), this, SLOT(    normalize() ) );
    connect( masking      , SIGNAL( triggered() ), this, SLOT(         mask() ) );
    connect( closing      , SIGNAL( triggered() ), this, SLOT(  closeRecord() ) );
    connect( exiting      , SIGNAL( triggered() ), this, SLOT(         exit() ) );
    connect( maskExporting, SIGNAL( triggered() ), this, SLOT(   exportMask() ) );
    connect( maskImporting, SIGNAL( triggered() ), this, SLOT(   importMask() ) );

    // -----------------------------------------------------------------

    acquiringLocalizer->setCheckable( true );
    acquiringObjectsManager->setCheckable( true );
    acquiringPointClouds->setCheckable( true );
    acquiringRegistration->setCheckable( true );
    acquiringModelInfo->setCheckable( true );
    acquiringObjectsManager->setCheckable( true );
    acquiringGulsun->setCheckable( true );

    QMenu* menuView = menuBar()->addMenu( "&View" );
    menuView->addAction( acquiringViewTab );
    menuView->addSeparator();
    menuView->addAction( acquiringObjectsManager );
    menuView->addAction( acquiringPointClouds );
    menuView->addAction( acquiringGulsun );
    menuView->addSeparator();
    menuView->addAction( acquiringLocalizer );
    menuView->addAction( acquiringRegistration );

    acquiringViewTab       ->setEnabled( false );
    acquiringLocalizer     ->setEnabled( false );
    acquiringModelInfo     ->setEnabled( false );
    acquiringObjectsManager->setEnabled( false );
    acquiringPointClouds   ->setEnabled( false );
    acquiringRegistration  ->setEnabled( false );
    acquiringGulsun        ->setEnabled( false );

    acquiringViewTab->setShortcut( QKeySequence::AddTab );
    acquiringGulsun->setShortcut( Qt::Key_F8 );
    acquiringObjectsManager->setShortcut( Qt::Key_F9 );
    acquiringPointClouds->setShortcut( Qt::Key_F10 );
    acquiringLocalizer->setShortcut( Qt::Key_F11 );
    acquiringRegistration->setShortcut( Qt::Key_F12 );

    connect( acquiringViewTab       , SIGNAL( triggered() ), this, SLOT(           acquireViewTab() ) );
    connect( acquiringLocalizer     , SIGNAL( triggered() ), this, SLOT(         acquireLocalizer() ) );
    connect( acquiringModelInfo     , SIGNAL( triggered() ), this, SLOT(         acquireModelInfo() ) );
    connect( acquiringObjectsManager, SIGNAL( triggered() ), this, SLOT(    acquireObjectsManager() ) );
    connect( acquiringPointClouds   , SIGNAL( triggered() ), this, SLOT(       acquirePointClouds() ) );
    connect( acquiringRegistration  , SIGNAL( triggered() ), this, SLOT(      acquireRegistration() ) );
    connect( acquiringGulsun        , SIGNAL( triggered() ), this, SLOT(            acquireGulsun() ) );

    // -----------------------------------------------------------------

    content->addWidget( carnaModelFactory );

    connect( carnaModelFactory, SIGNAL( created( Carna::base::model::Scene* ) ), this, SLOT( init( Carna::base::model::Scene* ) ) );

    tabWidget->setDocumentMode( true );
    content->addWidget( tabWidget );
    this->setCentralWidget( content );

    connect( tabWidget, SIGNAL( tabCloseRequested( int ) ), this, SLOT( tabCloseRequested( int ) ) );
}


MainWindow::~MainWindow()
{
    if( carna.get() )
    {
        this->closeRecord();
    }
}


void MainWindow::init( Carna::base::model::Scene* model )
{
    CARNA_ASSERT( model );
    CARNA_ASSERT( !carna.get() );

    carna.reset( new CarnaContextProvider( server, model ) );
    components.reset( new ComponentsProvider( server ) );
    pointClouds.reset( new PointCloudsProvider( server ) );

    // -----------------------------------------------------------------

    normalizing->setEnabled( true );
    masking->setEnabled( true );
    exporting->setEnabled( true );
    closing->setEnabled( true );
    maskImporting->setEnabled( true );
    acquiringViewTab->setEnabled( true );
    acquiringLocalizer->setEnabled( true );
    acquiringModelInfo->setEnabled( true );
    acquiringObjectsManager->setEnabled( true );
    acquiringPointClouds->setEnabled( true );
    acquiringRegistration->setEnabled( true );
    acquiringGulsun->setEnabled( true );

    updateMaskExporting();
    connect( model, SIGNAL( maskExchanged() ), this, SLOT( updateMaskExporting() ) );

    tabWidget->setTabsClosable( false );
    content->setCurrentWidget( tabWidget );
    acquireViewTab();

    components->takeComponent( new WindowingComponent( server, componentWindowFactory ) );
}


void MainWindow::updateMaskExporting()
{
    maskExporting->setEnabled( carna->model().hasVolumeMask() );
}


void MainWindow::acquireLocalizer()
{
    acquiringLocalizer->setEnabled( false );
    acquiringLocalizer->setChecked( true );

    LocalizerComponent* localizer = new LocalizerComponent( server, componentWindowFactory );
    components->takeComponent( localizer );

    connect( localizer, SIGNAL( destroyed() ), this, SLOT( localizerReleased() ) );
}


void MainWindow::localizerReleased()
{
    acquiringLocalizer->setEnabled( true );
    acquiringLocalizer->setChecked( false );
}


void MainWindow::acquireModelInfo()
{
    acquiringModelInfo->setEnabled( false );
    acquiringModelInfo->setChecked( true );

    ModelInfo* const modelInfo = new ModelInfo( server, componentWindowFactory );
    components->takeComponent( modelInfo );

    connect( modelInfo, SIGNAL( destroyed() ), this, SLOT( modelInfoReleased() ) );
}


void MainWindow::acquireObjectsManager()
{
    acquiringObjectsManager->setEnabled( false );
    acquiringObjectsManager->setChecked( true );

    ObjectsComponent* const component = new ObjectsComponent( server, componentWindowFactory );
    components->takeComponent( component );

    connect( component, SIGNAL( destroyed() ), this, SLOT( objectsManagerReleased() ) );
}


void MainWindow::modelInfoReleased()
{
    acquiringModelInfo->setEnabled( true );
    acquiringModelInfo->setChecked( false );
}


void MainWindow::objectsManagerReleased()
{
    acquiringObjectsManager->setEnabled( true );
    acquiringObjectsManager->setChecked( false );
}


void MainWindow::closeRecord()
{
    QProgressDialog dlg( "Closing record...", "", 0, 0, this );
    dlg.setCancelButton( nullptr );
    dlg.show();
    QApplication::processEvents();

    /* The 'processEvents' invocations below are required in order for
     * queued slot invocations such as'deleteLater' to be done in time.
     */

    pointClouds.reset();
    QApplication::processEvents();
    components.reset();
    QApplication::processEvents();
    carna.reset();
    QApplication::processEvents();

    normalizing->setEnabled( false );
    masking->setEnabled( false );
    exporting->setEnabled( false );
    closing->setEnabled( false );
    maskExporting->setEnabled( false );
    maskImporting->setEnabled( false );
    acquiringViewTab->setEnabled( false );
    acquiringLocalizer->setEnabled( false );
    acquiringModelInfo->setEnabled( false );
    acquiringObjectsManager->setEnabled( false );
    acquiringPointClouds->setEnabled( false );
    acquiringRegistration->setEnabled( false );
    acquiringGulsun->setEnabled( false );

    tabWidget->clear();
    content->setCurrentWidget( carnaModelFactory );

    dlg.hide();
}


void MainWindow::acquirePointClouds()
{
    acquiringPointClouds->setEnabled( false );
    acquiringPointClouds->setChecked( true );

    PointCloudsComponent* pointClouds = new PointCloudsComponent( server, componentWindowFactory );
    components->takeComponent( pointClouds );

    connect( pointClouds, SIGNAL( destroyed() ), this, SLOT( pointCloudsReleased() ) );
}


void MainWindow::pointCloudsReleased()
{
    acquiringPointClouds->setEnabled( true );
    acquiringPointClouds->setChecked( false );
}


void MainWindow::acquireRegistration()
{
    acquiringRegistration->setEnabled( false );
    acquiringRegistration->setChecked( true );

    RegistrationComponent* registration = new RegistrationComponent( server, componentWindowFactory );
    components->takeComponent( registration );

    connect( registration, SIGNAL( destroyed() ), this, SLOT( registrationReleased() ) );
}


void MainWindow::registrationReleased()
{
    acquiringRegistration->setEnabled( true );
    acquiringRegistration->setChecked( false );
}


void MainWindow::acquireGulsun()
{
    acquiringGulsun->setEnabled( false );
    acquiringGulsun->setChecked( true );

    GulsunComponent* gulsun = new GulsunComponent( server, componentWindowFactory );
    components->takeComponent( gulsun );

    connect( gulsun, SIGNAL( destroyed() ), this, SLOT( gulsunReleased() ) );
}


void MainWindow::gulsunReleased()
{
    acquiringGulsun->setEnabled( true );
    acquiringGulsun->setChecked( false );
}


void MainWindow::exit()
{
    this->close();
}


ViewWindow* MainWindow::acquireViewTab()
{
    ViewWindow* pViewWindow = new ViewWindow( server );
    tabWidget->addTab( pViewWindow, QString( "View Array " ).append( QString().setNum( nextTabId++ ) ) );
    if( tabWidget->count() > 1 )
    {
        tabWidget->setTabsClosable( true );
    }
    return pViewWindow;
}


void MainWindow::tabCloseRequested( int index )
{
    QWidget* const page = tabWidget->widget( index );
    tabWidget->removeTab( index );
    delete page;
    if( tabWidget->count() == 1 )
    {
        tabWidget->setTabsClosable( false );
    }
}


void MainWindow::closeEvent( QCloseEvent* )
{
    QApplication::exit( 0 );
}


void MainWindow::showSettings()
{
    /*
    OptionsDialog dlg( this );
    dlg.exec();
    */
}


void MainWindow::exportRecord()
{
    if( !carna.get() )
    {
        return;
    }

    Exporter exporter( server, this );
    exporter.install( new BinaryDumpProcessor() );
    exporter.run();
}


void MainWindow::normalize()
{
    VolumeNormalizer normalizer( carna->model(), this );

    bool ok = false;
    normalizer.setThreshold( QInputDialog::getInt
        ( this
        , "Volume Normalization"
        , "HUV threshold:"
        , normalizer.getThreshold()
        , -3071
        , 1024
        , 1
        , &ok ) );

    if( !ok )
    {
        return;
    }

    normalizer.compute();

    QString sizeLoss = QString::number( normalizer.getSizeLoss() * 100, 'f', 2 );

    const QMessageBox::StandardButton defaultButton =
                       std::abs( normalizer.getSizeLoss() - 1. ) < 1e-4
                    || std::abs( normalizer.getSizeLoss() ) < 1e-4
            ? QMessageBox::No
            : QMessageBox::Yes;

    if( QMessageBox::question( this, "Volume Normalization", "The resolution of the normalized volume data is " + sizeLoss + "% of the original. Do you want to close your current data set and load the new one?", QMessageBox::Yes | QMessageBox::No, defaultButton ) == QMessageBox::Yes )
    {
        Carna::base::model::Scene* const new_model = normalizer.getResult();
        this->closeRecord();
        this->init( new_model );
    }
}


void MainWindow::mask()
{
    std::unique_ptr< MaskingDialog > dlg( new MaskingDialog( server, this ) );
    if( dlg->exec() == QDialog::Accepted )
    {
        CARNA_ASSERT( dlg->hasMask() );

        std::unique_ptr< Carna::base::model::SceneFactory > modelFactory( new Carna::base::model::SceneFactory( this ) );
        Carna::base::model::Scene* const newModel = modelFactory->createFromVolumeMasking( carna->model(), dlg->getMask() );

        modelFactory.reset();
        dlg.reset();
        this->closeRecord();
        this->init( newModel );
    }
}


void MainWindow::exportMask()
{
    CARNA_ASSERT( carna->model().hasVolumeMask() );
    CARNA_ASSERT( carna->model().volumeMask().isBinary() );
    CARNA_ASSERT( sizeof( unsigned char ) == sizeof( uint8_t ) );

    const QString filename
        = QFileDialog::getSaveFileName
        ( this
        , "Export Binary Mask"
        , ""
        , "Binary masks (*.mask)"
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
        QMessageBox::critical( this, "Export Binary Mask", "Failed opening file for writing." );
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );
    QDataStream out( &file );

    const Carna::base::model::BufferedMaskAdapter::BinaryMask& mask = carna->model().volumeMask().binary();
    out << mask.size.x << mask.size.y << mask.size.z;

    QProgressDialog progress( "Exporting binary mask...", "", 0, mask.size.z, this );
    progress.setCancelButton( nullptr );
    progress.show();

    for( unsigned int z = 0; z < mask.size.z; ++z )
    {
        for( unsigned int y = 0; y < mask.size.y; ++y )
        for( unsigned int x = 0; x < mask.size.x; ++x )
        {
            const uint8_t mask_value = mask( x, y, z );
            out.writeRawData( reinterpret_cast< const char* >( &mask_value ), 1 );
        }

        progress.setValue( z );
    }

    progress.close();

    file.close();
    QApplication::restoreOverrideCursor();
}


void MainWindow::importMask()
{
    CARNA_ASSERT( sizeof( unsigned char ) == sizeof( uint8_t ) );

    const QString filename
        = QFileDialog::getOpenFileName
        ( this
        , "Import Binary Mask"
        , ""
        , "Binary masks (*.mask)"
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
        QMessageBox::critical( this, "Import Binary Mask", "Failed opening file for reading." );
        return;
    }

    QDataStream in( &file );

    Carna::base::Vector3ui size;
    in >> size.x >> size.y >> size.z;

    if( size.x != carna->model().volume().size.x ||
        size.y != carna->model().volume().size.y ||
        size.z != carna->model().volume().size.z )
    {
        if( QMessageBox::question( this, "Import Binary Mask", "The volume resolution does not match the loaded datasets resolution. Shall the mask be loaded anyway?" ) != QMessageBox::Ok )
        {
            file.close();
            return;
        }
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    Carna::base::model::BufferedMaskAdapter::BinaryMask* const mask = new Carna::base::model::BufferedMaskAdapter::BinaryMask( size );

    QProgressDialog progress( "Importing binary mask...", "", 0, size.z, this );
    progress.setCancelButton( nullptr );
    progress.show();

    for( unsigned int z = 0; z < size.z; ++z )
    {
        for( unsigned int y = 0; y < size.y; ++y )
        {
            std::vector< char > line_binary_values( size.x );
            const unsigned int read_bytes = in.readRawData( &line_binary_values.front(), line_binary_values.size() );
            CARNA_ASSERT( read_bytes == line_binary_values.size() );

            for( unsigned int x = 0; x < size.x; ++x )
            {
                const uint8_t mask_value = static_cast< uint8_t >( line_binary_values[ x ] );
                ( *mask )( x, y, z ) = mask_value;
            }
        }

        progress.setValue( z );
    }

    progress.close();

    file.close();

    carna->model().setVolumeMask(
        new Carna::base::model::BufferedMaskAdapter(
            new Carna::base::Composition< Carna::base::model::BufferedMaskAdapter::BinaryMask >(
                mask ) ) );

    QApplication::restoreOverrideCursor();
}
