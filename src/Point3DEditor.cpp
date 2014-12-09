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

#include "Point3DEditor.h"
#include <Carna/Point3D.h>
#include <Carna/ColorPicker.h>
#include <QFormLayout>
#include <QDoubleSpinBox>



// ----------------------------------------------------------------------------------
// Point3DEditor
// ----------------------------------------------------------------------------------

Point3DEditor::Point3DEditor( Carna::Point3D& editedObject, QWidget* parent )
    : Object3DEditor( editedObject, parent )
    , editedPoint( editedObject )
{
    QWidget* const pointPage = new QWidget();
    QFormLayout* const point = new QFormLayout();
    pointPage->setLayout( point );
    pointPage->setWindowTitle( "Color / Size" );

    Carna::ColorPicker* colorPicker = new Carna::ColorPicker( editedPoint.color(), true );
    point->addRow( "Color:", colorPicker );

    connect( colorPicker, SIGNAL( colorChanged( const QColor& ) ), &editedPoint, SLOT( setColor( const QColor& ) ) );
    connect( &editedPoint, SIGNAL( colorChanged( const QColor& ) ), colorPicker, SLOT( setColor( const QColor& ) ) );

    QDoubleSpinBox* const sbSize = new QDoubleSpinBox();
    sbSize->setMinimum(   1. );
    sbSize->setMaximum( 100. );
    sbSize->setValue( editedPoint.size() );
    sbSize->setDecimals( 1 );
    sbSize->setSingleStep( 1. );
    point->addRow( "Size:", sbSize );

    connect( sbSize, SIGNAL( valueChanged( double ) ), &editedPoint, SLOT( setSize( double ) ) );
    connect( &editedPoint, SIGNAL( sizeChanged( double ) ), sbSize, SLOT( setValue( double ) ) );

    addPage( pointPage );
}
