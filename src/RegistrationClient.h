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
// RegistrationClient
// ----------------------------------------------------------------------------------

/** \brief	Defines the registration service client.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class RegistrationClient : public Record::Client< Registration >
{

public:
    
    /** \copydoc Record::Client::Client
      */
    RegistrationClient( Record::Server& server )
        : Client( server )
    {
    }


    virtual CRA::Registration& operator*() const override
    {
        return *destination;
    }
    
    virtual void connectRmsChanged( QObject* receiver, const char* slot ) override
    {
        destination.connectRmsChanged( receiver, slot );
    }

    virtual void connectTransformationChanged( QObject* receiver, const char* slot ) override
    {
        destination.connectTransformationChanged( receiver, slot );
    }

    virtual double getRms() override
    {
        return destination.getRms();
    }

    virtual void setRms( double rms ) override
    {
        destination.setRms( rms );
    }

}; // RegistrationClient
