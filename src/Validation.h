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
// Validation
// ----------------------------------------------------------------------------------

/** \brief	Re-computes the RMS using two custom \ref PointCloud "point clouds".
  *
  * \date   23.7.12
  * \author Leonid Kostrykin
  */
class Validation : public RegistrationOperation
{

public:

    /** \brief	Instantiates.
      */
    Validation( Record::Server& server );


    /** \brief	Returns \c nullptr.
      */
    virtual QWidget* createController() override;


protected:

    virtual void perform( const RegistrationArguments& args
                        , RegistrationController& controller
                        , QWidget* dialogParent ) override;

}; // Validation
