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

#include "CarnaModelFactory.h"
#include "FileChooser.h"
#include "Importer.h"
#include "BinaryDumpProcessor.h"
#include <Carna/DicomController.h>
#include <Carna/ModelFactory.h>
#include <Carna/SeriesLoadingRequest.h>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMetaType>



// ----------------------------------------------------------------------------------
// CarnaModelFactory
// ----------------------------------------------------------------------------------

CarnaModelFactory::CarnaModelFactory( Record::Server& server, QWidget* parent )
    : QWidget( parent )
    , tabs( new QTabWidget() )
    , importer( new Importer( server, this ) )
    , importFileChooser( new FileChooser() )
{
    this->setLayout( new QVBoxLayout() );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );
    this->layout()->addWidget( tabs );

    tabs->setDocumentMode( true );

 // ----------------------------------------------------------------------------------

    Carna::DicomController* const dicomController = new Carna::DicomController();
    tabs->addTab( dicomController, "DICOM" );

    connect( dicomController, SIGNAL( seriesLoadingRequested( const SeriesLoadingRequest& ) ), this, SLOT( disptach( const SeriesLoadingRequest& ) ) );

 // ----------------------------------------------------------------------------------

    importer->install( new BinaryDumpProcessor() );

    importFileChooser->setAcceptMode( QFileDialog::AcceptOpen );
    importFileChooser->setFileMode( QFileDialog::ExistingFile );

    importFileChooser->setNameFilters( importer->getFilters() );

    connect( importFileChooser, SIGNAL( rejected() ), this, SLOT( closeImporter() ) );
    connect( importFileChooser, SIGNAL( accepted() ), this, SLOT( import() ) );

    tabs->addTab( importFileChooser, "Import" );
}


void CarnaModelFactory::disptach( const Carna::SeriesLoadingRequest& request )
{
    Carna::ModelFactory factory( this );
    Carna::Model* const model = factory.createFromRequest( request );

    if( model != nullptr )
    {
        emit created( model );
    }
}


void CarnaModelFactory::closeImporter()
{
    tabs->setCurrentIndex( 0 );
}


void CarnaModelFactory::import()
{
    const QStringList fileNames = importFileChooser->selectedFiles();
    if( fileNames.size() != 1 )
    {
        return;
    }

    const QString& fileName = fileNames[ 0 ];
    importer->setFile( new QFile( fileName ) );
    Carna::Model* const model = importer->run();

    if( model != nullptr )
    {
        emit created( model );
    }
}
