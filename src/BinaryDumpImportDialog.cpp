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

#include "BinaryDumpImportDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QGroupBox>



// ----------------------------------------------------------------------------------
// ImportDialog
// ----------------------------------------------------------------------------------

ImportDialog::ImportDialog( QWidget* parent, Qt::WindowFlags f )
    : QDialog( parent, f )
    , voxelFormat( *new IntegerFormatChooser() )
    , width ( 0 )
    , height( 0 )
    , depth ( 0 )
    , spacingX( 0 )
    , spacingY( 0 )
    , spacingZ( 0 )
    , metaDataFromFile( true )
{
    this->setWindowTitle( "Import Binary Dump" );
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

    QGroupBox* const gbMetaData = new QGroupBox( "Binary dump has no meta data header" );
    gbMetaData->setCheckable( true );
    gbMetaData->setChecked( !metaDataFromFile );

    QFormLayout* const metaData = new QFormLayout();
    form->addRow( gbMetaData );
    gbMetaData->setLayout( metaData );

    connect( gbMetaData, SIGNAL( toggled( bool ) ), this, SLOT( setMetaDataFromFile( bool ) ) );

 // dataset dimensions

    QLineEdit* const leWidth = new QLineEdit();
    metaData->addRow( "&Width:", leWidth );
    leWidth->setText( QString::number( width ) );
    QObject::connect( leWidth, SIGNAL( textChanged( const QString& ) ), this, SLOT( setDataSetWidth( const QString& ) ) );

    QLineEdit* const leHeight = new QLineEdit();
    metaData->addRow( "&Height:", leHeight );
    leHeight->setText( QString::number( height ) );
    QObject::connect( leHeight, SIGNAL( textChanged( const QString& ) ), this, SLOT( setDataSetHeight( const QString& ) ) );

    QLineEdit* const leDepth = new QLineEdit();
    metaData->addRow( "&Depth:", leDepth );
    leDepth->setText( QString::number( depth ) );
    QObject::connect( leDepth, SIGNAL( textChanged( const QString& ) ), this, SLOT( setDataSetDepth( const QString& ) ) );

    metaData->addItem( createSpacerItem() );

 // spacing

    QLineEdit* const leSpacingX = new QLineEdit();
    metaData->addRow( "Spacing &X:", leSpacingX );
    leSpacingX->setText( QString::number( spacingX ) );
    QObject::connect( leSpacingX, SIGNAL( textChanged( const QString& ) ), this, SLOT( setSpacingX( const QString& ) ) );

    QLineEdit* const leSpacingY = new QLineEdit();
    metaData->addRow( "Spacing &Y:", leSpacingY );
    leSpacingY->setText( QString::number( spacingY ) );
    QObject::connect( leSpacingY, SIGNAL( textChanged( const QString& ) ), this, SLOT( setSpacingY( const QString& ) ) );

    QLineEdit* const leSpacingZ = new QLineEdit();
    metaData->addRow( "Spacing &Z:", leSpacingZ );
    leSpacingZ->setText( QString::number( spacingZ ) );
    QObject::connect( leSpacingZ, SIGNAL( textChanged( const QString& ) ), this, SLOT( setSpacingZ( const QString& ) ) );

 // put all together

    global->addLayout( form );

    QDialogButtonBox* const buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttons, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );

    global->addWidget( buttons );

    this->setLayout( global );
}


void ImportDialog::setDataSetWidth( const QString& value_str )
{
    bool ok = false;
    const unsigned int value = value_str.toUInt( &ok );

    width = ( ok ? value : 0 );
}


void ImportDialog::setDataSetHeight( const QString& value_str )
{
    bool ok = false;
    const unsigned int value = value_str.toUInt( &ok );

    height = ( ok ? value : 0 );
}


void ImportDialog::setDataSetDepth( const QString& value_str )
{
    bool ok = false;
    const unsigned int value = value_str.toUInt( &ok );

    depth = ( ok ? value : 0 );
}


void ImportDialog::setSpacingX( const QString& value_str )
{
    bool ok = false;
    const double value = value_str.toDouble( &ok );

    spacingX = ( ok && value > 0 ? value : 0 );
}


void ImportDialog::setSpacingY( const QString& value_str )
{
    bool ok = false;
    const double value = value_str.toDouble( &ok );

    spacingY = ( ok && value > 0 ? value : 0 );
}


void ImportDialog::setSpacingZ( const QString& value_str )
{
    bool ok = false;
    const double value = value_str.toDouble( &ok );

    spacingZ = ( ok && value > 0 ? value : 0 );
}


void ImportDialog::accept()
{
    if( isMetaDataHeaderEnabled() )
    {
        QDialog::accept();
    }
    else
    {

        const static double EPSILON = 1e-6;

        bool ok = true;

        if( width == 0 || height == 0 || depth == 0 )
        {
            ok = false;
        }
        if( spacingX < EPSILON || spacingY < EPSILON || spacingZ < EPSILON )
        {
            ok = false;
        }

        if( ok )
        {
            QDialog::accept();
        }
        else
        {
            throw std::runtime_error( "The input is invalid." );
        }

    }
}


void ImportDialog::setMetaDataFromFile( bool customMetaData )
{
    this->metaDataFromFile = !customMetaData;
}
