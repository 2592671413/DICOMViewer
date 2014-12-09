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

#include <QWidget>

class HistogramView;

class QLabel;



// ----------------------------------------------------------------------------------
// HistogramController
// ----------------------------------------------------------------------------------

/** \brief  Configures some \ref HistogramView instance.
  *
  * \image  html    HistogramController.png
  *
  * \see    Histogram
  * \author Leonid Kostrykin
  * \date   20.7.12
  */
class HistogramController : public QWidget
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      *
      * \param  view    is the histogram to be configured by this controller.
      */
    HistogramController( HistogramView& view );


private:

    /** \brief	References the histogram to be configured by this controller.
      */
    HistogramView& view;


    /** \brief	Shows how many HU-values are currently being displayed per pixel
      *         within the \ref view.
      */
    QLabel* const laValuesPerPixel;
    
    /** \brief	Shows the class size currently used by the \ref view.
      */
    QLabel* const laClassSize;
    
    /** \brief	Shows the bar width currently used by the \ref view.
      */
    QLabel* const laBarWidth;


private slots:

    /** \brief	Updates the \ref view "view's" scale function.
      *
      * \param  id  either \c 0 for linear scale or \c 1 for logarithmic scale.
      */
    void updateFunction( int id );

    /** \brief	Updates \ref laValuesPerPixel, \ref laClassSize and \ref laBarWidth.
      */
    void updateParametersView();

}; // HistogramController
