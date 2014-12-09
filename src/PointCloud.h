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
#include <Carna/noncopyable.h>
#include <Carna/Transformation.h>
#include <Carna/Position.h>
#include <vector>
#include <QObject>



// ----------------------------------------------------------------------------------
// PointCloud
// ----------------------------------------------------------------------------------

/** \brief	Defines a point cloud.
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 31.5.12
  */
class PointCloud : public QObject
{

    Q_OBJECT

    NON_COPYABLE
    
public:

    /** \brief	Lists units which may be used to specify the point positions.
      */
    enum Unit
    {
        volumeUnits,
        millimeters
    };

    /** \brief  Lists possible sources of a point cloud.
      */
    enum Domain
    {

        trackingSide,
        dataSide,
        unknown
    };


    /** \brief	Defines point data type.
      */
    typedef Carna::Tools::Vector Point;

    /** \brief	Defines list of Point instances.
      */
    typedef std::vector< Point > PointList;


    /** \brief	Instantiates new point cloud.
      *
      * Communicates this point cloud as a new one to the rest of the application.
      */
    PointCloud( Record::Server&, Unit format, Domain source = unknown, const std::string& name = createNextDefaultName() );
    
    /** \brief	Instantiates new point cloud.
      *
      * Communicates this point cloud as a new one to the rest of the application.
      */
    PointCloud( const PointCloud&, const std::string& name = createNextDefaultName() );

    /** \brief	Releases references to this point cloud.
      *
      * Communicates this point cloud as an obsolete one to the rest of the application.
      */
    virtual ~PointCloud();


    /** \brief  Holds where this point cloud is from.
      */
    const Domain source;


    /** \brief	References internal list of Point instances.
      */
    PointList& getList();
    
    /** \brief	References internal list of Point instances.
      */
    const PointList& getList() const;

    /** \brief  Tells the position of some point.
      */
    Carna::Position getPoint( unsigned int index ) const;


    /** \brief	Tells the name of this point cloud.
      */
    const std::string& getName() const
    {
        return name;
    }

    /** \brief	Tells the units which the point positions are specified in.
      */
    Unit getFormat() const
    {
        return format;
    }


    /** \brief	Creates and tells some default name for a point cloud.
      */
    static const std::string createNextDefaultName();


    /** \brief	Converts the \ref format of this point cloud to the given units.
      */
    void convert( Unit newUnit, QWidget* modalParent = nullptr );


public slots:

    /** \brief	Renames this point cloud.
      */
    void rename( const std::string& name )
    {
        this->name = name;

        emit renamed();
        emit renamed( *this );
    }


private:

    /** \brief	References the record service.
      */
    Record::Server& server;

    /** \brief	Holds the points stored by this point cloud.
      */
    PointList list;

    /** \brief	Holds the name of this point cloud.
      */
    std::string name;
    
    /** \brief	Holds the units which the point positions are specified in.
      */
    Unit format;


signals:

    /** \brief	This point cloud has been renamed.
      */
    void renamed();
    
    /** \brief	This point cloud has been renamed.
      */
    void renamed( PointCloud& );

};
