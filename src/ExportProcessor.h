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

#include <Carna/noncopyable.h>
#include <string>

class Exporter;



// ----------------------------------------------------------------------------------
// ExportProcessor
// ----------------------------------------------------------------------------------

/** \brief	Exports data to a certain file format.
  *
  * \author Leonid Kostrykin
  * \date   27.7.12
  */
class ExportProcessor
{

    NON_COPYABLE

public:

    virtual ~ExportProcessor()
    {
    }


    virtual const std::string& description() = 0;

    virtual const std::string& pattern() = 0;


    virtual void doExport( Exporter& ) = 0;
    
}; // ExportProcessor
