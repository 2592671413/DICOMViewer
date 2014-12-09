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

/** \file   RegistrationController.h
  * \brief  Defines RegistrationController.
  */

#include "Server.h"
#include "Component.h"
#include <Carna/Carna.h>
#include <Carna/Transformation.h>
#include <CRA/Tool.h>
#include <QWidget>

class RegistrationComponent;
class RegistrationFactory;
class RegistrationProvider;
class RegistrationOperation;
class RegistrationController;
class PointCloudChooser;
class ToolChooser;

class QLabel;
class QGroupBox;
class QComboBox;
class QPushButton;




// ----------------------------------------------------------------------------------
// RegistrationController
// ----------------------------------------------------------------------------------

class RegistrationController : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    RegistrationController( Record::Server& server, RegistrationComponent& component );

    /** \brief  Does nothing.
      */
    virtual ~RegistrationController();


    void setRegistration( CRA::Tool& referenceBase, const Carna::Tools::Transformation&, double rms );


    /** \brief	References currently selected method.
      */
    RegistrationOperation& currentOperation();

    /** \brief	References currently selected method.
      */
    const RegistrationOperation& currentOperation() const;


protected:

    virtual void closeEvent( QCloseEvent* ) override;


private:

    /** \brief	References the record service.
      */
    Record::Server& server;

    /** \brief	Holds the registration service provider.
      */
    std::unique_ptr< RegistrationProvider > registration;

    /** \brief	References the component this widget belongs to.
      */
    RegistrationComponent& component;

    /** \brief	References the RMS displaying label.
      */
    QLabel* laRms;
    
    /** \brief	References the translation displaying label.
      */
    QLabel* laTranslation;

    Carna::Object3DChooser* const customTranslationPoint;

    PointCloudChooser* const customTranslationPointCloud;

    QPushButton* const buSetCustomTranslation;

    /** \brief	Contains the registration method's controller widget.
      */
    QGroupBox* const controllerOptionsFrame;
    
    /** \brief	Holds all available registration operations.
      */
    std::vector< RegistrationOperation* > operations;

    /** \brief	Presents \ref operations to the user.
      */
    QComboBox* const cbMethod;
    
    /** \brief	References the button for performing the selected operation.
      */
    QPushButton* const buOperationPredicate;

    /** \brief	Lets the user select the reference base.
      */
    ToolChooser* referenceBaseChooser;

    /** \brief	Lets the user select the source point cloud.
      */
    PointCloudChooser* recordedCloudChooser;
    
    /** \brief	Lets the user select the target point cloud.
      */
    PointCloudChooser* virtualCloudChooser;


    bool verifyPointCloudSelection( PointCloudChooser&, const QString& msgBoxTitle );


private slots:

    /** \brief	Updates the user interface.
      */
    void updateView();

    /** \brief	Enables this controller when a registration service facet provider is
      *         available.
      */
    void processProvidedService( const std::string& interfaceID );
    
    /** \brief	Disables this controller when no registration service facet provider is
      *         available anymore.
      */
    void processRemovedService( const std::string& interfaceID );

    /** \brief	Updates currently set registration by the custom translation.
      */
    void updateCustomTranslation();

    /** \brief	Creates controller widget from registration's current method within \ref controllerOptionsFrame
      *         and updates label on \ref buOperationPredicate.
      *
      * The argument is ignored.
      */
    void setOperation( int );

    /** \brief	Invokes \ref RegistrationOperation::performRegistrationSafely on the currently selected method.
      */
    void performOperation();

}; // RegistrationController
