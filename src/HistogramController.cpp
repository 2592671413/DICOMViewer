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

#include "HistogramView.h"
#include "HistogramController.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <climits>



// ----------------------------------------------------------------------------------
// HistogramController
// ----------------------------------------------------------------------------------

HistogramController::HistogramController( HistogramView& view )
    : view( view )
    , laValuesPerPixel( new QLabel() )
    , laClassSize( new QLabel() )
    , laBarWidth( new QLabel() )
{
    connect( &view, SIGNAL( parametersChanged() ), this, SLOT( updateParametersView() ) );

    QFormLayout* const form = new QFormLayout();

    this->setLayout( form );

    QComboBox* const cbFunction = new QComboBox();
    cbFunction->addItem( "Linear" );
    cbFunction->addItem( "Logarithmic" );
    cbFunction->setCurrentIndex( 1 );

    connect( cbFunction, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateFunction( int ) ) );

    QDoubleSpinBox* const sbZoom = new QDoubleSpinBox();
    sbZoom->setMinimum( 1 );
    sbZoom->setMaximum( std::numeric_limits< double >::max() );
    sbZoom->setValue( view.getZoom() );
    sbZoom->setSuffix( " times" );
    sbZoom->setDecimals( 1 );
    sbZoom->setSingleStep( std::pow( 10., -sbZoom->decimals() ) );

    connect( sbZoom, SIGNAL( valueChanged( double ) ),  &view, SLOT(  setZoom( double ) ) );
    connect(  &view, SIGNAL(  zoomChanged( double ) ), sbZoom, SLOT( setValue( double ) ) );

    QSpinBox* const sbMinClassSize = new QSpinBox();
    sbMinClassSize->setMinimum( 1 );
    sbMinClassSize->setMaximum( 4096 );
    sbMinClassSize->setValue( view.getMinClassSize() );
    sbMinClassSize->setSuffix( " values" );

    connect( sbMinClassSize, SIGNAL( valueChanged( int ) ), &view, SLOT( setMinClassSize( int ) ) );

    QSpinBox* const sbMinBarWidth = new QSpinBox();
    sbMinBarWidth->setMinimum( 1 );
    sbMinBarWidth->setMaximum( 1000 );
    sbMinBarWidth->setValue( view.getMinBarWidth() );
    sbMinBarWidth->setSuffix( " pixels" );

    connect( sbMinBarWidth, SIGNAL( valueChanged( int ) ), &view, SLOT( setMinBarWidth( int ) ) );

    QSpinBox* const sbMinHuv = new QSpinBox();
    sbMinHuv->setMinimum( -1024 );
    sbMinHuv->setMaximum(  3071 );
    sbMinHuv->setValue( view.getMinHuv() );

    connect( sbMinHuv, SIGNAL(  valueChanged( int ) ),    &view, SLOT( setMinHuv( int ) ) );
    connect(    &view, SIGNAL( minHuvChanged( int ) ), sbMinHuv, SLOT(  setValue( int ) ) );

    QSpinBox* const sbMaxHuv = new QSpinBox();
    sbMaxHuv->setMinimum( -1024 );
    sbMaxHuv->setMaximum(  3071 );
    sbMaxHuv->setValue( view.getMaxHuv() );

    connect( sbMaxHuv, SIGNAL(  valueChanged( int ) ),    &view, SLOT( setMaxHuv( int ) ) );
    connect(    &view, SIGNAL( maxHuvChanged( int ) ), sbMaxHuv, SLOT(  setValue( int ) ) );

    auto createSpacerItem = []()->QSpacerItem*
    {
        return new QSpacerItem( 5, 5 );
    };

    form->addRow( "Scale:", cbFunction );
    form->addRow( "Zoom:", sbZoom );
    form->addItem( createSpacerItem() );
    form->addRow( "Min. Class Size:", sbMinClassSize );
    form->addRow( "Min. Bar Width:", sbMinBarWidth );
    form->addItem( createSpacerItem() );
    form->addRow( "Current Class Size:", laClassSize );
    form->addRow( "Current Bar Width:", laBarWidth );
    form->addRow( "Values Per Pixel:", laValuesPerPixel );
    form->addItem( createSpacerItem() );
    form->addRow( "Min. HUV:", sbMinHuv );
    form->addRow( "Max. HUV:", sbMaxHuv );
}


void HistogramController::updateFunction( int index )
{
    switch( index )
    {

    case 0:
        view.setScale( HistogramView::linear );
        break;

    case 1:
        view.setScale( HistogramView::logarithmic );
        break;

    default:
        throw std::logic_error( "Unknown function index." );

    }

    view.setAutoZoom();
}


void HistogramController::updateParametersView()
{
    laBarWidth->setText( QString::number( view.getBarWidth() ) + " pixels" );
    laClassSize->setText( QString::number( view.getClassSize() ) + " values" );
    laValuesPerPixel->setText( QString::number( view.getValuesPerPixel() ) );
}
