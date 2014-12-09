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

#include "MaskingDialog.h"
#include "PointCloudChooser.h"
#include "PointCloud.h"
#include "CarnaContextClient.h"
#include <Carna/base/Vector3.h>
#include <Carna/base/model/Scene.h>
#include <Carna/base/model/Volume.h>
#include <Carna/base/model/Position.h>
#include <Carna/base/CarnaException.h>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include <QProgressDialog>
#include <vector>



// ----------------------------------------------------------------------------------
// PointCloudMaskAdapter
// ----------------------------------------------------------------------------------

class PointCloudMaskAdapter : public Carna::base::model::SceneFactory::BinarizedScalarField
{

public:

    PointCloudMaskAdapter( Carna::base::model::Scene& model, const PointCloud& points, QWidget* const parent = nullptr );


    virtual bool operator()( unsigned int x, unsigned int y, unsigned int z ) const override
    {
        const unsigned int index = getIndex( x, y, z );
        return mask[ index ];
    }

    virtual bool operator()( const Carna::base::Vector3ui& position ) const override
    {
        return ( *this )( position.x, position.y, position.z );
    }


private:

    const Carna::base::Vector3ui size;
    std::vector< bool > mask;

    unsigned int getIndex( unsigned int x, unsigned int y, unsigned int z ) const
    {
        return z * size.x * size.y + y * size.x + x;
    }

}; // PointCloudMaskAdapter


PointCloudMaskAdapter::PointCloudMaskAdapter( Carna::base::model::Scene& model, const PointCloud& points, QWidget* const parent )
    : size( model.volume().size )
    , mask( size.x * size.y * size.z, false )
{
    QProgressDialog progress( "Rasterizing...", "Masking", 0, points.getList().size(), parent );
    progress.setWindowTitle( QApplication::applicationName() );
    progress.setWindowModality( Qt::WindowModal );
    progress.setCancelButton( nullptr );
    progress.show();

    for( unsigned int pointIndex = 0; pointIndex < points.getList().size(); ++pointIndex )
    {
        const Carna::base::model::Position position = points.getPoint( pointIndex );
        const Carna::base::Vector millimeters = position.toMillimeters();

        const unsigned int x = static_cast< unsigned int >( millimeters.x() / model.spacingX() );
        const unsigned int y = static_cast< unsigned int >( millimeters.y() / model.spacingY() );
        const unsigned int z = static_cast< unsigned int >( millimeters.z() / model.spacingZ() );

        const unsigned int outputIndex = getIndex( x, y, z );

        mask[ outputIndex ] = true;

        progress.setValue( pointIndex );
        QApplication::processEvents();
    }

    progress.hide();
}



// ----------------------------------------------------------------------------------
// MaskingDialog
// ----------------------------------------------------------------------------------

MaskingDialog::MaskingDialog( Record::Server& server, QWidget* parent, Qt::WindowFlags f )
    : server( server )
    , QDialog( parent, f )
{
    this->setWindowTitle( "Select a mask" );
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

 // hint

    form->addRow( new QLabel( "If you proceed, the current data set will be closed and a new one loaded." ) );

 // point cloud

    PointCloudChooser* const pointCloudChooser = new PointCloudChooser( server );

    form->addRow( "Point Cloud:", pointCloudChooser );
    form->addItem( createSpacerItem() );

    connect( pointCloudChooser, SIGNAL( selectionChanged( PointCloud& ) ), this, SLOT( createMask( PointCloud& ) ) );
    connect( pointCloudChooser, SIGNAL( selectionDissolved() ), this, SLOT( releaseMask() ) );

 // put all together

    global->addLayout( form );

 // dialog buttons

    QDialogButtonBox* const buttons = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
    connect( buttons, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( buttons, SIGNAL( rejected() ), this, SLOT( reject() ) );

    global->addWidget( buttons );
    acceptButton = buttons->button( QDialogButtonBox::Ok );
    acceptButton->setEnabled( false );

    CARNA_ASSERT( acceptButton != nullptr );

    this->setLayout( global );
}


void MaskingDialog::createMask( PointCloud& pointCloud )
{
    Carna::base::model::Scene& model = CarnaContextClient( server ).model();

    QApplication::setOverrideCursor( Qt::WaitCursor );
    QApplication::processEvents();

    this->mask.reset( new PointCloudMaskAdapter( model, pointCloud, this ) );

    QApplication::restoreOverrideCursor();

    acceptButton->setEnabled( true );
}


void MaskingDialog::releaseMask()
{
    acceptButton->setEnabled( false );
}
