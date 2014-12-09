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
#include <Carna/Carna.h>
#include <string>

class Importer;



// ----------------------------------------------------------------------------------
// ImportProcessor
// ----------------------------------------------------------------------------------

/** \brief	Imports data from a certain file format.
  *
  * \author Leonid Kostrykin
  * \date   27.7.12
  */
class ImportProcessor
{

    NON_COPYABLE

public:

    virtual ~ImportProcessor()
    {
    }


    virtual const std::string& description() = 0;

    virtual const std::string& pattern() = 0;


    virtual Carna::Model* doImport( Importer& ) = 0;
    
}; // ImportProcessor
