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

#include "ObjectsController.h"
#include "ObjectsView.h"
#include "ObjectsComponent.h"
#include "Object3DEditor.h"
#include "Object3DEditorFactory.h"
#include "PointCloud3D.h"
#include "CarnaContextClient.h"
#include <Carna/base/model/Object3D.h>
#include <Carna/base/view/Point3D.h>
#include <Carna/base/view/Polyline.h>
#include <QApplication>
#include <QAction>
#include <QVBoxLayout>
#include <QToolBar>
#include <QInputDialog>
#include <QLineEdit>
#include <QFrame>
#include <QToolButton>
#include <QMenu>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>

#ifndef NO_CRA
#include "Pointer3D.h"
#endif



// ----------------------------------------------------------------------------------
// PolylineImportConfiguration
// ----------------------------------------------------------------------------------

class PolylineImportConfiguration
{
    
public:

    enum Unit
    {
        volumeUnits,
        millimeters
    };


    struct UnexpectedFormatException { };

    PolylineImportConfiguration( QFile& file );


    static PolylineImportConfiguration* createFromUserPrompt( QWidget* parent = nullptr );


    const QString& getFileName() const
    {
        return fileName;
    }

    const QRegExp& getVertexRegex() const
    {
        return vertexRegex;
    }

    Unit getUnits() const
    {
        return units;
    }

    float getVertexSize() const
    {
        return vertexSize;
    }

    float getLineWidth() const
    {
        return lineWidth;
    }

    bool hasBorder() const
    {
        return withBorder;
    }


private:

    Unit units;
    QString fileName;
    QRegExp vertexRegex;
    float vertexSize;
    float lineWidth;
    bool withBorder;

}; // PolylineImportConfiguration


PolylineImportConfiguration::PolylineImportConfiguration( QFile& file )
{
    QDomDocument dom;
    if( !dom.setContent( &file ) )
    {
        throw UnexpectedFormatException();
    }

    QDomElement root = dom.documentElement();
    if( root.tagName() != "PolylineImport" )
    {
        throw UnexpectedFormatException();
    }

    // read settings

    fileName = root.attribute( "file" );
    const QString vertexPattern = root.attribute( "vertex" );
    const QString space = root.attribute( "space" );
    vertexSize = std::max( 0.f, root.attribute( "vertexsize" ).toFloat() );
    lineWidth = std::max( 1.f, root.attribute( "width" ).toFloat() );
    withBorder = ( root.attribute( "border" ).toInt() > 0 );

    if( fileName.isNull() || vertexPattern.isNull() || space.isNull()
        || fileName.isEmpty() || vertexPattern.isEmpty() || space.isEmpty() )
    {
        throw UnexpectedFormatException();
    }

    QFileInfo dataFileInfo( fileName );
    if( !dataFileInfo.exists() && !dataFileInfo.isAbsolute() )
    {
        const QDir fileDir = QFileInfo( file ).absoluteDir();
        fileName = fileDir.filePath( fileName );
    }

    if( space == "model" )
    {
        units = millimeters;
    }
    else
    if( space == "volume" || space == "texture" )
    {
        units = volumeUnits;
    }
    else
    {
        throw UnexpectedFormatException();
    }

    vertexRegex = QRegExp( vertexPattern );
}


PolylineImportConfiguration* PolylineImportConfiguration::createFromUserPrompt( QWidget* const parent )
{
    const static QString dialogTitle = "Load Polyline Import Setup";

    const QString filename
        = QFileDialog::getOpenFileName
            ( parent
            , dialogTitle
            , ""
            , "XML-Files (*.xml)"
            , 0
            , QFileDialog::ReadOnly
            | QFileDialog::HideNameFilterDetails );

    if( filename.isEmpty() )
    {
        return nullptr;
    }

    QFile file( filename );
    if( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::critical( parent, dialogTitle, "Failed opening file for reading." );
        return nullptr;
    }

    try
    {
        return new PolylineImportConfiguration( file );
    }
    catch( const PolylineImportConfiguration::UnexpectedFormatException& )
    {
        QMessageBox::critical( parent, dialogTitle, "Illegal file format." );
        return nullptr;
    }
}



// ----------------------------------------------------------------------------------
// ObjectsController
// ----------------------------------------------------------------------------------

ObjectsController::ObjectsController( Record::Server& server, ObjectsComponent& component )
    : server( server )
    , component( component )
    , view( new ObjectsView( server ) )
    , objectReleasing( new QAction( QIcon( ":/icons/trash.png" ) , "&Release", this ) )
    , objectRenaming ( new QAction( QIcon( ":/icons/pencil.png" ), "Rena&me" , this ) )
    , editorDetaching( new QAction( QIcon( ":/icons/pin.png" )   , "&Detach", this ) )
    , pointCreation  ( new QAction( "Create &Point", this ) )
    , pointerCreation( new QAction( "Create Pointe&r", this ) )
    , polylineImport ( new QAction( "Import Poly&line", this ) )
    , tempPointCreation( new QAction( "Create Temporary Seed", this ) )
    , editorContainer( new QFrame() )
    , currentObjectEditor( nullptr )
{
    editorContainer->setLayout( new QVBoxLayout() );
    editorContainer->setContentsMargins( 0, 0, 0, 0 );
    editorContainer->setMinimumWidth( 200 );
    editorContainer->hide();

    QToolBar* const toolBar = new QToolBar();
    toolBar->setIconSize( QSize( 24, 24 ) );
    toolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    QToolButton* acquireButton = new QToolButton( toolBar );
    QMenu* acquireMenu = new QMenu( acquireButton );
    acquireButton->setMenu( acquireMenu );
    acquireButton->setPopupMode( QToolButton::InstantPopup );
    acquireButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    acquireButton->setText( "Cre&ate" );
    acquireButton->setIcon( QIcon( ":/icons/add.png" ) );

    acquireMenu->addAction( pointCreation );
    acquireMenu->addAction( pointerCreation );
    acquireMenu->addSeparator();
    acquireMenu->addAction( tempPointCreation );
    acquireMenu->addSeparator();
    acquireMenu->addAction( polylineImport );

    toolBar->addWidget( acquireButton );
    toolBar->addAction( objectReleasing );
    toolBar->addAction( objectRenaming );
    toolBar->addSeparator();
    toolBar->addAction( editorDetaching );

    connect( pointCreation, SIGNAL( triggered() ), this, SLOT( createPoint3D() ) );
    connect( polylineImport, SIGNAL( triggered() ), this, SLOT( importPolyline() ) );
    connect( tempPointCreation, SIGNAL( triggered() ), this, SLOT( createTempPoint1() ) );

#ifndef NO_CRA
    connect( pointerCreation, SIGNAL( triggered() ), this, SLOT( createPointer3D() ) );
#endif

    this->setMinimumWidth( toolBar->sizeHint().width() );
    this->setLayout( new QVBoxLayout() );
    this->layout()->addWidget( view );
    this->layout()->addWidget( toolBar );
    this->layout()->addWidget( editorContainer );

    connect( objectReleasing, SIGNAL( triggered() ), this, SLOT( releaseObjects() ) );
    connect( objectRenaming , SIGNAL( triggered() ), this, SLOT(   renameObject() ) );
    connect( editorDetaching, SIGNAL( triggered() ), this, SLOT(   detachEditor() ) );

    editorDetaching->setEnabled( false );

    connect( view, SIGNAL( selectionChanged() ), this, SLOT( objectsSelectionChanged() ) );
    connect( view, SIGNAL( objectDoubleClicked( Carna::base::model::Object3D& ) ), this, SLOT( renameObject( Carna::base::model::Object3D& ) ) );

    objectsSelectionChanged();
}


ObjectsController::~ObjectsController()
{
}


void ObjectsController::releaseObjects()
{
    std::vector< Carna::base::model::Object3D* > selectedObjects;
    view->fetchSelectedObjects( selectedObjects );
    view->selectNone();

    QApplication::setOverrideCursor( Qt::WaitCursor );

    for( auto it = selectedObjects.begin(); it != selectedObjects.end(); ++it )
    {
        Carna::base::model::Object3D* object = *it;
        delete object;
    }

    QApplication::restoreOverrideCursor();
}


void ObjectsController::renameObject()
{
    std::vector< Carna::base::model::Object3D* > selectedObjects;
    view->fetchSelectedObjects( selectedObjects );

    if( selectedObjects.size() == 1 )
    {
        renameObject( *selectedObjects[ 0 ] );
    }
}


void ObjectsController::renameObject( Carna::base::model::Object3D& object )
{
    bool ok;

    QString text = QInputDialog::getText
        ( this
        , "Point Clouds"
        , "Rename:"
        , QLineEdit::Normal
        , QString::fromStdString( object.name() )
        , &ok );

    if( ok && !text.isEmpty() )
    {
        object.setName( text.toStdString() );
    }
}


void ObjectsController::openObjectEditor()
{
    std::vector< Carna::base::model::Object3D* > selectedObjects;
    view->fetchSelectedObjects( selectedObjects );

    if( selectedObjects.empty() )
    {
        return;
    }

    openObjectEditor( *selectedObjects[ 0 ] );
}


void ObjectsController::closeObjectEditor()
{
    if( currentObjectEditor )
    {
        QLayoutItem* item;
        while( ( item = editorContainer->layout()->takeAt( 0 ) ) != nullptr )
        {
            delete item->widget();
            delete item;
        }
        currentObjectEditor = nullptr;
        editorContainer->hide();
        editorDetaching->setEnabled( false );
    }
}


void ObjectsController::openObjectEditor( Carna::base::model::Object3D& object )
{
    Object3DEditor* const editor = createObjectEditor( object );

    closeObjectEditor();

    editorContainer->layout()->addWidget( editor );
    currentObjectEditor = editor;
    editorContainer->show();
    editorDetaching->setEnabled( true );
}


void ObjectsController::detachEditor()
{
    if( currentObjectEditor )
    {
        Carna::base::model::Object3D& object = currentObjectEditor->editedObject;

        ComponentEmbeddable* const embeddable = &component.createEmbeddable
                ( currentObjectEditor
                , NullEmbeddablePlacer::instance()
                , QString::fromStdString( object.name() ) );

        connect( embeddable, SIGNAL( closed( Embeddable* ) ), this, SLOT( releaseEmbeddable( Embeddable* ) ) );

        connect( &object, SIGNAL( renamed( const std::string& ) ), embeddable, SLOT( setSuffix( const std::string& ) ) );
        connect( &object, SIGNAL( released() ), embeddable, SLOT( close() ) );

        currentObjectEditor = nullptr;
        editorContainer->hide();
        editorDetaching->setEnabled( false );
    }
}


Object3DEditor* ObjectsController::createObjectEditor( Carna::base::model::Object3D& object )
{
    Object3DEditorFactory::EditorType editorType;

    if( dynamic_cast< Carna::base::view::Point3D* >( &object ) )
    {
        editorType = Object3DEditorFactory::pointEditor;
    }
    else
    if( dynamic_cast< PointCloud3D* >( &object ) )
    {
        editorType = Object3DEditorFactory::pointCloudEditor;
    }
#ifndef NO_CRA
    else
    if( dynamic_cast< Pointer3D* >( &object ) )
    {
        editorType = Object3DEditorFactory::pointerEditor;
    }
#endif
    else
    {
        editorType = Object3DEditorFactory::genericEditor;
    }

    Object3DEditor* const editor = Object3DEditorFactory( editorType ).create( object );
    return editor;
}


void ObjectsController::objectsSelectionChanged()
{
    std::vector< Carna::base::model::Object3D* > selectedObjects;
    view->fetchSelectedObjects( selectedObjects );

    objectReleasing->setEnabled( selectedObjects.size()  > 0 );
    objectRenaming ->setEnabled( selectedObjects.size() == 1 );

    if( selectedObjects.size() == 1 )
    {
        openObjectEditor( *selectedObjects[ 0 ] );
    }
    else
    {
        closeObjectEditor();
    }
}


void ObjectsController::createPoint3D()
{
    new Carna::base::view::Point3D( CarnaContextClient( server ).model() );
}


void ObjectsController::createTempPoint1()
{
    Carna::base::model::Scene& model = CarnaContextClient( server ).model();
    new Carna::base::view::Point3D( model, Carna::base::model::Position::fromVolumeUnits( model, 0.72307533, 0.76862745, 0.33882746 ) );
}


void ObjectsController::importPolyline()
{
    const std::unique_ptr< PolylineImportConfiguration > setup(
        PolylineImportConfiguration::createFromUserPrompt( this ) );

    if( !setup.get() )
    {
        return;
    }

    const QString dialogTitle = "Polyline Import";

    QFile file( setup->getFileName() );
    if( !file.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::critical( this, dialogTitle, "Failed opening file " + setup->getFileName() + " for reading." );
        return;
    }

    QTextStream in( &file );
    const QString text = in.readAll();
    file.close();

    struct number_format_exception {};

 // create polyline

    Carna::base::model::Scene& model = CarnaContextClient( server ).model();
    Carna::base::view::Polyline* const line = new Carna::base::view::Polyline
        ( model
        , Carna::base::view::Polyline::lineStrip
        , setup->getLineWidth()
        , setup->getVertexSize() );

    line->setBorder( setup->hasBorder() );

 // link the polyline's lifetime to those of it's vertices

    connect( line, SIGNAL( allVerticesRemoved() ), line, SLOT( deleteLater() ) );

 // read vertices

    QApplication::setOverrideCursor( Qt::WaitCursor );

    try
    {
        int currentPosition = 0;
        while( ( currentPosition = setup->getVertexRegex().indexIn( text, currentPosition ) ) != -1 )
        {
            if( setup->getVertexRegex().captureCount() != 3 )
            {
                QMessageBox::critical( this, dialogTitle, "Must capture exactly 3 groups per vertex, but did " + QString::number( setup->getVertexRegex().captureCount() ) + "." );
                return;
            }

            bool ok = true;

            const double x = setup->getVertexRegex().cap( 1 ).toDouble( &ok );
            if( !ok ) throw number_format_exception();

            const double y = setup->getVertexRegex().cap( 2 ).toDouble( &ok );
            if( !ok ) throw number_format_exception();

            const double z = setup->getVertexRegex().cap( 3 ).toDouble( &ok );
            if( !ok ) throw number_format_exception();

            switch( setup->getUnits() )
            {

                case PolylineImportConfiguration::millimeters:
                {
                    ( *line ) << Carna::base::model::Position::fromMillimeters( model, x, y, z );
                    break;
                }

                case PolylineImportConfiguration::volumeUnits:
                {
                    ( *line ) << Carna::base::model::Position::fromVolumeUnits( model, x, y, z );
                    break;
                }

            }

            currentPosition += setup->getVertexRegex().matchedLength();
        }
    }
    catch( const number_format_exception& )
    {
        QMessageBox::critical( this, dialogTitle, "Failed to parse coordinate: not a number." );
        delete line;
    }

    QApplication::restoreOverrideCursor();
}


#ifndef NO_CRA

void ObjectsController::createPointer3D()
{
    new Pointer3D( server );
}

#endif  // NO_CRA
