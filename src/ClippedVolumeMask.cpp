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

#include "ClippedVolumeMask.h"
#include <Carna/Position.h>
#include <Carna/Model.h>



// ----------------------------------------------------------------------------------
// ClippedVolumeMask :: Line
// ----------------------------------------------------------------------------------

ClippedVolumeMask::Line::Line( const Carna::Tools::Vector& support, const Carna::Tools::Vector& way, double radius )
    : support( support )
    , way( way )
    , radius( radius )
{
}



// ----------------------------------------------------------------------------------
// ClippedVolumeMask :: Setup
// ----------------------------------------------------------------------------------

ClippedVolumeMask::Setup::Setup( const Carna::Model& model, int min, int max )
    : model( model )
    , min( min )
    , max( max )
{
}


ClippedVolumeMask::Setup::~Setup()
{
}


void ClippedVolumeMask::Setup::clearLines()
{
    lines.clear();
}


void ClippedVolumeMask::Setup::addLine( const Line& line )
{
    lines.push_back( line );
}


void ClippedVolumeMask::Setup::visitLines( const std::function< bool( const Line& ) >& visit ) const
{
    for( auto line_itr = lines.begin(); line_itr != lines.end(); ++line_itr )
    {
        if( !visit( *line_itr ) )
        {
            break;
        }
    }
}



// ----------------------------------------------------------------------------------
// ClippedVolumeMask
// ----------------------------------------------------------------------------------

ClippedVolumeMask::ClippedVolumeMask( const std::function< bool( Carna::Tools::Vector3ui& ) >& accept, const Setup& setup )
    : BinaryVolumeMask( setup.min, setup.max, setup.model.volume() )
    , setup( &setup )
    , accept( accept )
{
}


ClippedVolumeMask::ClippedVolumeMask()
    : BinaryVolumeMask()
    , setup( nullptr )
{
}


ClippedVolumeMask& ClippedVolumeMask::operator=( const ClippedVolumeMask& other )
{
    BinaryVolumeMask::operator=( other );
    this->setup = other.setup;
    this->accept = other.accept;
    return *this;
}


bool ClippedVolumeMask::test( unsigned int x, unsigned int y, unsigned int z ) const
{
    if( !accept( Carna::Tools::Vector3ui( x, y, z ) ) )
    {
        return false;
    }

    const Carna::Tools::Vector probed_point = Carna::Position::fromVolumeUnits
        ( setup->model
        , x / static_cast< double >( setup->model.volume().size.x - 1 )
        , y / static_cast< double >( setup->model.volume().size.y - 1 )
        , z / static_cast< double >( setup->model.volume().size.z - 1 ) ).toMillimeters();

    double distance_to_closest_line = std::numeric_limits< double >::infinity();
    const Line* closest_line = nullptr;
    setup->visitLines( [&]( const Line& line )->bool
        {
            const Carna::Tools::Vector probed_point_to_support = line.support - probed_point;

         // project 'probed_point_to_support' on to 'line.way'

            const Carna::Tools::Vector projected_probed_point_to_way =
                ( probed_point_to_support.dot( line.way ) / line.way.dot( line.way ) ) * line.way;

         // compute whether 'projected_probed_point_to_way' is reverse to 'line.way'

            const bool reverse_direction = projected_probed_point_to_way.dot( line.way ) < 0;

         // compute distance to line

            /*   support                 way
             *      o ------------------------------------------> o
             *     /               |                               \
             *    /                |                                \
             *  (1)               (2)                               (3)
             */

            double min_distance_to_line;
            if( reverse_direction ) //< case (1)
            {
                min_distance_to_line = probed_point_to_support.norm();
            }
            else
            {
                if( projected_probed_point_to_way.dot( projected_probed_point_to_way )
                    <= line.way.dot( line.way ) ) //< case (2)
                {
                    min_distance_to_line = ( probed_point_to_support + projected_probed_point_to_way ).norm();
                }
                else //< case (3)
                {
                    min_distance_to_line = ( probed_point_to_support + line.way ).norm();
                }
            }

            if( min_distance_to_line < distance_to_closest_line )
            {
                distance_to_closest_line = min_distance_to_line;
                closest_line = &line;

                if( distance_to_closest_line < closest_line->radius )
                {
                    return false;
                }
            }

            return true;
        }
    );

    if( closest_line == nullptr )
    {
        return false;
    }
    else
    {
        return distance_to_closest_line < closest_line->radius && BinaryVolumeMask::test( x, y, z );
    }
}
