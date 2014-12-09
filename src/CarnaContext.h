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
#include <Carna/Carna.h>



// ----------------------------------------------------------------------------------
// CarnaContext
// ----------------------------------------------------------------------------------

/** \brief	Defines the Carna service.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class CarnaContext : public Record::GenericService< CarnaContext >
{

public:

    /** \brief	References the Carna resource sharing context.
      */
    virtual Carna::Scene& scene() const = 0;

    virtual Carna::Model& model() const = 0;
    
}; // CarnaContext


template< >
const std::string Record::GenericService< CarnaContext >::serviceID = "Carna";
