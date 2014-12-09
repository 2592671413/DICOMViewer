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

#include "Object3DEditorDetails.h"
#include <QFormLayout>
#include <QDockWidget>



// ----------------------------------------------------------------------------------
// Object3DEditor
// ----------------------------------------------------------------------------------

Object3DEditor::Object3DEditor( Carna::Object3D& editedObject, QWidget* parent )
    : QMainWindow( parent )
    , editedObject( editedObject )
    , pimpl( new Details( *this ) )
{
    QWidget* const positionPage = new QWidget();
    QFormLayout* const position = new QFormLayout();
    positionPage->setLayout( position );
    positionPage->setWindowTitle( "Position" );

    position->addRow( "Units:", pimpl->cbUnit );

    position->addItem( new QSpacerItem( 5, 5 ) );

    position->addRow( "Position X:", pimpl->sbPositionX );
    position->addRow( "Position Y:", pimpl->sbPositionY );
    position->addRow( "Position Z:", pimpl->sbPositionZ );
    position->addRow( "Interpol. HUV:", pimpl->laHUV );

    addPage( positionPage );
}


Object3DEditor::~Object3DEditor()
{
}


void Object3DEditor::addPage( QWidget* page )
{
    QDockWidget* dock = new QDockWidget( page->windowTitle() );
    dock->setFeatures( QDockWidget::DockWidgetMovable );
    dock->setWidget( page );

    this->addDockWidget( Qt::LeftDockWidgetArea, dock );
    if( pimpl->lastAddedDock )
    {
        this->tabifyDockWidget( pimpl->lastAddedDock, dock );
    }

    pimpl->lastAddedDock = dock;
}
