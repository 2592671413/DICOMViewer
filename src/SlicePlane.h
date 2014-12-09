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

#include "RegistredComponent.h"



// ----------------------------------------------------------------------------------
// SlicePlane
// ----------------------------------------------------------------------------------

/** \brief  Provides any custom slice plane view.
  *
  * Provides a Carna SlicePlaneView.
  *
  * \see    \ref Component
  * \author Leonid Kostrykin
  * \date   26.5.11
  */
class SlicePlane : public RegistredComponent< SlicePlane >
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    SlicePlane( Record::Server& server, ComponentWindowFactory& );

}; // SlicePlane
