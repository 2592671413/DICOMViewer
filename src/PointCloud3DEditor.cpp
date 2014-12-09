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

#include "PointCloud3DEditor.h"
#include "PointCloud3D.h"
#include <Carna/ColorPicker.h>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>



// ----------------------------------------------------------------------------------
// PointCloud3DEditor
// ----------------------------------------------------------------------------------

PointCloud3DEditor::PointCloud3DEditor( PointCloud3D& editedObject, QWidget* parent )
    : Object3DEditor( editedObject, parent )
    , editedPointCloud( editedObject )
{
    QWidget* const colorAndSizePage = new QWidget();
    QFormLayout* const colorAndSize = new QFormLayout();
    colorAndSizePage->setLayout( colorAndSize );
    colorAndSizePage->setWindowTitle( "Point Color / Size" );

 // colorAndSize color

    Carna::ColorPicker* colorPicker = new Carna::ColorPicker( editedPointCloud.getPointColor(), true );
    colorAndSize->addRow( "Color:", colorPicker );

    connect( colorPicker, SIGNAL( colorChanged( const QColor& ) ), &editedPointCloud, SLOT( setPointColor( const QColor& ) ) );
    connect( &editedPointCloud, SIGNAL( pointColorChanged( const QColor& ) ), colorPicker, SLOT( setColor( const QColor& ) ) );

 // colorAndSize size

    QDoubleSpinBox* const sbSize = new QDoubleSpinBox();
    sbSize->setMinimum(   1. );
    sbSize->setMaximum( 100. );
    sbSize->setValue( editedPointCloud.getPointSize() );
    sbSize->setDecimals( 1 );
    sbSize->setSingleStep( 1. );
    colorAndSize->addRow( "Size:", sbSize );

    connect( sbSize, SIGNAL( valueChanged( double ) ), &editedPointCloud, SLOT( setPointSize( double ) ) );
    connect( &editedPointCloud, SIGNAL( pointSizeChanged( double ) ), sbSize, SLOT( setValue( double ) ) );

 // brightness modulation

    QCheckBox* const cbBrightnessModulation = new QCheckBox( "Modulate brightness by distance" );
    cbBrightnessModulation->setChecked( editedPointCloud.modulatesBrightnessByDistance() );
    colorAndSize->addRow( cbBrightnessModulation );
    
    connect( cbBrightnessModulation, SIGNAL( toggled( bool ) ), &editedPointCloud, SLOT( setBrightnessByDistanceModulation( bool ) ) );
    connect( &editedPointCloud, SIGNAL( brightnessByDistanceModulationChanged( bool ) ), cbBrightnessModulation, SLOT( setChecked( bool ) ) );

 // bounding box

    QCheckBox* const cbBoundingBox = new QCheckBox( "Draw bounding box" );
    cbBoundingBox->setChecked( editedPointCloud.drawsBoundingBox() );
    colorAndSize->addRow( cbBoundingBox );

    connect( cbBoundingBox, SIGNAL( toggled( bool ) ), &editedPointCloud, SLOT( setBoundingBoxDrawing( bool ) ) );
    connect( &editedPointCloud, SIGNAL( boundingBoxDrawingChanged( bool ) ), cbBoundingBox, SLOT( setChecked( bool ) ) );

 // registration page

    QWidget* const registrationPage = new QWidget();
    QFormLayout* const registration = new QFormLayout();
    registrationPage->setLayout( registration );
    registrationPage->setWindowTitle( "Registration" );

 // apply registration

    QCheckBox* const cbApplyRegistration = new QCheckBox( "Apply registration" );
    cbApplyRegistration->setChecked( editedPointCloud.appliesRegistration() );
    registration->addRow( cbApplyRegistration );

    QWidget* const applyRegistrationDetails = new QWidget();
    QLabel* const laApplyRegistrationDetails = new QLabel( "If this is a tracking-side point cloud and you have a registration provided, check this to make it be displayed at the right place." );
    QFont metaDataDetailsFont = laApplyRegistrationDetails->font();
    metaDataDetailsFont.setPointSizeF( metaDataDetailsFont.pointSizeF() - 0.5f );
    laApplyRegistrationDetails->setStyleSheet( "margin-top: 0px;" );
    laApplyRegistrationDetails->setFont( metaDataDetailsFont );
    laApplyRegistrationDetails->setWordWrap( true );
    laApplyRegistrationDetails->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Minimum );
    applyRegistrationDetails->setLayout( new QVBoxLayout() );
    applyRegistrationDetails->layout()->setContentsMargins( 25, 0, 0, 0 );
    applyRegistrationDetails->layout()->addWidget( laApplyRegistrationDetails );
    registration->addRow( applyRegistrationDetails );

    connect( cbApplyRegistration, SIGNAL( toggled( bool ) ), &editedPointCloud, SLOT( setRegistrationApplication( bool ) ) );
    connect( &editedPointCloud, SIGNAL( registrationApplicationChanged( bool ) ), cbApplyRegistration, SLOT( setChecked( bool ) ) );

 // finish

    addPage( colorAndSizePage );
    addPage( registrationPage );
}
