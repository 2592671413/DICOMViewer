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

#include "RegistrationController.h"
#include "LocalizerClient.h"
#include "CarnaContextClient.h"
#include "NotificationsClient.h"
#include "RegistrationClient.h"
#include "RegistrationProvider.h"
#include "RegistrationOperation.h"
#include "RegistrationArguments.h"
#include "ToolChooser.h"
#include "PointCloudChooser.h"
#include "PointToPointRegistration.h"
#include "ICP.h"
#include "Validation.h"
#include "PointCloud.h"
#include <Carna/base/qt/Object3DChooser.h>
#include <Carna/base/model/Object3D.h>
#include <Carna/base/model/Position.h>
#include <Carna/base/Transformation.h>
#include <Carna/base/CarnaException.h>
#include <Carna/base/Math.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QApplication>



// ----------------------------------------------------------------------------------
// RegistrationController
// ----------------------------------------------------------------------------------

RegistrationController::RegistrationController( Record::Server& server, RegistrationComponent& component )
    : QWidget()
    , server( server )
    , registration( nullptr )
    , component( component )
    , laRms( new QLabel( "" ) )
    , laTranslation( new QLabel( "" ) )
    , customTranslationPoint( new Carna::base::qt::Object3DChooser( CarnaContextClient( server ).model() ) )
    , customTranslationPointCloud( new PointCloudChooser( server ) )
    , buSetCustomTranslation( new QPushButton( "Set" ) )
    , controllerOptionsFrame( new QGroupBox( "" ) )
    , cbMethod( new QComboBox() )
    , referenceBaseChooser( new ToolChooser ( server ) )
    , recordedCloudChooser( new PointCloudChooser( server ) )
    , virtualCloudChooser ( new PointCloudChooser( server ) )
    , buOperationPredicate( new QPushButton() )
{
    QVBoxLayout* global = new QVBoxLayout();
    this->setLayout( global );

    QGroupBox* const customTranslationFrame = new QGroupBox( "Custom Translation" );
    QFormLayout* const customTranslationForm = new QFormLayout();
    customTranslationFrame->setLayout( customTranslationForm );
    customTranslationForm->addRow( "Data-side point:", customTranslationPoint );
    customTranslationForm->addRow( "Tracking-side point from:", customTranslationPointCloud );
    customTranslationForm->addRow( buSetCustomTranslation );

    connect( buSetCustomTranslation, SIGNAL( clicked() ), this, SLOT( updateCustomTranslation() ) );

    QFormLayout* form = new QFormLayout();
    form->addRow( "Current RMS:", laRms );
    form->addRow( "Translation: ", laTranslation );

    QGroupBox* controllerFrame = new QGroupBox( "Operations" );

    controllerOptionsFrame->setVisible( false );
    controllerOptionsFrame->setLayout( new QVBoxLayout() );

    global->addLayout( form );
    global->addSpacing( 10 );
    global->addWidget( customTranslationFrame );
    global->addSpacing( 10 );
    global->addWidget( controllerFrame );
    global->addWidget( controllerOptionsFrame );
    global->addStretch();

 // ----------------------------------------------------------------------------------

    updateView();

 // ----------------------------------------------------------------------------------

    NotificationsClient( server ).connectServiceProvided( this, SLOT( processProvidedService( const std::string& ) ) );
    NotificationsClient( server ).connectServiceRemoved ( this, SLOT(  processRemovedService( const std::string& ) ) );

 // ----------------------------------------------------------------------------------

    QFormLayout* const controller = new QFormLayout();
    controllerFrame->setLayout( controller );

    cbMethod->setEditable( false );
    controller->addRow( "Operation:", cbMethod );

    operations.push_back( new PointToPointRegistration( server ) );
    operations.push_back( new ICP( server ) );
    operations.push_back( new Validation( server ) );

    for( auto opIterator = operations.begin(); opIterator != operations.end(); ++opIterator )
    {
        RegistrationOperation& op = **opIterator;
        cbMethod->addItem( QString::fromStdString( op.name ) );
    }

    connect( cbMethod, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setOperation( int ) ) );

    controller->addRow( "Reference base:"      , referenceBaseChooser );
    controller->addRow( "Tracking-side points:", recordedCloudChooser );
    controller->addRow( "Data-side points:"    , virtualCloudChooser  );

    connect( buOperationPredicate, SIGNAL( clicked() ), this, SLOT( performOperation() ) );
    controller->addRow( buOperationPredicate );

    setOperation( 0 );
}


RegistrationController::~RegistrationController()
{
}


void RegistrationController::setRegistration( CRA::Tool& referenceBase
                                             , const Carna::base::Transformation& transformation
                                             , double rms )
{
    if( !this->registration.get() || &( **( this->registration ) ).getReferenceBase() != &referenceBase )
    {
        this->registration.reset();
        this->registration.reset( new RegistrationProvider( server, referenceBase ) );
    }

 // set transformation matrix

    ( **( this->registration ) ).setTransformation( transformation );

 // set RMS

    this->registration->setRms( rms );
}


void RegistrationController::updateCustomTranslation()
{
    if( !server.hasService( Registration::serviceID ) )
    {
        return;
    }

    CRA::Registration& registration = *RegistrationClient( server );
    if( customTranslationPoint->isObject3DSelected() )
    {
        if( !customTranslationPointCloud->isPointCloudSelected() )
        {
            QMessageBox::critical( this, "Registration", "You need to select the point cloud where the corresponding (nearest) point to the selected data point will be picked from." );
            return;
        }
        if( customTranslationPointCloud->getSelectedPointCloud().getList().empty() )
        {
            QMessageBox::critical( this, "Registration", "The tracking-side point cloud you selected is empty." );
            return;
        }

     // find closest model point
        
        using Carna::base::Vector;
        using Carna::base::Math;
        const Vector data_point = customTranslationPoint->selectedObject3D().position().toMillimeters();
        const Vector data_point_in_world_cs = registration.getTransformation().inverse() * data_point;

        QApplication::setOverrideCursor( Qt::WaitCursor );

        const PointCloud::PointList& model_points = customTranslationPointCloud->getSelectedPointCloud().getList();
        double min_square_distance = std::numeric_limits< double >::max();
        const Vector* closest_model_point = nullptr;
        for( auto it = model_points.begin(); it != model_points.end(); ++it )
        {
            const PointCloud::Point& model_point = *it;
            const double square_distance = Math::sq( model_point.x() - data_point_in_world_cs.x() )
                                         + Math::sq( model_point.y() - data_point_in_world_cs.y() )
                                         + Math::sq( model_point.z() - data_point_in_world_cs.z() );

            if( square_distance < min_square_distance )
            {
                min_square_distance = square_distance;
                closest_model_point = &model_point;
            }
        }

        CARNA_ASSERT( closest_model_point != nullptr );

        QApplication::restoreOverrideCursor();

     // compute required translation offset

        const Vector model_point = registration.getTransformation() * ( *closest_model_point );
        const Vector translation_offset = data_point - model_point;

     // compute new custom translation

        const Vector current_translation = registration.getTransformation() * Vector( 0, 0, 0 );
        const Vector new_custom_translation = current_translation + translation_offset;

     // finish

        registration.setCustomTranslation( new_custom_translation );
    }
    else
    {
        registration.removeCustomTranslation();
    }

    updateView();
}


void RegistrationController::updateView()
{
    if( !server.hasService( Registration::serviceID ) )
    {
        const static QString notRegistered = "not registered";

        laRms->setText( notRegistered );
        laTranslation->setText( notRegistered );

        customTranslationPoint->setEnabled( false );
        customTranslationPointCloud->setEnabled( false );
        buSetCustomTranslation->setEnabled( false );

        return;
    }

    CRA::Registration& registration = *RegistrationClient( server );

    const double tx = registration.getTransformation().a14();
    const double ty = registration.getTransformation().a24();
    const double tz = registration.getTransformation().a34();

    const double translation = std::sqrt( tx*tx + ty*ty + tz*tz );

    laTranslation->setText( QString::number( translation ) + " mm" );

    laRms->setText( QString::number( RegistrationClient( server ).getRms() ) + " mm" );

    customTranslationPoint->setEnabled( true );
    customTranslationPointCloud->setEnabled( true );
    buSetCustomTranslation->setEnabled( true );
}


void RegistrationController::processProvidedService( const std::string& interfaceID )
{
    if( interfaceID == Registration::serviceID )
    {
        RegistrationClient( server ).connectRmsChanged( this, SLOT( updateView() ) );
        RegistrationClient( server ).connectTransformationChanged( this, SLOT( updateView() ) );

        updateView();
    }
}


void RegistrationController::processRemovedService( const std::string& interfaceID )
{
    if( interfaceID == Registration::serviceID )
    {
        updateView();
    }
}


void RegistrationController::setOperation( int )
{
    buOperationPredicate->setText( QString::fromStdString( currentOperation().predicate ) );

 // update operation controller

    QWidget* const op_controller = currentOperation().createController();

    if( op_controller )
    {
        delete controllerOptionsFrame->layout();
        qDeleteAll( controllerOptionsFrame->children() );
        controllerOptionsFrame->setLayout( new QVBoxLayout() );

        controllerOptionsFrame->layout()->addWidget( op_controller );
        controllerOptionsFrame->setTitle( QString::fromStdString( currentOperation().name ) );
    }

    controllerOptionsFrame->setVisible( op_controller != nullptr );
}


RegistrationOperation& RegistrationController::currentOperation()
{
    const int selectedMethodIndex = cbMethod->currentIndex();

    return *operations[ selectedMethodIndex ];
}


const RegistrationOperation& RegistrationController::currentOperation() const
{
    return const_cast< RegistrationController* >( this )->currentOperation();
}


void RegistrationController::performOperation()
{
    RegistrationOperation& op = currentOperation();
    const QString msgBoxTitle = QString::fromStdString( op.name );

    if(     !recordedCloudChooser->isPointCloudSelected() 
     ||     !virtualCloudChooser ->isPointCloudSelected()
     ||   &( recordedCloudChooser->getSelectedPointCloud() )
       == &( virtualCloudChooser ->getSelectedPointCloud() ) )
    {
        QMessageBox::critical( this, msgBoxTitle, "The point cloud selection is invalid." );
        return;
    }

    if( !referenceBaseChooser->isToolSelected() )
    {
        QMessageBox::critical( this, msgBoxTitle, "The reference base selection is invalid." );
        return;
    }

    const PointCloud& recordedCloud = recordedCloudChooser->getSelectedPointCloud();
    const PointCloud& virtualCloud = virtualCloudChooser->getSelectedPointCloud();
    CRA::Tool& referenceBase = referenceBaseChooser->getSelectedTool();

    if( !verifyPointCloudSelection( *recordedCloudChooser, msgBoxTitle )
     || !verifyPointCloudSelection( *virtualCloudChooser , msgBoxTitle ) )
    {
        return;
    }

    const RegistrationArguments args( recordedCloud, virtualCloud, referenceBase );

    if( !op.performSafely( args, *this, this ) )
    {
        /* Usually the error already has been printed to the user.
         */
    }
}


bool RegistrationController::verifyPointCloudSelection( PointCloudChooser& chooser, const QString& msgBoxTitle )
{
    if( !chooser.isPointCloudSelected() )
    {
        QMessageBox::critical( this, msgBoxTitle, "The point cloud selection is invalid." );

        return false;
    }

    PointCloud& cloud = chooser.getSelectedPointCloud();

    if( cloud.getFormat() == PointCloud::volumeUnits )
    {
        std::stringstream ss;
        ss << "The points of at least one selected point cloud are saved in model units, but they are required to be in millimeters. Do you want to convert them now?";
        ss << std::endl << std::endl;
        ss << "Selected point cloud: " << cloud.getName();

        if( QMessageBox::question
            ( this
            , msgBoxTitle
            , QString::fromStdString( ss.str() )
            , QMessageBox::Yes | QMessageBox::Cancel
            , QMessageBox::Yes ) == QMessageBox::Yes )
        {
            cloud.convert( PointCloud::millimeters, this );

            return verifyPointCloudSelection( chooser, msgBoxTitle );
        }

        return false;
    }

    return true;
}


void RegistrationController::closeEvent( QCloseEvent* ev )
{
    if( registration.get() )
    {
        if( QMessageBox::warning
            ( this
            , "Registration"
            , "You are closing the registration controller.\nYour current registration will be released."
            , QMessageBox::Ok | QMessageBox::Cancel
            , QMessageBox::Ok ) == QMessageBox::Cancel )
        {
            ev->ignore();
            return;
        }
    }

    ev->accept();
}
