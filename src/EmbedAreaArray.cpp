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

#include "EmbedAreaArray.h"
#include <QGridLayout>



// ----------------------------------------------------------------------------------
// EmbedAreaArray
// ----------------------------------------------------------------------------------

EmbedAreaArray::EmbedAreaArray( QWidget* parent )
    : QWidget( parent )
    , rows( 0 )
    , cols( 0 )
{
    this->setLayout( new QGridLayout() );
    setDimensions( 1, 1 );
}


void EmbedAreaArray::setDimensions( unsigned int rows, unsigned int cols )
{
    QGridLayout* const myLayout = static_cast<QGridLayout*>( layout() );
    for( unsigned int row = 0; row < rows; ++row )
    {
        for( unsigned int col = 0; col < cols; ++col )
        {
            if( row >= this->rows || col >= this->cols )
            {
                myLayout->addWidget( new GriddedEmbedArea( row, col, this ), row, col );
            }
        }
    }
    for( unsigned int row = 0; row < this->rows; ++row )
    {
        for( unsigned int col = 0; col < this->cols; ++col )
        {
            if( row < rows && col < cols )
            {
                continue;
            }
            QLayoutItem* item = myLayout->itemAtPosition( row, col );
            QWidget* widget = item->widget();
            if( widget )
            {
                EmbedArea* pEA = static_cast<EmbedArea*>( widget );
                if( pEA->hasClient() )
                {
                    pEA->releaseClient();
                }
                widget->hide();
                myLayout->removeWidget( widget );
                delete widget;
            }
        }
    }
    
    for( unsigned int row = rows; row <= this->rows; ++row )
    {
        myLayout->setRowStretch( row, 0 );
    }
    for( unsigned int col = cols; col <= this->cols; ++col )
    {
        myLayout->setColumnStretch( col, 0 );
    }

    this->rows = rows;
    this->cols = cols;
    
    for( unsigned int row = 0; row < rows; ++row )
    {
        myLayout->setRowStretch( row, 1 );
    }
    for( unsigned int col = 0; col < cols; ++col )
    {
        myLayout->setColumnStretch( col, 1 );
    }

    this->update();
}
