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

/** \file   WindowingController.h
  * \brief  Defines WindowingController.
  */

#include <QWidget>
#include "CarnaContextClient.h"

class QSlider;
class QLabel;



// ----------------------------------------------------------------------------------
// WindowingController
// ----------------------------------------------------------------------------------

/** \brief  Defines widget for windowing configuration.
  *
  * \image  html    windowing_config.png
  *
  * %Windowing needs to be applied to each image, what might take a long time.
  * Therefore multi threading is used to accomplish this operation in a lesser amount
  * of time.
  *
  * \author Leonid Kostrykin
  * \date   2010-2012
  */
class WindowingController : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    WindowingController( Record::Server& server );

    /** \brief	Does nothing.
      */
    virtual ~WindowingController();


private slots:

    /** \brief	Sets new windowing level.
      */
    void setWindowingLevel( int );
    
    /** \brief	Sets new windowing width.
      */
    void setWindowingWidth( int );


private:

    /** \brief	Accesses the record service's Carna facet.
      */
    CarnaContextClient carna;


    /** \brief  References the slider which configures windowing level.
      */
    QSlider* const slWindowLevel;

    /** \brief  References the slider which configures windowing width.
      */
    QSlider* const slWindowWidth;
    

    /** \brief  Displays current windowing level.
      */
    QLabel* const laWindowLevel;

    /** \brief  Displays current windowing width.
      */
    QLabel* const laWindowWidth;

}; // WindowingController
