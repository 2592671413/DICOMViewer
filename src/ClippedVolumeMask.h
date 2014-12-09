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

#include "BinaryVolumeMask.h"
#include <Carna/Transformation.h>
#include <vector>



// ----------------------------------------------------------------------------------
// ClippedVolumeMask
// ----------------------------------------------------------------------------------

class ClippedVolumeMask : public BinaryVolumeMask
{

public:

    struct Line
    {

        Carna::Tools::Vector support, way;
        double radius;

        Line( const Carna::Tools::Vector& support, const Carna::Tools::Vector& way, double radius );

    };  // Line

    class Setup
    {

        NON_COPYABLE

        std::vector< Line > lines;

    public:

        Setup( const Carna::Model&, int min, int max );

        ~Setup();

        const Carna::Model& model;
        const int min;
        const int max;

        void clearLines();

        void addLine( const Line& );

        void visitLines( const std::function< bool( const Line& ) >& ) const;

    }; // Setup

 // ----------------------------------------------------------------------------------

    ClippedVolumeMask( const std::function< bool( Carna::Tools::Vector3ui& ) >& accept, const Setup& );

    ClippedVolumeMask();

    virtual ClippedVolumeMask& operator=( const ClippedVolumeMask& );

 // ----------------------------------------------------------------------------------

    virtual bool test( unsigned int x, unsigned int y, unsigned int z ) const override;

 // ----------------------------------------------------------------------------------

private:

    const Setup* setup;

    std::function< bool( Carna::Tools::Vector3ui& ) > accept;

}; // ClippedVolumeMask
