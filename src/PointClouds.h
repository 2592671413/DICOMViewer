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

class PointCloud;



// ----------------------------------------------------------------------------------
// PointClouds
// ----------------------------------------------------------------------------------

/** \brief	Defines the point clouds service.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class PointClouds : public Record::GenericService< PointClouds >
{

public:
    
    /** \brief	Defines list of PointCloud objects.
      */
    typedef std::deque< PointCloud* > PointCloudsList;


    /** \brief	References list of all point clouds.
      */
    virtual const PointCloudsList& getPointClouds() const = 0;

    /** \brief	Connects the <code>PointCloudsProvider::pointCloudAdded( PointCloud& )</code> signal.
      */
    virtual void connectPointCloudAdded( QObject* receiver, const char* slot ) = 0;

    /** \brief	Connects the <code>PointCloudsProvider::pointCloudRemoved( PointCloud& )</code> signal.
      */
    virtual void connectPointCloudRemoved( QObject* receiver, const char* slot ) = 0;

    /** \brief	Communicates the given point cloud as a new one to the rest of the application.
      */
    virtual void takePointCloud( PointCloud* ) = 0;
    
    /** \brief	Communicates the given point cloud as an obsolete one to the rest of the application.
      */
    virtual void removePointCloud( PointCloud& ) = 0;

}; // PointClouds


template< >
const std::string Record::GenericService< PointClouds >::serviceID = "Point Clouds";
