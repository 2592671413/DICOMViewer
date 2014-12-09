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

#include "SurfaceExtractionDialog.h"
#include "SurfaceExtraction.h"
#include "CarnaContextClient.h"
#include <Carna/base/qt/Object3DChooser.h>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressDialog>
#include <QMessageBox>
#include <QApplication>
#include <QFuture>
#include <QtConcurrentRun>
#include <QFutureWatcher>



// ----------------------------------------------------------------------------------
// SurfaceExtractionDialog
// ----------------------------------------------------------------------------------

SurfaceExtractionDialog::SurfaceExtractionDialog( Record::Server& server, QWidget* parent )
    : QWidget( parent )
    , server( server )
    , seedPointSelector( new Carna::base::qt::Object3DChooser( CarnaContextClient( server ).model() ) )
    , sbHuv0( new QSpinBox() )
    , sbHuv1( new QSpinBox() )
{
    QFormLayout* form = new QFormLayout();
    this->setLayout( form );

    form->addRow( "Seed point:", seedPointSelector );

    form->addRow( "HUV minimum:", sbHuv0 );
    form->addRow( "HUV maximum:", sbHuv1 );

    const static int MIN_HUV = -1024;
    const static int MAX_HUV =  3071;

    sbHuv0->setMinimum( MIN_HUV );
    sbHuv0->setMaximum( MAX_HUV );
    sbHuv0->setValue  (       0 );

    sbHuv1->setMinimum( MIN_HUV );
    sbHuv1->setMaximum( MAX_HUV );
    sbHuv1->setValue  ( MAX_HUV );

    QPushButton* const buExtract = new QPushButton( "Extract surface" );
    connect( buExtract, SIGNAL( clicked() ), this, SLOT( run() ) );
    form->addRow( buExtract );
}


SurfaceExtractionDialog::~SurfaceExtractionDialog()
{
}


void SurfaceExtractionDialog::run()
{
    if( !seedPointSelector->isObject3DSelected() )
    {
        QMessageBox::critical( this, "Surface Extraction", "No seed point selected." );
        return;
    }
    const Carna::base::model::Object3D& seedPoint = seedPointSelector->selectedObject3D();

    const int huv0 = sbHuv0->value();
    const int huv1 = sbHuv1->value();

    if( huv1 <= huv0 )
    {
        QMessageBox::critical( this, "Surface Extraction", "The minimal HU value must be lesser than the maximal HU value." );
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QProgressDialog status( "", "", 0, 0, this );
    status.setWindowModality( Qt::WindowModal );
    status.setCancelButton( nullptr );
    status.setWindowTitle( "Surface Extraction" );

 // perform segmentation

    status.setLabelText( "Performing segmentation..." );

    std::stringstream errors;
    std::function< Segmentation*() > computeSegmentation = [&]()->Segmentation*
    {
        try
        {
            return new Segmentation( CarnaContextClient( server ).model(), seedPoint, huv0, huv1 );
        }
        catch( const std::bad_alloc& ex )
        {
            errors << "Not enough memory to finish operation.";
            const std::string ex_msg( ex.what() );
            if( !ex_msg.empty() )
            {
                errors << std::endl << std::endl << ex_msg;
            }
            throw;
        }
        catch( const std::exception& ex )
        {
            errors << ex.what();
            throw;
        }
    };
    QFuture< Segmentation* > segmentation = QtConcurrent::run( computeSegmentation );
    try
    {
        QFutureWatcher< Segmentation* > watcher;
        watcher.setFuture( segmentation );
        connect( &watcher, SIGNAL( finished() ), &status, SLOT( reset() ) );
        status.exec();

     // perform surface extraction

        status.setLabelText( "Performing surface extraction..." );

        SurfaceExtraction surface( status, server, segmentation.result()->getMask() );

     // finish

        delete segmentation.result();

        QApplication::restoreOverrideCursor();

        std::stringstream ss;
        ss << "Point cloud '" << surface.getPointCloud().getName() << "' has been created.";

        if( surface.getPointCloud().getList().empty() )
        {
            ss << std::endl << std::endl << "The point cloud is empty.";

            QMessageBox::warning( this, "Surface Extraction", QString::fromStdString( ss.str() ) );
        }
        else
        {
            ss << std::endl << std::endl << "The point cloud consists of ";
            ss << surface.getPointCloud().getList().size() << " points.";

            QMessageBox::information( this, "Surface Extraction", QString::fromStdString( ss.str() ) );
        }

        CARNA_ASSERT( errors.str().empty() );
    }
    catch( const QtConcurrent::UnhandledException& )
    {
        const std::string errors_str = errors.str();
        if( errors_str.empty() )
        {
            throw;
        }
        else
        {
            throw std::runtime_error( errors_str );
        }
    }
}
