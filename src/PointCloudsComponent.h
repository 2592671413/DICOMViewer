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

/** \file   PointCloudsComponent.h
  * \brief  Defines PointCloudsComponent.
  */

#include "Server.h"
#include "Component.h"



// ----------------------------------------------------------------------------------
// PointCloudsComponent
// ----------------------------------------------------------------------------------

/** \brief	\ref Component which allows the user to manage \ref PointCloud "point clouds".
  *
  * Provides a \ref ::PointCloudsController "PointCloudsController":
  *
  * \image  html    PointCloudsController.png
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class PointCloudsComponent : public Component
{

public:

    /** \brief  Instantiates.
      */
    PointCloudsComponent( Record::Server& server, ComponentWindowFactory& );

    /** \brief  Does nothing.
      */
    virtual ~PointCloudsComponent();

}; // PointCloudsComponent
