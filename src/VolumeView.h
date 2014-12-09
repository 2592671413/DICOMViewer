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
#include <Carna/Carna.h>

class ToolChooser;



// ----------------------------------------------------------------------------------
// VolumeView
// ----------------------------------------------------------------------------------

/** \brief  Provides \c Carna::VolumeView and \c Carna::VolumeController instances.
  *
  * \see    \ref Component
  * \author Leonid Kostrykin
  * \date   8.8.11
  */
class VolumeView : public RegistredComponent< VolumeView >
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    VolumeView( Record::Server& server, ComponentWindowFactory& factory );


private:

    Carna::VolumeVisualization* const view;

}; // VolumeView
