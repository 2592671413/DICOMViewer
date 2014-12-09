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
#include <Carna/Carna.h>
#include <Carna/base/noncopyable.h>
#include <QFile>
#include <set>

class ImportProcessor;

class QStringList;



// ----------------------------------------------------------------------------------
// Importer
// ----------------------------------------------------------------------------------

/** \brief  Allows the user to import volume data.
  *
  * \author Leonid Kostrykin
  * \date   27.7.12
  */
class Importer : public QObject
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      */
    Importer( Record::Server& server, QWidget* parent = nullptr );

    ~Importer();


    void install( ImportProcessor* );

    Carna::base::model::Scene* run();


    QFile& file()
    {
        return *processedFile;
    }


    QWidget* const parent;

    Record::Server& server;


    QStringList getFilters() const;


public slots:

    void setFile( QFile* );

    void reset();


private:

    std::unique_ptr< QFile > processedFile;

    std::set< ImportProcessor* > processors;
    
}; // Importer
