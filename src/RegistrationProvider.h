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

#include "Registration.h"



// ----------------------------------------------------------------------------------
// RegistrationProvider
// ----------------------------------------------------------------------------------

/** \brief  Defines the registration service provider.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class RegistrationProvider : public QObject, public Record::Provider< Registration >
{

    Q_OBJECT

public:
    
    /** \copydoc Record::Provider::Provider
      */
    RegistrationProvider( Record::Server& server, CRA::Tool& referenceBase )
        : Provider( server, Provider::manualInstallation )
        , registration( referenceBase )
        , rms( 0 )
    {
        server.provide( *this );

        connect( &registration, SIGNAL( transformationChanged() ), this, SLOT( processNewTransformation() ) );
    }


    virtual CRA::Registration& operator*() const override
    {
        return const_cast< RegistrationProvider* >( this )->registration;
    }

    virtual void connectRmsChanged( QObject* receiver, const char* slot ) override
    {
        connect( this, SIGNAL( rmsChanged() ), receiver, slot );
    }

    virtual void connectTransformationChanged( QObject* receiver, const char* slot ) override
    {
        connect( this, SIGNAL( transformationChanged() ), receiver, slot );
    }

    virtual double getRms() override
    {
        return rms;
    }

    virtual void setRms( double rms ) override
    {
        this->rms = rms;
        
        emit rmsChanged();
    }


private:

    /** \brief	Holds the currently supplied registration.
      */
    CRA::Registration registration;

    /** \brief	Holds the RMS of the current registration.
      */
    double rms;


signals:

    /** \brief	The RMS has been updated via \ref setRms.
      */
    void rmsChanged();

    /** \brief	The transformation matrix has changed.
      */
    void transformationChanged();


private slots:

    /** \brief	Emits the \ref transformationChanged signal.
      */
    void processNewTransformation()
    {
        emit transformationChanged();
    }

}; // RegistrationProvider
