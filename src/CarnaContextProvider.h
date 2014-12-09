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
    CarnaContextProvider( Record::Server& server, Carna::base::model::Scene* model )
        : Provider( server )
        , myModel( model )
        , myScene( new Carna::base::view::SceneProvider( *model ) )
    {
    }

    /** \brief  Releases the Carna model and context.
      */
    virtual ~CarnaContextProvider();


    virtual Carna::base::view::SceneProvider& scene() const override
    {
        return *myScene;
    }

    virtual Carna::base::model::Scene& model() const override
    {
        return *myModel;
    }


private:

    const std::unique_ptr< Carna::base::model::Scene > myModel;

    const std::unique_ptr< Carna::base::view::SceneProvider > myScene;

};
