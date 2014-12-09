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

#include "PointClouds.h"



// ----------------------------------------------------------------------------------
// PointCloudsClient
// ----------------------------------------------------------------------------------

/** \brief	Defines the point clouds service client.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class PointCloudsClient : public Record::Client< PointClouds >
{

public:
    
    /** \copydoc Record::Client::Client
      */
    PointCloudsClient( Record::Server& server )
        : Client( server )
    {
    }


    virtual const PointCloudsList& getPointClouds() const override
    {
        return destination.getPointClouds();
    }

    virtual void connectPointCloudAdded( QObject* receiver, const char* slot ) override
    {
        destination.connectPointCloudAdded( receiver, slot );
    }

    virtual void connectPointCloudRemoved( QObject* receiver, const char* slot ) override
    {
        destination.connectPointCloudRemoved( receiver, slot );
    }

    virtual void takePointCloud( PointCloud* pointCloud ) override
    {
        destination.takePointCloud( pointCloud );
    }

    virtual void removePointCloud( PointCloud& pointCloud ) override
    {
        destination.removePointCloud( pointCloud );
    }

}; // PointCloudsClient
