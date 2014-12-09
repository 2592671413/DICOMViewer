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
#include <QFile>
#include <set>

class ExportProcessor;



// ----------------------------------------------------------------------------------
// Exporter
// ----------------------------------------------------------------------------------

/** \brief	Allows the user to export the volume data.
  *
  * \author Leonid Kostrykin
  * \date   27.7.12
  */
class Exporter
{

    NON_COPYABLE

public:

    /** \brief	Instantiates.
      */
    Exporter( Record::Server& server, QWidget* parent = nullptr );

    ~Exporter();


    void install( ExportProcessor* );

    void run();


    QFile& file()
    {
        return *processedFile;
    }


    QWidget* const parent;

    Record::Server& server;


private:

    std::unique_ptr< QFile > processedFile;

    std::set< ExportProcessor* > processors;
    
}; // Exporter
