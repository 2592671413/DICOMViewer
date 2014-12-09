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

#include <QWidget>
#include <Carna/noncopyable.h>
#include "Server.h"

class RegistrationArguments;
class RegistrationController;



// ----------------------------------------------------------------------------------
// RegistrationOperation
// ----------------------------------------------------------------------------------

/** \brief	Represents some registration method, such as point to point registration or ICP.
  *
  * \author Leonid Kostrykin
  * \date   4.6.12 - 31.7.12
  */
class RegistrationOperation
{

    NON_COPYABLE

public:

    /** \brief	Instantiates.
      */
    RegistrationOperation( const std::string& name, const std::string& predicate, Record::Server& server );

    /** \brief	Does nothing.
      */
    virtual ~RegistrationOperation();


    /** \brief	Holds the name of this registration method.
      */
    const std::string name;

    /** \brief	Holds the predicate of this operation.
      */
    const std::string predicate;

    /** \brief	References the record service.
      */
    Record::Server& server;


    /** \brief	Invokes \ref perform, performing several checks.
      */
    bool performSafely( const RegistrationArguments& args
                      , RegistrationController& controller
                      , QWidget* dialogParent = 0 );


    /** \brief	Creates and returns a controller widget for this registration method.
      *
      * May also return \c nullptr.
      */
    virtual QWidget* createController() = 0;


protected:

    /** \brief	Performs the registration.
      */
    virtual void perform( const RegistrationArguments& args
                        , RegistrationController& controller
                        , QWidget* dialogParent ) = 0;

}; // RegistrationOperation
