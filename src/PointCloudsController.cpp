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

#include "PointCloudsController.h"
#include "PointCloudsComponent.h"
#include "PointCloudsClient.h"
#include "PointCloud.h"
#include "EmbeddablePlacer.h"
#include "PointCloud3D.h"
#include "CarnaContextClient.h"
#include "PointCloudComposer.h"
#include "SurfaceExtractionDialog.h"
#include "DataSize.h"
#include <Carna/base/view/Point3D.h>
#include <QAction>
#include <QListWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QToolButton>
#include <QMenu>
#include <QInputDialog>
#include <QProgressDialog>
#include <QApplication>
#include <QXmlSimpleReader>
#include <QPushButton>
#include <QXmlStreamWriter>

#ifndef NO_CRA
#include "PointRecorder.h"
#endif



// ----------------------------------------------------------------------------------
// PointCloudListItem
// ----------------------------------------------------------------------------------

class PointCloudListItem : public QListWidgetItem
{

public:

    PointCloudListItem( PointCloud& pointCloud )
        : pointCloud( pointCloud )
    {
        setText( QString::fromStdString( pointCloud.getName() ) );
    }

    PointCloud& pointCloud;

}; // PointCloudListItem



// ----------------------------------------------------------------------------------
// PointCloudsListWidget
// ----------------------------------------------------------------------------------

class PointCloudsListWidget : public QListWidget
{

public:

    PointCloudsListWidget( PointCloudsController& controller )
        : controller( controller )
    {
    }


protected:

    void mouseDoubleClickEvent( QMouseEvent* ev )
    {
        QListWidget::mouseDoubleClickEvent( ev );

        controller.showCloudDetails();
    }


private:

    PointCloudsController& controller;

}; // PointCloudsListWidget



// ----------------------------------------------------------------------------------
// PointCloudsController
// ----------------------------------------------------------------------------------

PointCloudsController::PointCloudsController( Record::Server& server, PointCloudsComponent& component )
    : QWidget()
    , server( server )
    , component( component )
    , pointCloudsListWidget( new PointCloudsListWidget( *this ) )
    , cloudSaving      ( new QAction( QIcon( ":/icons/save.png" )       , "&Save"   , this ) )
    , cloudLoading     ( new QAction( QIcon( ":/icons/load.png" )       , "&Load"   , this ) )
    , cloudReleasing   ( new QAction( QIcon( ":/icons/trash.png" )      , "&Release", this ) )
    , cloudRenaming    ( new QAction( QIcon( ":/icons/pencil.png" )     , "Rena&me" , this ) )
    , cloudDetails     ( new QAction( QIcon( ":/icons/information.png" ), "&Details", this ) )
#ifndef NO_CRA
    , cloudRecording   ( new QAction( "Re&cord"          , this ) )
#endif
    , cloudExtracting  ( new QAction( "Extract &Surface" , this ) )
    , cloudBuilding    ( new QAction( "From &3D Objects" , this ) )
    , object3dCreation ( new QAction( "Create 3D &Object", this ) )
    , point3dCreation  ( new QAction( "Create 3D &Points", this ) )
#ifndef NO_CRA
    , recorderWindow( nullptr )
#endif
    , cloudCreatorWindow( nullptr )
    , surfaceExtractionWindow( nullptr )
{
    updateListWidget();

    PointCloudsClient( server ).connectPointCloudAdded  ( this, SLOT(   processAddedPointCloud( PointCloud& ) ) );
    PointCloudsClient( server ).connectPointCloudRemoved( this, SLOT( processRemovedPointCloud( PointCloud& ) ) );

    connect( pointCloudsListWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( pointCloudSelectionChanged() ) );

    this->setLayout( new QVBoxLayout() );

    this->layout()->addWidget( pointCloudsListWidget );

    QToolBar* toolBar = new QToolBar();
    toolBar->setIconSize( QSize( 24, 24 ) );
    toolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addAction( cloudSaving );
    toolBar->addAction( cloudLoading );
    toolBar->addAction( cloudRenaming );
    toolBar->addAction( cloudDetails );
    toolBar->addAction( cloudReleasing );
    toolBar->addSeparator();

    QToolButton* acquireButton = new QToolButton( toolBar );
    QMenu* acquireMenu = new QMenu( acquireButton );
    acquireButton->setMenu( acquireMenu );
    acquireButton->setPopupMode( QToolButton::InstantPopup );
    acquireButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    acquireButton->setText( "&New" );
    acquireButton->setIcon( QIcon( ":/icons/add.png" ) );

#ifndef NO_CRA
    acquireMenu->addAction( cloudRecording );
#endif
    acquireMenu->addAction( cloudExtracting );
    acquireMenu->addAction( cloudBuilding );
    acquireMenu->addSeparator();
    acquireMenu->addAction( object3dCreation );
    acquireMenu->addAction( point3dCreation );

    toolBar->addWidget( acquireButton );

    this->layout()->addWidget( toolBar );
    this->setMinimumWidth( toolBar->sizeHint().width() + acquireButton->sizeHint().width() );

    connect( cloudSaving     , SIGNAL( triggered() ), this, SLOT(      savePointCloud() ) );
    connect( cloudLoading    , SIGNAL( triggered() ), this, SLOT(      loadPointCloud() ) );
    connect( cloudReleasing  , SIGNAL( triggered() ), this, SLOT(   releasePointCloud() ) );
    connect( cloudRenaming   , SIGNAL( triggered() ), this, SLOT(    renamePointCloud() ) );
    connect( cloudDetails    , SIGNAL( triggered() ), this, SLOT(    showCloudDetails() ) );
#ifndef NO_CRA
    connect( cloudRecording  , SIGNAL( triggered() ), this, SLOT(    recordPointCloud() ) );
#endif
    connect( cloudExtracting , SIGNAL( triggered() ), this, SLOT(      extractSurface() ) );
    connect( cloudBuilding   , SIGNAL( triggered() ), this, SLOT( createFrom3dObjects() ) );
    connect( object3dCreation, SIGNAL( triggered() ), this, SLOT(      create3dObject() ) );
    connect( point3dCreation , SIGNAL( triggered() ), this, SLOT(      create3dPoints() ) );

    pointCloudSelectionChanged();
}


PointCloudsController::~PointCloudsController()
{
}


void PointCloudsController::processAddedPointCloud( PointCloud& )
{
    updateListWidget();
}


void PointCloudsController::processRemovedPointCloud( PointCloud& )
{
    updateListWidget();
}


void PointCloudsController::updateListWidget()
{
    const PointClouds::PointCloudsList& list = PointCloudsClient( server ).getPointClouds();

    pointCloudsListWidget->clear();

    for( auto it = list.begin(); it != list.end(); ++it )
    {
        PointCloud* const cloud = *it;
        PointCloudListItem* item = new PointCloudListItem( *cloud );
        pointCloudsListWidget->insertItem( pointCloudsListWidget->count(), item );

        connect( cloud, SIGNAL( renamed() ), this, SLOT( updateListWidget() ) );
    }
}


PointCloud* PointCloudsController::getSelectedPointCloud()
{
    if( pointCloudsListWidget->selectedItems().empty() )
    {
        QMessageBox::critical( this, "Point Clouds", "No point cloud selected." );
        return 0;
    }

    PointCloudListItem* const item = static_cast< PointCloudListItem* >( pointCloudsListWidget->selectedItems().at( 0 ) );
    PointCloud* const cloud = &( item->pointCloud );
    
    return cloud;
}


void PointCloudsController::savePointCloud()
{
    PointCloud* const cloud = getSelectedPointCloud();

    if( !cloud )
    {
        return;
    }

    const QString filename
        = QFileDialog::getSaveFileName
            ( this
            , "Save Point Cloud"
            , QString::fromStdString( cloud->getName() )
            , "XML files (*.xml);;Text dump (*.txt);;Binary dump (*.bin)"
            , 0
            , QFileDialog::DontResolveSymlinks
            | QFileDialog::HideNameFilterDetails );

    if( filename.isEmpty() )
    {
        return;
    }

    const signed int lastDotIndex = filename.lastIndexOf( "." );
    const QString fileExtension = ( lastDotIndex < 0 ? "" : filename.right( filename.length() - lastDotIndex - 1 ) ).toLower();

    QFile file( filename );
    if( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        QMessageBox::critical( this, "Save Point Cloud", "Failed opening file for writing." );
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QProgressDialog progress( "Writing points...", "", 0, cloud->getList().size() - 1, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );

    if( fileExtension == "xml" )
    {

        QString format_str;
        switch( cloud->getFormat() )
        {

            case PointCloud::millimeters:
                format_str = "millimeters";
                break;

            case PointCloud::volumeUnits:
                format_str = "volumeunits";
                break;

            default:
                throw std::logic_error( "unknown PointCloud::Unit format" );
        }

        QXmlStreamWriter xml( &file );
        xml.setAutoFormatting( true );
        xml.writeStartDocument();
        xml.writeStartElement( "cloud" );
        xml.writeAttribute( "name", QString::fromStdString( cloud->getName() ) );
        xml.writeAttribute( "format", format_str );

        switch( cloud->source )
        {
            
            case PointCloud::trackingSide:
            {
                xml.writeAttribute( "source", "tracking" );
                break;
            }
            
            case PointCloud::dataSide:
            {
                xml.writeAttribute( "source", "data" );
                break;
            }
            
            case PointCloud::unknown:
            default:
            {
                break;
            }

        }
        
        for( PointCloud::PointList::const_iterator it  = cloud->getList().begin();
                                                   it != cloud->getList().end();
                                                 ++it )
        {
            const PointCloud::Point& p = *it;

            xml.writeStartElement( "point" );
            xml.writeAttribute( "x", QString::number( p.x() ) );
            xml.writeAttribute( "y", QString::number( p.y() ) );
            xml.writeAttribute( "z", QString::number( p.z() ) );
            xml.writeEndElement();

            progress.setValue( it - cloud->getList().begin() );
        }

        xml.writeEndElement();
        xml.writeEndDocument();

    }
    else
    if( fileExtension == "txt" )
    {

        QTextStream stream( &file );
        for( PointCloud::PointList::const_iterator it  = cloud->getList().begin();
            it != cloud->getList().end();
            ++it )
        {
            const PointCloud::Point& p = *it;

            stream << p.x() << "\t" << p.y() << "\t" << p.z() << "\n";

            progress.setValue( it - cloud->getList().begin() );
        }

    }
    else
    if( fileExtension == "bin" )
    {

        QDataStream stream( &file );
        stream << QString::fromStdString( cloud->getName() );
        stream << ( cloud->getFormat() == PointCloud::volumeUnits );
        stream << static_cast< unsigned int >( cloud->getList().size() );
        for( PointCloud::PointList::const_iterator it  = cloud->getList().begin();
            it != cloud->getList().end();
            ++it )
        {
            const PointCloud::Point& p = *it;

            stream << p.x() << p.y() << p.z();

            progress.setValue( it - cloud->getList().begin() );
        }

    }
    else
    {
        QMessageBox::critical( this, "Saving Point Cloud", "Unknown file extension." );
    }

    file.close();

    QApplication::restoreOverrideCursor();
}


void PointCloudsController::loadPointCloud()
{
    const QStringList filenames
        = QFileDialog::getOpenFileNames
            ( this
            , "Load Point Cloud"
            , ""
            , "XML files and binary dumps (*.xml *.bin);;XML files (*.xml);;Binary dumps (*.bin)"
            , 0
            , QFileDialog::ReadOnly
            | QFileDialog::HideNameFilterDetails );

    for( auto filenameIterator = filenames.begin(); filenameIterator != filenames.end(); ++filenameIterator )
    {
        QString filename = *filenameIterator;

        if( filename.isEmpty() )
        {
            continue;;
        }

        QFile file( filename );
        if( !file.open( QIODevice::ReadOnly ) )
        {
            QMessageBox::critical( this, "Load Point Cloud", "Failed opening file for reading." );
            return;
        }

        const signed int lastDotIndex = filename.lastIndexOf( "." );
        const QString fileExtension = ( lastDotIndex < 0 ? "" : filename.right( filename.length() - lastDotIndex - 1 ) ).toLower();

        if( fileExtension == "xml" )
        {

            QXmlSimpleReader xmlReader;
            QXmlInputSource xmlSource( &file );

            class SaxParser : public QXmlDefaultHandler
            {

            public:

                SaxParser( Record::Server& server, const QFile& file, QWidget* modalParent )
                    : cloud( nullptr )
                    , server( server )
                    , file( file )
                    , progress( QFileInfo( file ).fileName(), "", 0, file.size(), modalParent )
                {
                    progress.setWindowTitle( "Reading points" );
                    progress.setWindowModality( Qt::WindowModal );
                    progress.setCancelButton( nullptr );
                }

                virtual bool startElement( const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attributes )
                {
                    if( localName == "cloud" )
                    {
                        if( cloud )
                        {
                            return false;
                        }

                        PointCloud::Unit format;
                        const QString& format_str = attributes.value( "format" );
                        if( format_str == "volumeunits" || format_str == "modelunits" )
                        {
                            format = PointCloud::volumeUnits;
                        }
                        else
                        if( format_str == "millimeters" )
                        {
                            format = PointCloud::millimeters;
                        }
                        else
                        {
                            return false;
                        }

                        const QString& source_str = attributes.value( "source" );
                        PointCloud::Domain source = PointCloud::unknown;
                        if( source_str == "tracking" )
                        {
                            source = PointCloud::trackingSide;
                        }
                        else
                        if( source_str == "data" )
                        {
                            source = PointCloud::dataSide;
                        }

                        const QString name = attributes.value( "name" );
                        cloud = new PointCloud( server, format, source, name.toStdString() );
                    }
                    else
                    if( localName == "point" )
                    {
                        if( !cloud )
                        {
                            return false;
                        }

                        const QString str_x = attributes.value( "x" );
                        const QString str_y = attributes.value( "y" );
                        const QString str_z = attributes.value( "z" );

                        bool ok;

                        const double x = str_x.toDouble( &ok );
                        if( !ok )
                        {
                            return false;
                        }

                        const double y = str_y.toDouble( &ok );
                        if( !ok )
                        {
                            return false;
                        }

                        const double z = str_z.toDouble( &ok );
                        if( !ok )
                        {
                            return false;
                        }

                        cloud->getList().push_back( PointCloud::Point( x, y, z ) );
                    }

                    return true;
                }

                virtual bool endElement( const QString& namespaceURI, const QString& localName, const QString& qName )
                {
                    if( localName == "cloud" )
                    {
                        if( !cloud )
                        {
                            return false;
                        }

                        cloud = nullptr;
                    }

                    progress.setValue( file.pos() );

                    return true;
                }

            private:

                PointCloud* cloud;

                Record::Server& server;

                const QFile& file;

                QProgressDialog progress;

            }; // Handler

            SaxParser parser( server, file, this );
            xmlReader.setContentHandler( &parser );
            xmlReader.setErrorHandler( &parser );

            QApplication::setOverrideCursor( Qt::WaitCursor );

            try
            {
                if( !xmlReader.parse( xmlSource ) )
                {
                    QMessageBox::critical( this, "Load Point Cloud", "Unexpected file format." );
                }
            }
            catch( const std::logic_error& ex )
            {
                std::stringstream ss;
                ss << "Logical Exception: " << ex.what();
                QMessageBox::critical( this, "Load Point Cloud", QString::fromStdString( ss.str() ) );
            }

            QApplication::restoreOverrideCursor();

        }
        else
        if( fileExtension == "bin" )
        {
            QDataStream stream( &file );

            QApplication::setOverrideCursor( Qt::WaitCursor );

            QString name;
            stream >> name;

            bool isModelUnits;
            stream >> isModelUnits;

            unsigned int count;
            stream >> count;

            PointCloud* const cloud = new PointCloud( server, isModelUnits ? PointCloud::volumeUnits : PointCloud::millimeters, PointCloud::unknown, name.toStdString() );

            QProgressDialog progress( QFileInfo( file ).fileName(), "Cancel", 0, count, this );
            progress.setWindowTitle( "Reading points" );
            progress.setWindowModality( Qt::WindowModal );

            cloud->getList().resize( count );

            for( unsigned int i = 0; i < count; ++i )
            {
                if( progress.wasCanceled() )
                {
                    cloud->getList().resize( i );
                    break;
                }

                double x, y, z;
            
                stream >> x;
                stream >> y;
                stream >> z;

                cloud->getList()[ i ] = PointCloud::Point( x, y, z );

                progress.setValue( i );
            }

            QApplication::restoreOverrideCursor();
        }
        else
        {
            QMessageBox::critical( this, "Saving Point Cloud", "Unknown file extension." );
        }
    }
}


void PointCloudsController::releasePointCloud()
{
    PointCloud* const cloud = getSelectedPointCloud();

    if( !cloud )
    {
        return;
    }

    delete cloud;
}


void PointCloudsController::renamePointCloud()
{
    PointCloud* const cloud = getSelectedPointCloud();

    if( !cloud )
    {
        return;
    }

    bool ok;

    QString text = QInputDialog::getText( this
                                        , "Point Clouds"
                                        , "Rename:", QLineEdit::Normal
                                        , QString::fromStdString( cloud->getName() ), &ok );

    if( ok && !text.isEmpty() )
    {
        cloud->rename( text.toStdString() );
    }
}


void PointCloudsController::showCloudDetails()
{
    PointCloud* const cloud = getSelectedPointCloud();

    if( !cloud )
    {
        return;
    }

 // create output dialog

    std::stringstream ss;
    ss << "Number of points: " << cloud->getList().size() << std::endl;
    ss << "Points format: " << ( cloud->getFormat() == PointCloud::millimeters ? "millimeters" : "model units" ) << std::endl;
    ss << "Payload size: " << std::string( DataSize( cloud->getList().size() * sizeof( double ) * 3 ) ) << std::endl;
    ss << "Source: ";

    switch( cloud->source )
    {

        case PointCloud::trackingSide:
        {
            ss << "tracking-side";
            break;
        }

        case PointCloud::dataSide:
        {
            ss << "data-side";
            break;
        }

        default:
        {
            ss << "unknown";
            break;
        }

    }

    QPushButton* const buRename = new QPushButton( "&Rename" );
    QPushButton* const buClose = new QPushButton( "Cl&ose" );
    QPushButton* const buConvert = new QPushButton( cloud->getFormat() == PointCloud::millimeters ? "&Convert to model units" : "&Convert to millimeters" );

    QMessageBox msgBox( this );
    msgBox.setWindowTitle( QString::fromStdString( cloud->getName() ) );
    msgBox.setText( QString::fromStdString( ss.str() ) );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.addButton( buConvert, QMessageBox::YesRole );
    msgBox.addButton( buRename, QMessageBox::YesRole );
    msgBox.addButton( buClose, QMessageBox::NoRole );
    msgBox.setDefaultButton( buRename );
    msgBox.setEscapeButton( buClose );
    msgBox.exec();

    if( msgBox.clickedButton() == buConvert )
    {
        if( cloud->getFormat() == PointCloud::millimeters )
        {
            cloud->convert( PointCloud::volumeUnits, this );
        }
        else
        {
            cloud->convert( PointCloud::millimeters, this );
        }
    }
    else
    if( msgBox.clickedButton() == buRename )
    {
        this->renamePointCloud();
    }
}


void PointCloudsController::create3dObject()
{
    PointCloud* const cloud = getSelectedPointCloud();

    if( !cloud )
    {
        return;
    }

    new PointCloud3D( server, *cloud, this );
}


void PointCloudsController::create3dPoints()
{
    PointCloud* const cloud = getSelectedPointCloud();

    if( !cloud )
    {
        return;
    }

    const static unsigned int pointNumberConfirmationThreshold = 100;

    if( cloud->getList().size() > pointNumberConfirmationThreshold )
    {
        if( QMessageBox::question
            (     this
                , "Point Cloud to Points"
                , QString::fromStdString( "You are trying to create one discrete visual 3D representation for each point from the point cloud. " )
                + QString::fromStdString( "This point cloud has more than " )
                + QString::number( pointNumberConfirmationThreshold )
                + QString::fromStdString( " points. Proceed?" )
                , QMessageBox::Yes | QMessageBox::No
                , QMessageBox::Yes
            ) == QMessageBox::No )
        {
            return;
        }
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    QProgressDialog progress( "Creating points...", "", 0, cloud->getList().size() - 1, this );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );

    for( PointCloud::PointList::const_iterator it  = cloud->getList().begin();
                                               it != cloud->getList().end();
                                             ++it )
    {
        const Carna::base::Vector& p = *it;

        Carna::base::model::Scene& model = CarnaContextClient( server ).model();

        std::auto_ptr< Carna::base::model::Position > pos;

        switch( cloud->getFormat() )
        {

        case PointCloud::millimeters:
            pos.reset( new Carna::base::model::Position( Carna::base::model::Position::fromMillimeters( model, p.x(), p.y(), p.z() ) ) );
            break;

        case PointCloud::volumeUnits:
            pos.reset( new Carna::base::model::Position( Carna::base::model::Position::fromVolumeUnits( model, p.x(), p.y(), p.z() ) ) );
            break;

        default:
            throw std::logic_error( "unknown PointCloud::Unit format" );

        }

        new Carna::base::view::Point3D( model, *pos );

        progress.setValue( it - cloud->getList().begin() );
    }

    QApplication::restoreOverrideCursor();
}


#ifndef NO_CRA

void PointCloudsController::recordPointCloud()
{
    if( !recorderWindow )
    {
        PointCloud* cloud = new PointCloud( server, PointCloud::millimeters );

        PointRecorder* const recorder = new PointRecorder( server, *cloud );

        std::stringstream ssSuffix;
        ssSuffix << ": " << cloud->getName() << " Recorder";

        recorderWindow = &component.createEmbeddable( recorder, SingleEmbeddablePlacer::instance(), QString::fromStdString( ssSuffix.str() ) );

        connect( recorderWindow, SIGNAL( destroyed() ), this, SLOT( pointRecorderClosed() ) );
    }
    else
    {
        recorderWindow->show();
        recorderWindow->raise();
    }

    cloudRecording->setEnabled( false );

    this->setEnabled( false );
}


void PointCloudsController::pointRecorderClosed()
{
    cloudRecording->setEnabled( true );
    recorderWindow = nullptr; // TODO: am I obsoleted?

    this->setEnabled( true );
}

#endif  // NO_CRA


void PointCloudsController::extractSurface()
{
    if( !surfaceExtractionWindow )
    {
        SurfaceExtractionDialog* dialog = new SurfaceExtractionDialog( server );

        surfaceExtractionWindow = &component.createEmbeddable( dialog, SingleEmbeddablePlacer::instance(), " Surface Extraction" );

        connect( surfaceExtractionWindow, SIGNAL( destroyed() ), this, SLOT( surfaceExtractorClosed() ) );
    }
    else
    {
        surfaceExtractionWindow->show();
        surfaceExtractionWindow->raise();
    }

    cloudExtracting->setEnabled( false );
}


void PointCloudsController::surfaceExtractorClosed()
{
    cloudExtracting->setEnabled( true );
#ifndef NO_CRA
    recorderWindow = nullptr; // TODO: am I obsoleted?
#endif
}


void PointCloudsController::createFrom3dObjects()
{
    if( !cloudCreatorWindow )
    {
        PointCloud* cloud = new PointCloud( server, PointCloud::volumeUnits );

        PointCloudComposer* const creator = new PointCloudComposer( server, *cloud );

        std::stringstream ssSuffix;
        ssSuffix << ": " << cloud->getName() << " from 3D objects";

        cloudCreatorWindow = &component.createEmbeddable( creator, SingleEmbeddablePlacer::instance(), QString::fromStdString( ssSuffix.str() ) );

        connect( cloudCreatorWindow, SIGNAL( destroyed() ), this, SLOT( cloudCreatorClosed() ) );
    }
    else
    {
        cloudCreatorWindow->show();
        cloudCreatorWindow->raise();
    }

    cloudBuilding->setEnabled( false );

    this->setEnabled( false );
}


void PointCloudsController::cloudCreatorClosed()
{
    cloudBuilding->setEnabled( true );
    cloudCreatorWindow = nullptr; // TODO: am I obsoleted?

    this->setEnabled( true );
}


void PointCloudsController::pointCloudSelectionChanged()
{
    const bool someCloudSelected = !pointCloudsListWidget->selectedItems().empty();

    cloudSaving->setEnabled( someCloudSelected );
    cloudReleasing->setEnabled( someCloudSelected );
    cloudRenaming->setEnabled( someCloudSelected );
    cloudDetails->setEnabled( someCloudSelected );
    object3dCreation->setEnabled( someCloudSelected );
    point3dCreation->setEnabled( someCloudSelected );
}
