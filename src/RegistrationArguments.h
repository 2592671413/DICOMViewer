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

#include <CRA/Tool.h>

class PointCloud;



// ----------------------------------------------------------------------------------
// RegistrationArguments
// ----------------------------------------------------------------------------------

/** \brief	Holds the arguments which need to be passed to every \ref RegistrationMethod.
  *
  * \date   4.6.12
  * \author Leonid Kostrykin
  */
class RegistrationArguments
{

public:

    /** \brief	Instantiates.
      */
    RegistrationArguments( const PointCloud& recordedPoints
                         , const PointCloud& virtualPoints
                         , CRA::Tool& referenceBase )
        : recordedPoints( recordedPoints )
        , virtualPoints( virtualPoints )
        , referenceBase( referenceBase )
    {
    }

    
    /** \brief	References the source point cloud.
      */
    const PointCloud& recordedPoints;
    
    /** \brief	References the target point cloud.
      */
    const PointCloud& virtualPoints;
    
    /** \brief	References the reference base.
      */
    CRA::Tool& referenceBase;

}; // RegistrationArguments
