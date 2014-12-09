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

#include "VolumeController.h"
#include "CarnaContextClient.h"
#include "ToolChooser.h"
#include "RegistrationClient.h"
#include "NotificationsClient.h"
#include "VolumeViewCameraController.h"
#include "GulsunComponent.h"
#include "VolumeView.h"
#include <Carna/base/Composition.h>
#include <Carna/base/Aggregation.h>
#include <Carna/VolumeRenderings/VolumeControllerUI.h>
#include <Carna/VolumeRenderings/VolumeVisualization.h>
#include <Carna/VolumeRenderings/VolumeRenderer.h>
#include <Carna/VolumeRenderings/MIP/MaximumIntensityProjection.h>
#include <Carna/base/view/DefaultCamera.h>
#include <Carna/base/Association.h>
#include <Carna/base/view/SceneProvider.h>
#include <Carna/base/view/Monoscopic.h>
#include <Carna/base/CarnaException.h>
#include <Carna/stereoscopic/Philips.h>
#include <Carna/stereoscopic/Zalman.h>
#include <Carna/base/qt/ExpandableGroupBox.h>
#include <CRA/Link.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QIcon>
#include <QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>



// ----------------------------------------------------------------------------------
// VolumeController
// ----------------------------------------------------------------------------------

VolumeController::VolumeController( Carna::VolumeRenderings::VolumeVisualization& view, Record::Server& server, VolumeView& tool )
    : tool( tool )
    , view( view )
    , server( server )
    , cameraController( new VolumeViewCameraController( view ) )
    , cbCameraChooser( new QComboBox() )
    , linkedCameraChooser( new ToolChooser( server ) )
    , preferredCameraMode( undefinedPreferredProjection )
    , cbAutoRotate( new QCheckBox( "Auto-rotate with:" ) )
    , sbSecondsPerRotation( new QDoubleSpinBox() )
    , sbTargetFramesPerSecond( new QSpinBox() )
    , cbRenderMode( new QComboBox() )
    , buGulsun( new QPushButton( "Accelerated Gulsun Vessel Segmentation" ) )
{
    CARNA_ASSERT( view.isInitialized() );

    Carna::VolumeRenderings::VolumeControllerUI* const mainController = new Carna::VolumeRenderings::VolumeControllerUI( view.renderer(), CarnaContextClient( server ).model() );

    linkedCameraChooser->setEnabled( server.hasService( Registration::serviceID ) );

    connect
        ( linkedCameraChooser
        , SIGNAL( selectionChanged( CRA::Tool& ) )
        , this
        , SLOT(  setLinkedCamera( CRA::Tool& ) ) );

    connect
        ( linkedCameraChooser
        , SIGNAL(  selectionDissolved() )
        , this
        , SLOT( releaseLinkedCamera() ) );

    NotificationsClient( server ).connectServiceProvided( this, SLOT( processAddedService  ( const std::string& ) ) );
    NotificationsClient( server ).connectServiceRemoved ( this, SLOT( processRemovedService( const std::string& ) ) );

    QVBoxLayout* const global = new QVBoxLayout();
    QFormLayout* const header = new QFormLayout();

    QPushButton* const buResetCamera = new QPushButton( QIcon( ":/icons/home.png" ), "" );
    buResetCamera->setToolTip( "Reset Camera" );
    connect( buResetCamera, SIGNAL( clicked() ), this, SLOT( resetCamera() ) );

    QWidget* const cameraManager = new QWidget();
    cameraManager->setLayout( new QHBoxLayout() );
    cameraManager->layout()->setContentsMargins( 0, 0, 0, 0 );
    cameraManager->layout()->addWidget( linkedCameraChooser );
    cameraManager->layout()->addWidget( buResetCamera );

    header->addRow( "Camera linked to:", cameraManager );

 // render modes

    const QString renderModeNames[] = { "Monoscopic", "Philips", "Zalman" };
    for( unsigned int renderModeIndex = 0; renderModeIndex < 3; ++renderModeIndex )
    {
        cbRenderMode->addItem( renderModeNames[ renderModeIndex ] );
    }
    cbRenderMode->setCurrentIndex( 0 );
    connect( cbRenderMode, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setRenderMode( int ) ) ) ;

    header->addRow( "Render mode:", cbRenderMode );

 // camera setup

    Carna::base::qt::ExpandableGroupBox* const gbCameraSetup = new Carna::base::qt::ExpandableGroupBox( "Camera Setup" );
    QFormLayout* const cameraSetup = new QFormLayout();
    gbCameraSetup->child()->setLayout( cameraSetup );
    gbCameraSetup->child()->layout()->setContentsMargins( 0, 0, 0, 0 );
    header->addRow( gbCameraSetup );

 // camera setup - preferred projection

    QComboBox* const cbPreferredCameraMode = new QComboBox();
    cbPreferredCameraMode->addItem( "Perspective Projection" );
    cbPreferredCameraMode->addItem( "Orthogonal Projection" );
    cbPreferredCameraMode->addItem( "Automatic" );
    cbPreferredCameraMode->setCurrentIndex( static_cast< int >( preferredCameraMode ) );

    connect( cbPreferredCameraMode, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setPreferredCameraMode( int ) ) );
    connect( &view.renderer(), SIGNAL( modeChanged() ), this, SLOT( updateCamera() ) );

    QTimer::singleShot( 0, this, SLOT( updateCamera() ) );

    cameraSetup->addRow( "Preferred projection:", cbPreferredCameraMode );

 // camera setup - auto rotate

    view.setController( new Carna::base::controller::DefaultInteractionStrategy( view, cameraController ) );

    cbAutoRotate->setChecked( cameraController->hasAutoRotate() );
    sbSecondsPerRotation->setEnabled( cameraController->hasAutoRotate() );
    sbSecondsPerRotation->setRange( 1., 60. );
    sbSecondsPerRotation->setValue( cameraController->getSecondsPerRotation() );
    sbSecondsPerRotation->setPrefix( "360° / " );
    sbSecondsPerRotation->setSuffix( "s" );

    connect( cbAutoRotate, SIGNAL( toggled( bool ) ), cameraController, SLOT( setAutoRotate( bool ) ) );
    connect( cbAutoRotate, SIGNAL( toggled( bool ) ), sbSecondsPerRotation, SLOT( setEnabled( bool ) ) );
    connect( cbAutoRotate, SIGNAL( toggled( bool ) ), sbTargetFramesPerSecond, SLOT( setEnabled( bool ) ) );
    connect( sbSecondsPerRotation, SIGNAL( valueChanged( double ) ), cameraController, SLOT( setSecondsPerRotation( double ) ) );

    cameraSetup->addRow( cbAutoRotate, sbSecondsPerRotation );

 // camera setup - target frames per second

    sbTargetFramesPerSecond->setEnabled( cameraController->hasAutoRotate() );
    sbTargetFramesPerSecond->setRange( 1, 25 );
    sbTargetFramesPerSecond->setValue( cameraController->getStepsPerSecond() );
    sbTargetFramesPerSecond->setSuffix( "frames / 1s" );

    connect( sbTargetFramesPerSecond, SIGNAL( valueChanged( int ) ), cameraController, SLOT( setStepsPerSecond( int ) ) );

    cameraSetup->addRow( "Target FPS:", sbTargetFramesPerSecond );

 // accelerated Gulsun vessel segmentation

    connect( buGulsun, SIGNAL( clicked() ), this, SLOT( openGulsun() ) );

 // finish

    global->addLayout( header );
    global->addSpacing( 10 );
    global->addWidget( mainController );
	/*
    global->addWidget( buGulsun );
	*/

    mainController->layout()->setContentsMargins( 0, 0, 0, 0 );

    this->setLayout( global );
}


VolumeController::~VolumeController()
{
    releaseLinkedCamera();
}


void VolumeController::openGulsun()
{
    const Carna::base::model::Scene& model = view.renderer().provider.scene;
    IntensitySampler cpu( model );
    GpuIntensitySampler gpu( view.environment(), view.renderer().provider );

    const unsigned int z = 20;
    const unsigned int y = 20;

    const double rz = z / double( model.volume().size.z - 1 );
    const double ry = y / double( model.volume().size.y - 1 );

    for( unsigned int x = 1; x < model.volume().size.x - 1; ++x )
    {
        const double rx = x / double( model.volume().size.x - 1 );

        Carna::base::model::Position position = Carna::base::model::Position::fromVolumeUnits( model, rx, ry, rz );
        const double sample_cpu = cpu.valueAt( position.toMillimeters() );
        const double sample_gpu = gpu.valueAt( position.toMillimeters() );

        qDebug()
            << "CPU:" << QString::number( sample_cpu, 'f', 2 )
            << "GPU:" << QString::number( sample_gpu, 'f', 2 )
            << "deviation:" << sample_gpu - sample_cpu;
    }

 // ----------------------------------------------------------------------------------

    buGulsun->setEnabled( false );

    GulsunController* const gulsun = new GulsunController( server, &view.environment() );
    tool.createDockable( gulsun, Qt::RightDockWidgetArea ).setWindowTitle( "Accelerated Gulsun Vessel Segmentation" );
}


void VolumeController::gulsunClosed()
{
    buGulsun->setEnabled( true );
}


void VolumeController::setRenderMode( int renderModeIndex )
{
    Carna::base::view::Renderer::RenderMode* newRenderMode;
    switch( static_cast< RenderMode >( renderModeIndex ) )
    {
        
        case monoscopic:
        {
            newRenderMode = new Carna::base::view::Monoscopic( view.renderer() );
            break;
        }
        
        case philips:
        {
            newRenderMode = new Carna::stereoscopic::Philips( view.renderer() );
            break;
        }
        
        case zalman:
        {
            newRenderMode = new Carna::stereoscopic::Zalman( view.renderer() );
            break;
        }

        default:
            return;

    }

    view.renderer().setRenderMode( newRenderMode );
}


void VolumeController::resetCamera()
{
    if( linkedCameraChooser->isToolSelected() )
    {
        linkedCameraChooser->selectNone();
    }
    else
    {
        this->setDefaultCamera();
        view.renderer().invalidate();
    }
}


void VolumeController::processAddedService( const std::string& serviceID )
{
    if( serviceID == Registration::serviceID )
    {
        linkedCameraChooser->setEnabled( true );
    }
}


void VolumeController::processRemovedService( const std::string& serviceID )
{
    if( serviceID == Registration::serviceID )
    {
        linkedCameraChooser->selectNone();
        linkedCameraChooser->setEnabled( false );
    }
}


void VolumeController::setDefaultCamera()
{
    bool usePerspectiveProjection;
    switch( preferredCameraMode )
    {

        case perspectiveProjection:
        {
            usePerspectiveProjection = true;
            break;
        }

        case orthogonalProjection:
        {
            usePerspectiveProjection = false;
            break;
        }

        case undefinedPreferredProjection:
        default:
        {
            if( view.renderer().mode().name == Carna::VolumeRenderings::MIP::MaximumIntensityProjection::NAME )
            {
                usePerspectiveProjection = false;
            }
            else
            {
                usePerspectiveProjection = true;
            }
            break;
        }

    }

    Carna::base::view::Camera* camera = new Carna::base::view::DefaultCamera( view.renderer().provider.scene, usePerspectiveProjection );
    view.renderer().setCamera( new Carna::base::Composition< Carna::base::view::Camera >( camera ) );

    cbAutoRotate->setEnabled( true );
    sbSecondsPerRotation->setEnabled( cameraController->hasAutoRotate() );
    sbTargetFramesPerSecond->setEnabled( cameraController->hasAutoRotate() );
    cbAutoRotate->setChecked( cameraController->hasAutoRotate() );
}


void VolumeController::setPreferredCameraMode( int preferredCameraMode_int )
{
    CARNA_ASSERT( preferredCameraMode_int >= 0 && preferredCameraMode_int <= 2 );

    const PreferredCameraMode preferredCameraMode = static_cast< PreferredCameraMode >( preferredCameraMode_int );

    if( this->preferredCameraMode != preferredCameraMode )
    {
        this->preferredCameraMode = preferredCameraMode;
        this->updateCamera();
    }
}


void VolumeController::updateCamera()
{
    Carna::base::view::DefaultCamera* const camera = dynamic_cast< Carna::base::view::DefaultCamera* >( &view.renderer().camera() );
    if( camera )
    {
        switch( preferredCameraMode )
        {

            case perspectiveProjection:
            {
                camera->setPerspectiveProjection( true );
                break;
            }

            case orthogonalProjection:
            {
                camera->setPerspectiveProjection( false );
                break;
            }

            case undefinedPreferredProjection:
            default:
            {
                if( view.renderer().mode().name == Carna::VolumeRenderings::MIP::MaximumIntensityProjection::NAME )
                {
                    camera->setPerspectiveProjection( false );
                }
                else
                {
                    camera->setPerspectiveProjection( true );
                }
                break;
            }

        }
    }
}


void VolumeController::unlinkCamera()
{
    if( linkedCamera.get() && server.hasService( Registration::serviceID ) )
    {
        CRA::Registration& registration = *RegistrationClient( server );

        bool restart;
        do
        {
            restart = false;
            for( auto it = registration.getLinks().begin(); it != registration.getLinks().end(); )
            {
                CRA::VirtualElement& linkedObject = it->virtualElement;
                if( &linkedObject == linkedCamera.get() )
                {
                    registration.remove( *it );

                    restart = true;
                    break;
                }
            }
        }
        while( restart );
    }
}


void VolumeController::releaseLinkedCamera()
{
    unlinkCamera();

    if( linkedCamera.get() )
    {
        setDefaultCamera();

        linkedCamera.reset();

        view.renderer().invalidate();
    }
}


void VolumeController::setLinkedCamera( CRA::Tool& rb )
{
    if( !server.hasService( Registration::serviceID ) )
    {
        return;
    }

    unlinkCamera();

    try
    {
        CRA::LinkedCamera* const newCamera = new CRA::LinkedCamera();

        view.renderer().setCamera( new Carna::base::Aggregation< Carna::base::view::Camera >( *newCamera ) );

        linkedCamera.reset( newCamera );

        RegistrationClient( server )->add( CRA::Link( rb, *linkedCamera ) );

        cbAutoRotate->setEnabled( false );
        sbSecondsPerRotation->setEnabled( false );
        sbTargetFramesPerSecond->setEnabled( false );
    }
    catch( const std::runtime_error& ex )
    {
        std::stringstream ss;
        ss << "An error has occurred: " << std::endl << std::endl;
        ss << ex.what();

        linkedCameraChooser->selectNone();

        QMessageBox::critical( this, "Linked Camera", QString::fromStdString( ss.str() ) );
    }
}
