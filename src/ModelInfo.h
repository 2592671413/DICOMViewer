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

/** \file   ModelInfo.h
  * \brief  Defines ModelInfo, ModelInfoWidget
  */

#include "Component.h"

class QLabel;



// ----------------------------------------------------------------------------------
// ModelInfoWidget
// ----------------------------------------------------------------------------------

/** \brief  Defines user-interface widget for ModelInfo.
  *
  * \author Leonid Kostrykin
  * \date   17.2.2011
  */
class ModelInfoWidget : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    ModelInfoWidget( Record::Server& server );

    /** \brief  Does nothing.
      */
    virtual ~ModelInfoWidget();


private:

    /** \brief	References the record service.
      */
    Record::Server& server;

}; // ModelInfoWidget



// ----------------------------------------------------------------------------------
// ModelInfo
// ----------------------------------------------------------------------------------

/** \brief  Defines UI component which displays general model information.
  *
  * Provides a ModelInfoWidget.
  *
  * \author Leonid Kostrykin
  * \date   17.2.2011
  */
class ModelInfo : public Component
{

public:

    /** \brief  Instantiates.
      */
    ModelInfo( Record::Server& server, ComponentWindowFactory& );

    /** \brief  Does nothing.
      */
    virtual ~ModelInfo();

}; // ModelInfo
