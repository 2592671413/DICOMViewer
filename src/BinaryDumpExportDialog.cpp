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

#include "BinaryDumpExportDialog.h"
#include <cstdint>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QLabel>



// ----------------------------------------------------------------------------------
// ExportDialog
// ----------------------------------------------------------------------------------

ExportDialog::ExportDialog( QWidget* parent, Qt::WindowFlags f )
    : QDialog( parent, f )
    , voxelFormat( *new IntegerFormatChooser() )
    , metaDataExport( true )
{
    this->setWindowTitle( "Export Binary Dump" );
    if( parent != nullptr )
    {
        this->setWindowModality( Qt::WindowModal );
    }

    QVBoxLayout* const global = new QVBoxLayout();
    QFormLayout* const form = new QFormLayout();

    auto createSpacerItem = []()->QSpacerItem*
    {
        return new QSpacerItem( 5, 5 );
    };

 // voxel format

    form->addRow( "Voxel &format:", &voxelFormat );
    form->addItem( createSpacerItem() );

 // meta data

    const unsigned int header_size = sizeof( uint16_t ) * 3 + sizeof( double ) * 3;
    QCheckBox* const cbMetaData = new QCheckBox( "Write " + QString::number( header_size ) + " bytes meta data &header." );
    cbMetaData->setChecked( metaDataExport );
    form->addRow( cbMetaData );

    QWidget* const metaDataDetails = new QWidget();
    QLabel* const laMetaDataDetails = new QLabel( "This includes the dataset dimensions and the spacings. If the header is not written, you will have to put in those values on your own responsibility, when you try to re-import the data at a later time." );
    QFont metaDataDetailsFont = laMetaDataDetails->font();
    metaDataDetailsFont.setPointSizeF( metaDataDetailsFont.pointSizeF() - 0.5f );
    laMetaDataDetails->setStyleSheet( "margin-top: 0px;" );
    laMetaDataDetails->setFont( metaDataDetailsFont );
    laMetaDataDetails->setWordWrap( true );
    laMetaDataDetails->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Minimum );
    metaDataDetails->setLayout( new QVBoxLayout() );
    metaDataDetails->layout()->setContentsMargins( 25, 0, 0, 0 );
    metaDataDetails->layout()->addWidget( laMetaDataDetails );
    form->addRow( metaDataDetails );

    connect( cbMetaData, SIGNAL( toggled( bool ) ), this, SLOT( setMetaDataExport( bool ) ) );

 // put all together

    global->addLayout( form );

 // dialog buttons

    QDialogButtonBox* const buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttons, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );

    global->addWidget( buttons );

    this->setLayout( global );
}


void ExportDialog::setMetaDataExport( bool metaDataExport )
{
    this->metaDataExport = metaDataExport;
}
