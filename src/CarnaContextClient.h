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

#include "CarnaContext.h"
#include <Carna/base/view/SceneProvider.h>



// ----------------------------------------------------------------------------------
// CarnaContextClient
// ----------------------------------------------------------------------------------

/** \brief	Defines the Carna service client.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.7.12
  */
class CarnaContextClient : public Record::Client< CarnaContext >
{

public:
    
    /** \copydoc Record::Client::Client
      */
    CarnaContextClient( Record::Server& server )
        : Client( server )
    {
    }

    virtual Carna::base::view::SceneProvider& scene() const override
    {
        return destination.scene();
    }

    virtual Carna::base::model::Scene& model() const override
    {
        return destination.model();
    }

}; // CarnaContextClient
