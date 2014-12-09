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

#include "ToolEditor.h"
#include "LocalizerClient.h"
#include "LocalizerProvider.h"
#include "CarnaContextClient.h"
#include "RegistrationClient.h"
#include "NotificationsClient.h"
#include "ToolCalibrator.h"
#include "LocalizerComponent.h"
#include <Carna/base/qt/Object3DChooser.h>
#include <CRA/Link.h>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QInputDialog>



// ----------------------------------------------------------------------------------
// ToolEditor
// ----------------------------------------------------------------------------------

const static QString rbIsVisibleText = "<font color=\"#00AA22\">Visible</font>";
const static QString rbIsInvisibleText = "<font color=\"#BB2200\">Invisible</font>";


ToolEditor::ToolEditor( LocalizerComponent& component, Record::Server& server, const std::string& port, const std::string& name, LocalizerProvider& localizer, QWidget* parent )
    : QWidget( parent )
    , server( server )
    , component( component )
    , tool( LocalizerClient( server ).getLocalizer(), port, name )
    , localizer( localizer )
    , laVisibility( new QLabel( rbIsInvisibleText ) )
    , laPort( new QLabel( QString::fromStdString( port ) ) )
    , buName( new QPushButton( QString::fromStdString( tool.getName() ) ) )
    , objectChooser( new Carna::base::qt::Object3DChooser( CarnaContextClient( server ).model() ) )
    , recentlyLinkedObject( nullptr )
    , calibratorWindow( nullptr )
    , recentlyVisible( false )
{
    connect( &tool, SIGNAL( renamed( const std::string& ) ), this, SLOT( setName( const std::string& ) ) );

    laPort->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    laPort->setAlignment( Qt::AlignCenter );

    buName->setFlat( true );
    buName->setStyleSheet( "text-decoration: underline;" );
    buName->setCursor( Qt::PointingHandCursor );
    buName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    connect( buName, SIGNAL( clicked() ), this, SLOT( rename() ) );

    objectChooser->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    objectChooser->setEnabled( server.hasService( Registration::serviceID ) );

    connect( objectChooser, SIGNAL( selectionChanged( Object3D& ) ), this, SLOT( setLinkedObject3D( Object3D& ) ) );
    connect( objectChooser, SIGNAL( selectionDissolved() ), this, SLOT( removeLinkedObject3D() ) );

    QPushButton* const buRemove = new QPushButton( "Remove" );

    buRemove->setFlat( true );
    buRemove->setStyleSheet( "text-decoration: underline;" );
    buRemove->setCursor( Qt::PointingHandCursor );
    buRemove->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    connect( buRemove, SIGNAL( clicked() ), this, SLOT( removeMyself() ) );

    localizer.addTool( tool );

    laVisibility->setStyleSheet( "border: 1px inset #CCCCCC; padding: 2px;" );
    laPort      ->setStyleSheet( "border: 1px inset #CCCCCC; padding: 2px;" );
    buName      ->setStyleSheet( "border: 1px inset #CCCCCC; padding: 2px; text-decoration: underline;" );
    buRemove    ->setStyleSheet( "border: 1px inset #CCCCCC; padding: 2px; text-decoration: underline;" );

    laVisibility->setSizePolicy( QSizePolicy::Fixed    , QSizePolicy::Minimum );
    laPort      ->setSizePolicy( QSizePolicy::Fixed    , QSizePolicy::Minimum );
    buRemove    ->setSizePolicy( QSizePolicy::Fixed    , QSizePolicy::Minimum );
    buName      ->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );

    this->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    laVisibility->setFixedWidth  ( 100 );
    laPort      ->setFixedWidth  ( 100 );
    buRemove    ->setFixedWidth  ( 100 );
    buName      ->setMinimumWidth( 100 );

    QPushButton* const buCalibrate = new QPushButton();
    buCalibrate->setIcon( QIcon( ":/icons/configure.png" ) );
    buCalibrate->setToolTip( "Pivot Calibration" );

    connect( buCalibrate, SIGNAL( clicked() ), this, SLOT( calibrate() ) );

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins( 1, 1, 1, 1 );
    layout->setSpacing( 2 );
    layout->addWidget( laVisibility );
    layout->addWidget( buName );
    layout->addWidget( laPort );
    layout->addWidget( objectChooser );
    layout->addWidget( buCalibrate );
    layout->addWidget( buRemove );
    this->setLayout( layout );

    NotificationsClient( server ).connectServiceProvided( this, SLOT( processProvidedService( const std::string& ) ) );
    NotificationsClient( server ).connectServiceRemoved ( this, SLOT(  processRemovedService( const std::string& ) ) );
}


void ToolEditor::processProvidedService( const std::string& serviceID )
{
    if( serviceID == Registration::serviceID )
    {
        objectChooser->setEnabled( true );
        objectChooser->selectNone();
    }
}


void ToolEditor::processRemovedService( const std::string& serviceID )
{
    if( serviceID == Registration::serviceID )
    {
        objectChooser->selectNone();
        objectChooser->setEnabled( false );
    }
}


void ToolEditor::setLinkedObject3D( Carna::base::model::Object3D& object )
{
    if( server.hasService( Registration::serviceID ) )
    {
        if( recentlyLinkedObject.get() )
        {
            removeLinkedObject3D();
        }

        CRA::Registration& registration = *RegistrationClient( server );

        recentlyLinkedObject.reset( new CRA::LinkedObject( object ) );

        CRA::Link link( tool, *recentlyLinkedObject );

        registration.add( link );
    }
}


void ToolEditor::removeLinkedObject3D()
{
    if( server.hasService( Registration::serviceID ) && recentlyLinkedObject.get() )
    {
        CRA::Registration& registration = *RegistrationClient( server );

        CRA::Link link( tool, *recentlyLinkedObject );

        registration.remove( link );

        recentlyLinkedObject.reset();
    }
}


void ToolEditor::removeMyself()
{
    removeLinkedObject3D();

    emit removeRequested( this );
}


ToolEditor::~ToolEditor()
{
    if( calibratorWindow )
    {
        calibratorWindow->close();
    }
    localizer.removeTool( tool );
}


void ToolEditor::updateView()
{
    if( tool.isVisible() == recentlyVisible )
    {
        return;
    }

    if( tool.isVisible() )
    {
        laVisibility->setText( rbIsVisibleText );
    }
    else
    {
        laVisibility->setText( rbIsInvisibleText );
    }

    recentlyVisible = tool.isVisible();
}


void ToolEditor::setName( const std::string& newName )
{
    buName->setText( QString::fromStdString( newName ) );
}


void ToolEditor::rename()
{
    bool ok;

    QString text = QInputDialog::getText( this, this->windowTitle()
                                        , "Rename:", QLineEdit::Normal
                                        , QString::fromStdString( tool.getName() ), &ok );

    if( ok && !text.isEmpty() )
    {
        tool.setName( text.toStdString() );
    }
}


void ToolEditor::calibrate()
{
    if( calibratorWindow )
    {
        calibratorWindow->raise();
    }
    else
    {
        ToolCalibrator* calibrator = new ToolCalibrator( server, tool );
        calibratorWindow = &component.createEmbeddable( calibrator, NullEmbeddablePlacer::instance(), QString::fromStdString( tool.getName() ) );

        connect( &tool, SIGNAL( renamed( const QString& ) ), calibratorWindow, SLOT( setSuffix( const QString& ) ) );
        connect( calibratorWindow, SIGNAL( destroyed() ), this, SLOT( calibrationClosed() ) );
    }
}


void ToolEditor::calibrationClosed()
{
    this->calibratorWindow = nullptr; // TODO: am I obsoleted?
}
