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

/** \file   EmbeddablePlacer.h
  * \brief  Defines EmbeddablePlacer, NullEmbeddablePlacer, SingleEmbeddablePlacer.
  */

#include <set>
#include <Carna/base/Singleton.h>

class GriddedEmbedArea;
class Embeddable;



// ----------------------------------------------------------------------------------
// EmbeddablePlacer
// ----------------------------------------------------------------------------------

/** \brief  Interface used for positioning Embeddable instances.
  *
  * \see    This class is utilized by the \ref ComponentWindowFactory.
  *
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class EmbeddablePlacer
{

public:

    /** \brief  Does nothing.
      */
    virtual ~EmbeddablePlacer()
    {
    }


    typedef std::set<GriddedEmbedArea*> GriddedEmbedAreaSet;


    /** \brief  Places the specified Embeddable.
      *
      * Placing, in this context, implies, beside positioning, eventual docking. An
      * aggregation of all known EmbedArea instances, along with their position, is
      * supplied in order to allow the implementations of this interface to take an
      * appropriate decision.
      */
    virtual void place( Embeddable&, const GriddedEmbedAreaSet& ) = 0;

}; // EmbeddablePlacer



// ----------------------------------------------------------------------------------
// NullEmbeddablePlacer
// ----------------------------------------------------------------------------------

/** \brief  Null-implementation of the EmbedablePlacer interface.
  *
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class NullEmbeddablePlacer : public EmbeddablePlacer
                           , public Carna::base::Singleton< NullEmbeddablePlacer >
{

public:

    /** \brief  Does nothing.
      */
    virtual void place( Embeddable&, const GriddedEmbedAreaSet& )
    {
    }

}; // NullEmbeddablePlacer



// ----------------------------------------------------------------------------------
// SingleEmbeddablePlacer
// ----------------------------------------------------------------------------------

/** \brief  EmbedablePlacer implementation for single-window components.
  *
  * Puts the Embeddable into an EmbedArea if there is only one and it's not occupied.
  * Else, no positioning will be performed.
  *
  * \author Leonid Kostrykin
  * \date   4.2.2011
  */
class SingleEmbeddablePlacer : public EmbeddablePlacer
                             , public Carna::base::Singleton< SingleEmbeddablePlacer >
{

public:

    /** \brief  Eventually puts the Embeddable into a EmbedArea.
      */
    virtual void place( Embeddable&, const GriddedEmbedAreaSet& );

}; // SingleEmbeddablePlacer
