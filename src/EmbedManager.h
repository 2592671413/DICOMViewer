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

/** \file   EmbedManager.h
  * \brief  Defines EmbedManager.
  */

#include <set>
#include <Carna/base/Singleton.h>

class EmbedArea;



// ----------------------------------------------------------------------------------
// EmbedManager
// ----------------------------------------------------------------------------------

/** \brief  Singleton which lists embed-areas within an application.
  *
  * Supplies an EmbedArea aggregation. Every EmbedArea registers upon the instance of
  * this class when it becomes visible and unregisters when it gets hidden.
  *
  * \see    EmbedArea, Embedable
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class EmbedManager : public Carna::base::Singleton< EmbedManager >
{

public:

    /** \brief  Set of embed areas
      */
    typedef std::set< EmbedArea* > EmbedAreaSet;


private:

    /** \brief  Holds registered embed areas.
      */
    EmbedAreaSet myEmbedAreas;


public:

    /** \brief  Only relevant to EmbedArea.
      *
      * Inserts the specified instance into the aggregation.
      */
    void registerEmbedArea( EmbedArea& ea )
    {
        myEmbedAreas.insert( &ea );
    }

    /** \brief  Only relevant to EmbedArea.
      *
      * Removes the specified instance from the aggregation.
      */
    void unregisterEmbedArea( EmbedArea& ea )
    {
        myEmbedAreas.erase( &ea );
    }

    /** \brief  References the EmbedArea aggregation.
      */
    const EmbedAreaSet& embedAreas() const
    {
        return myEmbedAreas;
    }

}; // EmbedManager
