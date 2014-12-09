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
#include <Carna/base/Transformation.h>
#include <vector>



// ----------------------------------------------------------------------------------
// ClippedVolumeMask
// ----------------------------------------------------------------------------------

class ClippedVolumeMask : public BinaryVolumeMask
{

public:

    struct Line
    {

        Carna::base::Vector support, way;
        double radius;

        Line( const Carna::base::Vector& support, const Carna::base::Vector& way, double radius );

    };  // Line

    class Setup
    {

        NON_COPYABLE

        std::vector< Line > lines;

    public:

        Setup( const Carna::base::model::Scene&, int min, int max );

        ~Setup();

        const Carna::base::model::Scene& model;
        const int min;
        const int max;

        void clearLines();

        void addLine( const Line& );

        void visitLines( const std::function< bool( const Line& ) >& ) const;

    }; // Setup

 // ----------------------------------------------------------------------------------

    ClippedVolumeMask( const std::function< bool( Carna::base::Vector3ui& ) >& accept, const Setup& );

    ClippedVolumeMask();

    virtual ClippedVolumeMask& operator=( const ClippedVolumeMask& );

 // ----------------------------------------------------------------------------------

    virtual bool test( unsigned int x, unsigned int y, unsigned int z ) const override;

 // ----------------------------------------------------------------------------------

private:

    const Setup* setup;

    std::function< bool( Carna::base::Vector3ui& ) > accept;

}; // ClippedVolumeMask
