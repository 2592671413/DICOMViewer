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

#include "ExportProcessor.h"
#include "ImportProcessor.h"



// ----------------------------------------------------------------------------------
// BinaryDumpProcessor
// ----------------------------------------------------------------------------------

/** \brief	Processes \c *.bin exports and impots.
  *
  * \author Leonid Kostrykin
  * \date   26.7.12 - 27.7.12
  */
class BinaryDumpProcessor : public ExportProcessor, public ImportProcessor
{

public:

    virtual const std::string& description() override;

    virtual const std::string& pattern() override;


    virtual void doExport( Exporter& ) override;

    virtual Carna::Model* doImport( Importer& ) override;
    
}; // BinaryDumpProcessor
