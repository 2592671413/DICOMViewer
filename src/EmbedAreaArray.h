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

/** \file   EmbedAreaArray.h
  * \brief  Defines EmbedAreaArray, GriddedEmbedArea.
  */

#include "EmbedArea.h"
#include <Carna/noncopyable.h>



// ----------------------------------------------------------------------------------
// EmbedAreaArray
// ----------------------------------------------------------------------------------

/** \brief  Groups embed-areas in a matrix-like array.
  *
  * Composes \f$ n \geq 1 \f$ GriddedEmbedArea instances.
  *
  * \see    EmbedArea, GriddedEmbedArea
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class EmbedAreaArray : public QWidget
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      */
    EmbedAreaArray( QWidget* parent = NULL );


    /** \brief  Specifies number and layout of areas to offer.
      *
      * Method releases dropped EmbedArea instances and acquires new ones.
      */
    void setDimensions( unsigned int rows, unsigned int cols );


    /** \brief  Counts the number of areas shown in horizontal direction.
      */
    unsigned int getRows() const
    {
        return rows;
    }

    /** \brief  Counts the number of areas shown in vertical direction.
      */
    unsigned int getCols() const
    {
        return cols;
    }


private:

    /** \property rows
      * \brief  Describes row count of this array.
      *
      * \property cols
      * \brief  Describes column count of this array.
      */
    unsigned int rows, cols;

}; // EmbedAreaArray



// ----------------------------------------------------------------------------------
// GriddedEmbedArea
// ----------------------------------------------------------------------------------

/** \brief  Member of a certain EmbedAreaArray.
  *
  * Associates an EmbedArea with an EmbedAreaArray and a pair of coordinates in
  * \f$\mathbb{N}^2\f$.
  *
  * \see    EmbedAreaArray, EmbedArea
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class GriddedEmbedArea : public EmbedArea
{

public:

    /** \brief  Instantiates.
      *
      * \param  x       position of this embed area inside the parent embed area array
      * \param  y       position of this embed area inside the parent embed area array
      * \param  parent  references the embed area array this embed area belongs to
      */
    GriddedEmbedArea( unsigned int x, unsigned int y, EmbedAreaArray* parent )
        : EmbedArea( parent )
        , x( y )
        , y( x )
        , parentArray( *parent )
    {
    }


    /** \brief  First coordinate in \f$\mathbb{N}^2\f$
      */
    const unsigned int x;
    
    /** \brief  Second coordinate in \f$\mathbb{N}^2\f$
      */
    const unsigned int y;

    /** \brief  The EmbedAreaArray this is a component of.
      */
    EmbedAreaArray& parentArray;

}; // GriddedEmbedArea




