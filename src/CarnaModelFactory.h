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
#include <Carna/dicom/SeriesLoadingRequest.h>
#include <QWidget>

class Importer;
class FileChooser;

class QTabWidget;



// ----------------------------------------------------------------------------------
// CarnaModelFactory
// ----------------------------------------------------------------------------------

class CarnaModelFactory : public QWidget
{

    Q_OBJECT

public:

    CarnaModelFactory( Record::Server& server, QWidget* parent = nullptr );


signals:

    void created( Carna::base::model::Scene* );


private:

    QTabWidget* const tabs;

    std::unique_ptr< Importer > importer;

    FileChooser* const importFileChooser;


private slots:

    void dispatch( const Carna::dicom::SeriesLoadingRequest& );
    void closeImporter();
    void import();
    
}; // CarnaModelFactory
