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

#include "Server.h"
#include <CRA/Registration.h>



// ----------------------------------------------------------------------------------
// Registration
// ----------------------------------------------------------------------------------

/** \brief	Defines the registration service.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class Registration : public Record::GenericService< Registration >
{

public:

    
    /** \brief	References the supplied registration.
      */
    virtual CRA::Registration& operator*() const = 0;
    
    /** \brief	References the supplied registration.
      */
    CRA::Registration* operator->() const
    {
        return &( **this );
    }
    
    
    /** \brief	Connects the \ref RegistrationProvider::rmsChanged "rmsChanged()" signal.
      */
    virtual void connectRmsChanged( QObject* receiver, const char* slot ) = 0;
    
    /** \brief	Connects the \ref RegistrationProvider::transformationChanged "transformationChanged()" signal.
      */
    virtual void connectTransformationChanged( QObject* receiver, const char* slot ) = 0;

    /** \brief	Tells the RMS of the current registration.
      */
    virtual double getRms() = 0;

    /** \brief	Sets the RMS of the current registration.
      */
    virtual void setRms( double ) = 0;

}; // Registration


template< >
const std::string Record::GenericService< Registration >::serviceID = "Registration";
