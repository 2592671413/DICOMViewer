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

#include "Exporter.h"
#include "ExportProcessor.h"
#include <QFileDialog>
#include <QRegExp>
#include <sstream>



// ----------------------------------------------------------------------------------
// Exporter
// ----------------------------------------------------------------------------------

Exporter::Exporter( Record::Server& server, QWidget* parent )
    : server( server )
    , parent( parent )
{
}


Exporter::~Exporter()
{
    std::for_each( processors.begin(), processors.end(), []( ExportProcessor* processor ){ delete processor; } );
}


void Exporter::install( ExportProcessor* processor )
{
    processors.insert( processor );
}


void Exporter::run()
{
    if( processors.empty() )
    {
        return;
    }

    std::stringstream filters;

    for( auto it = processors.begin(); it != processors.end(); ++it )
    {
        ExportProcessor& processor = ( **it );
        if( it != processors.begin() )
        {
            filters << ";;";
        }
        filters << processor.description() << " (" << processor.pattern() << ")";
    }

    const QString fileName
        = QFileDialog::getSaveFileName( parent
                                      , "Export Volume"
                                      , ""
                                      , QString::fromStdString( filters.str() )
                                      , 0
                                      , QFileDialog::DontResolveSymlinks
                                      | QFileDialog::HideNameFilterDetails );

    if( fileName.isEmpty() )
    {
        return;
    }

    this->processedFile.reset( new QFile( fileName ) );

    for( auto it = processors.begin(); it != processors.end(); ++it )
    {
        ExportProcessor& processor = ( **it );

        QRegExp rx( QString::fromStdString( processor.pattern() ) );
        rx.setPatternSyntax( QRegExp::Wildcard );
        if( rx.exactMatch( fileName ) )
        {
            processor.doExport( *this );
            return;
        }
    }

    this->processedFile.reset();

    throw std::runtime_error( "Unsupported file format." );
}
