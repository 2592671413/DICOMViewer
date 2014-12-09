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
#include <Carna/Position.h>
#include <Carna/Object3D.h>
#include <climits>



// ----------------------------------------------------------------------------------
// Object3DEditor :: Details
// ----------------------------------------------------------------------------------

const double Object3DEditor::Details::MIN_STEP_MILLIMETERS = 1e-2;
const double Object3DEditor::Details::MIN_STEP_VOLUMEUNITS = 1e-4;


Object3DEditor::Details::Details( Object3DEditor& self )
    : cbUnit( new QComboBox() )
    , sbPositionX( new QDoubleSpinBox() )
    , sbPositionY( new QDoubleSpinBox() )
    , sbPositionZ( new QDoubleSpinBox() )
    , laHUV( new QLabel() )
    , lastAddedDock( nullptr )
    , self( self )
{
    cbUnit->addItem( "Millimeters" );
    cbUnit->addItem( "Volume Units" );

    connect( cbUnit, SIGNAL( currentIndexChanged( int ) ), this, SLOT( setUnits( int ) ) );

    setUnits( cbUnit->currentIndex() );

    connect( &( self.editedObject ), SIGNAL( moved() ), this, SLOT( updatePosition() ) );

    connect( sbPositionX, SIGNAL( valueChanged( double ) ), this, SLOT( setPositionX( double ) ) );
    connect( sbPositionY, SIGNAL( valueChanged( double ) ), this, SLOT( setPositionY( double ) ) );
    connect( sbPositionZ, SIGNAL( valueChanged( double ) ), this, SLOT( setPositionZ( double ) ) );
}


void Object3DEditor::Details::setUnits( int units )
{
    currentUnits = static_cast< Units >( units );

    const Carna::Model& model = self.editedObject.model;

    configure( sbPositionX, 1. / ( model.volume().size.x - 1 ) );
    configure( sbPositionY, 1. / ( model.volume().size.y - 1 ) );
    configure( sbPositionZ, 1. / ( model.volume().size.z - 1 ) );

    updatePosition();
}


void Object3DEditor::Details::configure( QDoubleSpinBox* sb, float singleStepInVolumeSpace )
{
    sb->setMinimum( -std::numeric_limits< double >::infinity() );
    sb->setMaximum(  std::numeric_limits< double >::infinity() );
    sb->setDecimals( 8 );
    switch( currentUnits )
    {

        case millimeters:
        {
            sb->setSingleStep( 1. );
            sb->setSuffix( " mm" );
            break;
        }

        case volumeUnits:
        {
            sb->setSingleStep( singleStepInVolumeSpace );
            sb->setSuffix( " vu" );
            break;
        }

        default:
        {
            throw std::logic_error( "Unsupported units argument." );
        }

    }
}


void Object3DEditor::Details::updatePosition()
{
    const Carna::Position& position = self.editedObject.position();
    
    switch( currentUnits )
    {

        case millimeters:
        {
            currentPosition = position.toMillimeters();
            break;
        }

        case volumeUnits:
        {
            currentPosition = position.toVolumeUnits();
            break;
        }

        default:
        {
            throw std::logic_error( "Unsupported units argument." );
        }

    }

    sbPositionX->setValue( currentPosition.x() );
    sbPositionY->setValue( currentPosition.y() );
    sbPositionZ->setValue( currentPosition.z() );

    laHUV->setText( QString::number( self.editedObject.model.intensityAt( position ), 'f', 2 ) );
}


double Object3DEditor::Details::getCurrentEpsilon() const
{
    switch( currentUnits )
    {

        case millimeters:
        {
            return MIN_STEP_MILLIMETERS;
        }

        case volumeUnits:
        {
            return MIN_STEP_VOLUMEUNITS;
        }

        default:
        {
            throw std::logic_error( "Unsupported units argument." );
        }

    }
}


void Object3DEditor::Details::setCurrentCoordinate( double value, int coordinate )
{
    if( std::abs( currentPosition[ coordinate ] - value ) > getCurrentEpsilon() )
    {
        currentPosition[ coordinate ] = value;
        Carna::Model& model = self.editedObject.model;
        switch( currentUnits )
        {

            case millimeters:
            {
                self.editedObject.setPosition( Carna::Position::fromMillimeters( model, currentPosition ) );
                break;
            }

            case volumeUnits:
            {
                self.editedObject.setPosition( Carna::Position::fromVolumeUnits( model, currentPosition ) );
                break;
            }

            default:
            {
                throw std::logic_error( "Unsupported units argument." );
            }

        }
    }
}


void Object3DEditor::Details::setPositionX( double x )
{
    setCurrentCoordinate( x, 0 );
}


void Object3DEditor::Details::setPositionY( double y )
{
    setCurrentCoordinate( y, 1 );
}


void Object3DEditor::Details::setPositionZ( double z )
{
    setCurrentCoordinate( z, 2 );
}
