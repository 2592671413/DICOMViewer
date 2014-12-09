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

#include "BinaryDumpProcessor.h"
#include "BinaryDumpExportDialog.h"
#include "BinaryDumpImportDialog.h"
#include "Exporter.h"
#include "Importer.h"
#include "DataSize.h"
#include "CarnaContextClient.h"
#include "OptimizedVolumeDecorator.h"
#include <Carna/Model.h>
#include <Carna/ModelFactory.h>
#include <Carna/UInt16Volume.h>
#include <QDialog>
#include <QProgressDialog>
#include <functional>
#include <cstdint>



// ----------------------------------------------------------------------------------
// dumpSlice
// ----------------------------------------------------------------------------------

template< typename VoxelType >
static void dumpSlice( QDataStream& out, const Carna::Volume& volume, unsigned int z )
{
    for( unsigned int y = 0; y < volume.size.y; ++y )
    for( unsigned int x = 0; x < volume.size.x; ++x )
    {
        const auto huv = volume( x, y, z );
        out << static_cast< VoxelType >( huv );
    }
}



// ----------------------------------------------------------------------------------
// createVoxelReader
// ----------------------------------------------------------------------------------

template< typename VoxelType >
static std::function< Carna::UInt16Volume::VoxelType( QDataStream& ) > createVoxelReader()
{
    return []( QDataStream& in )->Carna::UInt16Volume::VoxelType
    {
        VoxelType huv;
        in >> huv;
        return static_cast< Carna::UInt16Volume::VoxelType >( huv + 1024 ) << 4;
    };
}



// ----------------------------------------------------------------------------------
// BinaryDumpProcessor
// ----------------------------------------------------------------------------------

const std::string& BinaryDumpProcessor::description()
{
    const static std::string description = "Binary dump";
    return description;
}


const std::string& BinaryDumpProcessor::pattern()
{
    const static std::string pattern = "*.bin";
    return pattern;
}


void BinaryDumpProcessor::doExport( Exporter& exporter )
{
    ExportDialog export_settings( exporter.parent );
    if( export_settings.exec() != QDialog::Accepted )
    {
        return;
    }

 // prepare

    QFile& file = exporter.file();
    file.open( QIODevice::WriteOnly | QIODevice::Truncate );
    QDataStream out( &file );

    const Carna::Model& model = CarnaContextClient( exporter.server ).model();
    const Carna::Volume& volume = model.volume();

 // compute dump size

    const unsigned long dump_size_in_bytes = [&]()->unsigned long
    {
        const unsigned long voxels_count = volume.size.x * volume.size.y * volume.size.z;
        
        switch( export_settings.voxelFormat.value() )
        {

            case IntegerFormatChooser::native16bit:
            {
                return voxels_count * 2;
            }

            case IntegerFormatChooser::native32bit:
            {
                return voxels_count * 4;
            }

            default:
                throw std::logic_error( "Unsupported integer format." );

        }
    }();

 // write meta data (optionally)

    if( export_settings.isMetaDataExportEnabled() )
    {
        out << static_cast< uint16_t >( volume.size.x );
        out << static_cast< uint16_t >( volume.size.y );
        out << static_cast< uint16_t >( volume.size.z );

        out << static_cast< double >( model.spacingX() );
        out << static_cast< double >( model.spacingY() );
        out << static_cast< double >( model.spacingZ() );
    }

 // dump data

    QProgressDialog progress( "Writing " + DataSize( dump_size_in_bytes ) + "...", "Abort", 0, volume.size.z - 1, exporter.parent );
    progress.setWindowTitle( "Export Volume" );
    progress.setWindowModality( Qt::WindowModal );
    progress.show();

    for( unsigned int z = 0; z < volume.size.z; ++z )
    {
        if( progress.wasCanceled() )
        {
            break;
        }

     // write line-wise

        switch( export_settings.voxelFormat.value() )
        {

            case IntegerFormatChooser::native16bit:
            {
                dumpSlice< uint16_t >( out, volume, z );
                break;
            }

            case IntegerFormatChooser::native32bit:
            {
                dumpSlice< uint32_t >( out, volume, z );
                break;
            }

            default:
                throw std::logic_error( "Unsupported integer format." );

        }

        progress.setValue( z );
    }

    file.close();
}


Carna::Model* BinaryDumpProcessor::doImport( Importer& importer )
{
    ImportDialog import_settings( importer.parent );
    if( import_settings.exec() != QDialog::Accepted )
    {
        return nullptr;
    }

    // prepare

    QFile& file = importer.file();
    file.open( QIODevice::ReadOnly );
    QDataStream in( &file );

    uint16_t width, height, depth;
    double spacingX, spacingY, spacingZ;
    unsigned int data_offset;

 // determine meta data

    if( import_settings.isMetaDataHeaderEnabled() )
    {
        in >> width >> height >> depth;
        in >> spacingX >> spacingY >> spacingZ;

        data_offset = sizeof( uint16_t ) * 3 + sizeof( double ) * 3;
    }
    else
    {
        width  = import_settings.getDataSetWidth();
        height = import_settings.getDataSetHeight();
        depth  = import_settings.getDataSetDepth();

        spacingX = import_settings.getSpacingX();
        spacingY = import_settings.getSpacingY();
        spacingZ = import_settings.getSpacingZ();

        data_offset = 0;
    }

    if( width == 0 || height == 0 || depth == 0 )
    {
        throw std::runtime_error( "Dataset dimensions are invalid." );
    }
    if( spacingX <= 0 || spacingY <= 0 || spacingZ <= 0 )
    {
        throw std::runtime_error( "Spacings are invalid." );
    }

 // compute dump size

    const unsigned long data_size_in_bytes = [&]()->unsigned long
    {
        const unsigned long voxels_count = width * height * depth;

        switch( import_settings.voxelFormat.value() )
        {

            case IntegerFormatChooser::native16bit:
            {
                return voxels_count * 2;
            }

            case IntegerFormatChooser::native32bit:
            {
                return voxels_count * 4;
            }

            default:
                throw std::logic_error( "Unsupported integer format." );

        }
    }();

    if( data_size_in_bytes + data_offset > file.size() )
    {
        throw std::runtime_error( "Binary dump smaller than expected." );
    }

 // read data

    const static unsigned int PROGRESS_STEPS = 1000;
    QProgressDialog status( "Reading " + DataSize( data_size_in_bytes ) + "...", "Abort", 0, PROGRESS_STEPS - 1, importer.parent );
    status.setWindowTitle( "Import Volume" );
    status.setWindowModality( Qt::WindowModal );
    status.show();

    const Carna::Tools::Vector3ui size( width, height, depth );

    Carna::UInt16Volume::BufferType* const buffer = new Carna::UInt16Volume::BufferType( size.x * size.y * size.z );

    Carna::UInt16Volume* const volume = new Carna::UInt16Volume( size, new Carna::Tools::Composition< Carna::UInt16Volume::BufferType >( buffer ) );

    std::function< Carna::UInt16Volume::VoxelType( QDataStream& ) > readVoxel;
    switch( import_settings.voxelFormat.value() )
    {

        case IntegerFormatChooser::native16bit:
        {
            readVoxel = createVoxelReader< uint16_t >();
            break;
        }

        case IntegerFormatChooser::native32bit:
        {
            readVoxel = createVoxelReader< uint32_t >();
            break;
        }

        default:
            throw std::logic_error( "Unsupported integer format." );

    }

    unsigned int progress = 0;
    unsigned int progress_step = buffer->size() / PROGRESS_STEPS;
    for( unsigned long i = 0; i < buffer->size(); ++i )
    {
        if( status.wasCanceled() )
        {
            break;
        }

        const Carna::UInt16Volume::VoxelType voxel = readVoxel( in );

        ( *buffer )[ i ] = voxel;

        if( ( i + 1 ) % progress_step == 0 )
        {
            status.setValue( ++progress );
        }
    }

    /*
    OptimizedVolumeDecorator* optimizedVolume = new OptimizedVolumeDecorator
        ( new Carna::Tools::Composition< const Carna::Volume >( volume )
        , spacingX
        , spacingY
        , spacingZ );
        
    Carna::Model* const model = new Carna::Model
        ( new Carna::Tools::Composition< Carna::Volume >( optimizedVolume )
        , spacingX
        , spacingY
        , spacingZ );
        */

    Carna::Model* const model = new Carna::Model
        ( new Carna::Tools::Composition< Carna::Volume >( volume )
        , spacingX
        , spacingY
        , spacingZ );

    model->setRecommendedVoidThreshold( Carna::ModelFactory::computeVoidThreshold( *volume ) );

    file.close();

    return model;
}
