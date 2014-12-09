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

#include "Pointer3DEditor.h"
#include "Pointer3D.h"
#include "Pointer3DCalibration.h"
#include <Carna/WideColorPicker.h>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>



// ----------------------------------------------------------------------------------
// Pointer3DEditor
// ----------------------------------------------------------------------------------

Pointer3DEditor::Pointer3DEditor( Pointer3D& editedPointer, QWidget* parent )
    : Object3DEditor( editedPointer, parent )
    , editedPointer( editedPointer )
    , calibration( new Pointer3DCalibration( editedPointer ) )
{
    QWidget* const sizeAndColorPage = new QWidget();
    QFormLayout* const sizeAndColorForm = new QFormLayout();
    sizeAndColorPage->setLayout( sizeAndColorForm );
    sizeAndColorPage->setWindowTitle( "Color / Size" );

 // head color

    Carna::WideColorPicker* headColorPicker = new Carna::WideColorPicker( editedPointer.getHeadColor() );
    sizeAndColorForm->addRow( "Head Color:", headColorPicker );

    connect( headColorPicker, SIGNAL( colorChanged( const QColor& ) ), &editedPointer, SLOT( setHeadColor( const QColor& ) ) );
    connect( &editedPointer, SIGNAL( headColorChanged( const QColor& ) ), headColorPicker, SLOT( setColor( const QColor& ) ) );

 // shaft color

    Carna::WideColorPicker* shaftColorPicker = new Carna::WideColorPicker( editedPointer.getShaftColor() );
    sizeAndColorForm->addRow( "Shaft Color:", shaftColorPicker );

    connect( shaftColorPicker, SIGNAL( colorChanged( const QColor& ) ), &editedPointer, SLOT( setShaftColor( const QColor& ) ) );
    connect( &editedPointer, SIGNAL( shaftColorChanged( const QColor& ) ), shaftColorPicker, SLOT( setColor( const QColor& ) ) );

 // length

    QDoubleSpinBox* const sbLength = new QDoubleSpinBox();
    sbLength->setMinimum(    1. );
    sbLength->setMaximum( 1000. );
    sbLength->setValue( editedPointer.getLength() );
    sbLength->setDecimals( 1 );
    sbLength->setSingleStep( 2. );
    sbLength->setSuffix( " mm" );
    sizeAndColorForm->addRow( "Length:", sbLength );

    connect( sbLength, SIGNAL( valueChanged( double ) ), &editedPointer, SLOT( setLength( double ) ) );
    connect( &editedPointer, SIGNAL( lengthChanged( double ) ), sbLength, SLOT( setValue( double ) ) );

 // width

    QDoubleSpinBox* const sbWidth = new QDoubleSpinBox();
    sbWidth->setMinimum(   1. );
    sbWidth->setMaximum( 100. );
    sbWidth->setValue( editedPointer.getWidth() );
    sbWidth->setDecimals( 1 );
    sbWidth->setSingleStep( 1. );
    sizeAndColorForm->addRow( "Width:", sbWidth );

    connect( sbWidth, SIGNAL( valueChanged( double ) ), &editedPointer, SLOT( setWidth( double ) ) );
    connect( &editedPointer, SIGNAL( widthChanged( double ) ), sbWidth, SLOT( setValue( double ) ) );

 // shaft direction page

    QWidget* const shaftDirectionPage = new QWidget();
    QFormLayout* const shaftDirectionForm = new QFormLayout();
    shaftDirectionPage->setLayout( shaftDirectionForm );
    shaftDirectionPage->setWindowTitle( "Shaft Direction" );

 // calibration

    QGroupBox* const gbCalibration = new QGroupBox( "Calibration" );
    QVBoxLayout* const calibrationLayout = new QVBoxLayout();
    gbCalibration->setLayout( calibrationLayout );
    shaftDirectionForm->addRow( gbCalibration );
    shaftDirectionForm->addItem( new QSpacerItem( 5, 5 ) );

    QPushButton* const buCalibrationReset   = new QPushButton( "Reset" );
    QPushButton* const buCalibrationCapture = new QPushButton( "Capture" );

    QHBoxLayout* const calibrationButtons = new QHBoxLayout();
    calibrationButtons->setContentsMargins( 0, 0, 0, 0 );
    calibrationButtons->addWidget( buCalibrationReset );
    calibrationButtons->addWidget( buCalibrationCapture );

    QLabel* const laCalibrationHint = new QLabel( "Move the pointer along it's shaft's axis and capture at least two points." );
    laCalibrationHint->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
    laCalibrationHint->setWordWrap( true );

    calibrationLayout->addWidget( laCalibrationHint );
    calibrationLayout->addLayout( calibrationButtons );

    connect( buCalibrationReset, SIGNAL( clicked() ), calibration.get(), SLOT( reset  () ) );
    connect( buCalibrationCapture, SIGNAL( clicked() ), this, SLOT( captureAndComputeCalibration() ) );

    connect( calibration.get(), SIGNAL( computed( const Carna::Tools::Vector& ) ), &editedPointer, SLOT( setShaftDirection( const Carna::Tools::Vector& ) ) );

 // invert

    QCheckBox* const cbInverseDirection = new QCheckBox( "Invert shaft direction" );
    cbInverseDirection->setChecked( editedPointer.isInverseDirection() );
    shaftDirectionForm->addRow( cbInverseDirection );

    connect( cbInverseDirection, SIGNAL( toggled( bool ) ), &editedPointer, SLOT( setInverseDirection( bool ) ) );
    connect( &editedPointer, SIGNAL( inverseDirectionChanged( bool ) ), cbInverseDirection, SLOT( setChecked( bool ) ) );

 // finish

    addPage( sizeAndColorPage );
    addPage( shaftDirectionPage );
}


Pointer3DEditor::~Pointer3DEditor()
{
}


void Pointer3DEditor::captureAndComputeCalibration()
{
    calibration->capture();
    try
    {
        calibration->compute();
    }
    catch( ... )
    {
    }
}
