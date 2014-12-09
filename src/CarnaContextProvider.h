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
#include <Carna/Scene.h>



// ----------------------------------------------------------------------------------
// CarnaContextProvider
// ----------------------------------------------------------------------------------

/** \brief	Defines the Carna service provider.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.7.12
  */
class CarnaContextProvider : public Record::Provider< CarnaContext >
{

public:

    /** \copydoc Record::Provider::Provider
      */
    CarnaContextProvider( Record::Server& server, Carna::Model* model )
        : Provider( server )
        , myModel( model )
        , myScene( new Carna::Scene( *model ) )
    {
    }

    /** \brief  Releases the Carna model and context.
      */
    virtual ~CarnaContextProvider();


    virtual Carna::Scene& scene() const override
    {
        return *myScene;
    }

    virtual Carna::Model& model() const override
    {
        return *myModel;
    }


private:

    const std::unique_ptr< Carna::Model > myModel;

    const std::unique_ptr< Carna::Scene > myScene;

};
