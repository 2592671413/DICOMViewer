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

#include "RegistrationOperation.h"



// ----------------------------------------------------------------------------------
// PointToPointRegistration
// ----------------------------------------------------------------------------------

/** \brief	Performs point to point registration.
  *
  * \date   4.6.12
  * \author Leonid Kostrykin
  */
class PointToPointRegistration : public RegistrationOperation
{

public:

    /** \brief	Instantiates.
      */
    PointToPointRegistration( Record::Server& server );


    /** \brief	Returns \c nullptr.
      */
    virtual QWidget* createController() override;


protected:

    virtual void perform( const RegistrationArguments& args
                        , RegistrationController& controller
                        , QWidget* dialogParent ) override;

}; // PointToPointRegistration
