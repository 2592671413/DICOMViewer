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

#include "PointCloud.h"
#include "PointCloudsClient.h"
#include "CarnaContextClient.h"
#include <Carna/Position.h>
#include <sstream>
#include <QApplication>
#include <QProgressDialog>



// ----------------------------------------------------------------------------------
// PointCloud
// ----------------------------------------------------------------------------------

PointCloud::PointCloud( Record::Server& server, Unit format, Domain source, const std::string& name )
    : source( source )
    , server( server )
    , name( name )
    , format( format )
{
    PointCloudsClient( server ).takePointCloud( this );
}


PointCloud::PointCloud( const PointCloud& original, const std::string& name )
    : source( original.source )
    , server( original.server )
    , list( original.list )
    , name( name )
    , format( original.format )
{
    PointCloudsClient( server ).takePointCloud( this );
}


PointCloud::~PointCloud()
{
    PointCloudsClient( server ).removePointCloud( *this );

    QApplication::setOverrideCursor( Qt::WaitCursor );

    list.clear();

    QApplication::restoreOverrideCursor();
}


const PointCloud::PointList& PointCloud::getList() const
{
    return list;
}


Carna::Position PointCloud::getPoint( unsigned int index ) const
{
    Carna::Model& model = CarnaContextClient( server ).model();
    switch( getFormat() )
    {

        case PointCloud::millimeters:
        {
            return Carna::Position::fromMillimeters( model, list[ index ] );
        }

        case PointCloud::volumeUnits:
        {
            return Carna::Position::fromVolumeUnits( model, list[ index ] );
        }

        default:
        {
            throw std::logic_error( "unsupported unit" );
        }

    }
}


PointCloud::PointList& PointCloud::getList()
{
    return list;
}


const std::string PointCloud::createNextDefaultName()
{
    static unsigned int nextIndex = 1;

    std::stringstream ss;
    ss << "Point Cloud " << ( nextIndex++ );

    return ss.str();
}


void PointCloud::convert( PointCloud::Unit newUnit, QWidget* modalParent )
{
    if( newUnit == this->getFormat() )
    {
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QProgressDialog progress( "Converting point cloud...", "", 0, list.size() - 1, modalParent );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );
    Carna::Model& model = CarnaContextClient( server ).model();

    Carna::Position position( model );
    for( auto it = list.begin(); it != list.end(); ++it )
    {
        Point& point = *it;
        if( this->getFormat() == volumeUnits )
        {
            position.setVolumeUnits( point );
            point = position.toMillimeters();
        }
        else
        {
            position.setMillimeters( point );
            point = position.toVolumeUnits();
        }

        progress.setValue( it - list.begin() );
    }

    this->format = newUnit;

    QApplication::restoreOverrideCursor();
}
