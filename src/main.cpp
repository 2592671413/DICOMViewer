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

#include "MainWindow.h"
#include <QFileInfo>
#include <Carna/base/Application.h>
#include <Carna/Version.h>
#include <Carna/base/view/SceneProvider.h>
#include <Carna/base/model/SceneFactory.h>
#include <sstream>
#include <QMessageBox>
#include <QDir>

// ----------------------------------------------------------------------------------

CARNA_ASSERT_API_VERSION( 2, 5, 1 )

// ----------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------
// DicomViewer
// ----------------------------------------------------------------------------------

class DicomViewer : public Carna::base::Application
{

public:

    DicomViewer( int argc, char** argv )
        : Carna::base::Application( argc, argv )
    {
        setApplicationName( "DICOM Viewer" );

        QDir working_directory = QDir::current();
        if( working_directory.cd( "res" ) )
        {
            QDir::setCurrent( working_directory.path() );
        }
        else
        {
            QMessageBox::warning( 0, "Filesystem", "Working directory could not be changed to 'res' sub directory." );
        }
    }
    

    virtual bool notify( QObject* receiver, QEvent* ev ) override
    {
        try
        {
            return Carna::base::Application::notify( receiver, ev );
        }
        catch( const std::bad_alloc& )
        {
            QMessageBox::critical( 0, "Fatal Error", "Not enough memory to finish operation." );
        }
        catch( const std::exception& ex )
        {
            QMessageBox::critical( 0, "Fatal Error", QString::fromStdString( ex.what() ) );
        }
        catch( ... )
        {
            QMessageBox::critical( 0, "Fatal Error", "Unknown error" );
        }
        return false;
    }

}; // DicomViewer


// ----------------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------------

int main( int argc, char** argv )
{
    DicomViewer app( argc, argv );

    // check max 3D texture size

    const unsigned int max3dTextureSize = Carna::base::view::SceneProvider::max3DTextureSize();
    const unsigned int max3dTextureSizeMin = 512;

    if( max3dTextureSize < max3dTextureSizeMin )
    {
        std::stringstream ss;

        ss << "Your system is running with low hardware capabilities. The maximal 3D texture size is "
           << max3dTextureSize << ", at least " << max3dTextureSizeMin << " is recommended.\n\n"
           << "If your system does have multiple GPU devices, assure that you are running "
           << QFileInfo( QCoreApplication::applicationFilePath() ).fileName().toStdString()
           << " with the right one, e.g. not the on-board implemented.";

        QMessageBox::warning( 0, "Low Hardware Capabilities", QString::fromStdString( ss.str() ) );
    }

    // run application

    MainWindow w;

    w.show();

    return app.exec();
}
