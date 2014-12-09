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
// Histogram
// ----------------------------------------------------------------------------------

/** \brief  Provides a histogram of the loaded dataset.
  *
  * Provides instances of \ref HistogramView and \ref HistogramController:
  *
  * \image  html    Histogram.png
  *
  * \see    \ref Component
  * \author Leonid Kostrykin
  * \date   19.7.12
  */
class Histogram : public RegistredComponent< Histogram >
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    Histogram( Record::Server& server, ComponentWindowFactory& );

}; // Histogram
