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

#include "Importer.h"
#include "ImportProcessor.h"
#include <QFileDialog>
#include <QRegExp>
#include <sstream>



// ----------------------------------------------------------------------------------
// Importer
// ----------------------------------------------------------------------------------

Importer::Importer( Record::Server& server, QWidget* parent )
    : server( server )
    , parent( parent )
{
}


Importer::~Importer()
{
    std::for_each( processors.begin(), processors.end(), []( ImportProcessor* processor ){ delete processor; } );
}


void Importer::install( ImportProcessor* processor )
{
    processors.insert( processor );
}


void Importer::reset()
{
    processedFile.reset();
}


void Importer::setFile( QFile* file )
{
    processedFile.reset( file );
}


QStringList Importer::getFilters() const
{
    QStringList filters;

    for( auto it = processors.begin(); it != processors.end(); ++it )
    {
        ImportProcessor& processor = ( **it );

        QString filter = QString::fromStdString( processor.description() )
                       + " ("
                       + QString::fromStdString( processor.pattern() )
                       + ")";

        filters << filter;
    }

    return filters;
}


Carna::base::model::Scene* Importer::run()
{
    if( processors.empty() )
    {
        return nullptr;
    }

    if( processedFile.get() == nullptr )
    {
        const QString fileName
            = QFileDialog::getOpenFileName( parent
                                          , "Import Volume"
                                          , ""
                                          , getFilters().join( ";;" )
                                          , 0
                                          , QFileDialog::DontResolveSymlinks
                                          | QFileDialog::HideNameFilterDetails );

        if( fileName.isEmpty() )
        {
            return nullptr;
        }

        this->processedFile.reset( new QFile( fileName ) );
    }

    const QString fileName = processedFile->fileName();

    for( auto it = processors.begin(); it != processors.end(); ++it )
    {
        ImportProcessor& processor = ( **it );

        QRegExp rx( QString::fromStdString( processor.pattern() ) );
        rx.setPatternSyntax( QRegExp::Wildcard );
        if( rx.exactMatch( fileName ) )
        {
            return processor.doImport( *this );;
        }
    }

    this->processedFile.reset();

    throw std::runtime_error( "Unsupported file format." );
}
