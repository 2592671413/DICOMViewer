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

#include "Server.h"
#include <QWidget>
#include <functional>

class QPaintEvent;



// ----------------------------------------------------------------------------------
// HistogramView
// ----------------------------------------------------------------------------------

/** \brief  Renders a histogram of the associated dataset.
  *
  * \image  html    HistogramView.png
  *
  * This view can easily be tweaked by using a \ref HistogramController.
  *
  * \see    Histogram
  * \author Leonid Kostrykin
  * \date   19.7.12 - 26.7.12
  */
class HistogramView : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    HistogramView( Record::Server& server );


    /** \brief	Defines \f$\mathbb R \to \mathbb R\f$ scale function.
      */
    typedef std::function< double( double ) > Scale;


    /** \brief	Holds the \f$x \mapsto x \f$ function.
      */
    const static Scale linear;

    /** \brief	Holds the \f$x \mapsto \log(1+x) \f$ function.
      */
    const static Scale logarithmic;


    /** \brief	Sets the scale function.
      */
    void setScale( const Scale& );


    /** \brief	Tells \f$\sum\limits_{x,y,z} \mathrm{huv}(x,y,z)\f$
      */
    unsigned long getSum() const
    {
        return sum;
    }

    /** \brief	Tells the configured minimal class size.
      *
      * The actually used class size is computed automatically.
      * It is greater or equal to this value.
      */
    unsigned int getMinClassSize() const
    {
        return minClassSize;
    }
    
    /** \brief	Tells the configured minimal bar width.
      *
      * The actually used bar width is computed automatically.
      * It is greater or equal to this value.
      */
    unsigned int getMinBarWidth() const
    {
        return minBarWidth;
    }

    /** \brief	Tells how many HU-values are currently being displayed per pixel.
      *
      * This value is determined by the following parameters:
      * - this widget's width
      * - the currently configured minimal class size
      * - the currently configured minimal bar width
      * - the currently configured first HUV
      * - the currently configured last HUV
      */
    double getValuesPerPixel() const
    {
        return valuesPerPixel;
    }

    /** \brief	Tells the currently used class size.
      *
      * \see    \ref getMinClassSize
      * \see    \ref setMinClassSize
      */
    unsigned int getClassSize() const
    {
        return classSize;
    }
    
    /** \brief	Tells the currently used bar width.
      *
      * \see    \ref getMinBarWidth
      * \see    \ref setMinBarWidth
      */
    unsigned int getBarWidth() const
    {
        return barWidth;
    }

    /** \brief	Tells the first HUV shown within the histogram.
      *
      * \see    \ref setMinHuv
      */
    int getMinHuv() const
    {
        return huv0;
    }
    
    /** \brief	Tells the last HUV shown within the histogram.
      *
      * \see    \ref setMaxHuv
      */
    int getMaxHuv() const
    {
        return huv1;
    }

    /** \brief	Tells the zoom of the ordinate.
      *
      * This is a linear zoom factor.
      *
      * \see    \ref setZoom
      */
    double getZoom() const
    {
        return zoom;
    }


public slots:

    void setMinClassSize( int );

    void setMinBarWidth( int );

    void setMinHuv( int );

    void setMaxHuv( int );

    void setZoom( double );

    void setAutoHuvRange();

    void setAutoZoom();


signals:

    void parametersChanged();

    void minHuvChanged( int );

    void maxHuvChanged( int );

    void zoomChanged( double );


protected:

    virtual void paintEvent( QPaintEvent* );

    virtual void resizeEvent( QResizeEvent* );


private:

    Record::Server& server;

    std::vector< unsigned long > histogram;

    unsigned long sum;

    bool initialized;


    Scale scale;

    unsigned int minClassSize;

    unsigned int minBarWidth;

    double zoom;


    int huv0;

    int huv1;


    double valuesPerPixel;

    unsigned int classSize;

    unsigned int barWidth;


    void compute( bool forceComputation = false );


private slots:

    void init();

}; // HistogramView
