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

#include "PointCloud.h"
#include "PointClouds.h"
#include <Carna/base/CarnaException.h>
#include <set>
#include <algorithm>
#include <QObject>



// ----------------------------------------------------------------------------------
// PointCloudsProvider
// ----------------------------------------------------------------------------------

/** \brief	Defines the point clouds service provider.
  *
  * \see    \ref Record::Service
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class PointCloudsProvider : public QObject, public Record::Provider< PointClouds >
{

    Q_OBJECT

public:
    
    /** \copydoc Record::Provider::Provider
      */
    PointCloudsProvider( Record::Server& server )
        : Provider( server )
    {
    }
    
    /** \copydoc Record::Provider::~Provider
      *
      * Releases all point clouds.
      */
    virtual ~PointCloudsProvider()
    {
        while( !pointClouds.empty() )
        {
            delete pointClouds.back();
        }
    }


    virtual const PointCloudsList& getPointClouds() const override
    {
        return pointClouds;
    }

    virtual void connectPointCloudAdded( QObject* receiver, const char* slot ) override
    {
        connect( this, SIGNAL( pointCloudAdded( PointCloud& ) ), receiver, slot );
    }

    virtual void connectPointCloudRemoved( QObject* receiver, const char* slot ) override
    {
        connect( this, SIGNAL( pointCloudRemoved( PointCloud& ) ), receiver, slot );
    }

    virtual void takePointCloud( PointCloud* pointCloud ) override
    {
        CARNA_ASSERT( std::find( pointClouds.begin(), pointClouds.end(), pointCloud ) == pointClouds.end() );

        pointClouds.push_back( pointCloud );

        emit pointCloudAdded( *pointCloud );
    }

    virtual void removePointCloud( PointCloud& pointCloud ) override
    {
        PointCloudsList::iterator it = std::find( pointClouds.begin(), pointClouds.end(), &pointCloud );

        CARNA_ASSERT( it != pointClouds.end() );
        CARNA_ASSERT( std::find( it + 1, pointClouds.end(), &pointCloud ) == pointClouds.end() );

        pointClouds.erase( it );

        emit pointCloudRemoved( pointCloud );
    }


private:

    /** \brief	Holds all point clouds.
      */
    PointCloudsList pointClouds;


signals:

    /** \brief	New PointCloud has been created.
      */
    void pointCloudAdded( PointCloud& pointCloud );
    
    /** \brief	PointCloud has been released.
      */
    void pointCloudRemoved( PointCloud& pointCloud );

}; // PointCloudsProvider
