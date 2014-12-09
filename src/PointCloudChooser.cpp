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

#include "PointCloudChooser.h"
#include "PointCloudsClient.h"
#include "PointCloud.h"
#include <QHBoxLayout>
#include <QComboBox>



// ----------------------------------------------------------------------------------
// PointCloudChooser
// ----------------------------------------------------------------------------------

PointCloudChooser::PointCloudChooser( Record::Server& server, QWidget* parent )
    : QWidget( parent )
    , cbPointClouds( new QComboBox() )
    , server( server )
    , selectedPointCloud( 0 )
    , ignoreSelectionChanges( false )
{
    QHBoxLayout* const layout = new QHBoxLayout();
    this->setLayout( layout );

    cbPointClouds->setEditable( false );
    cbPointClouds->setMinimumWidth( 130 );

    layout->addWidget( cbPointClouds );
    layout->setContentsMargins( 0, 0, 0, 0 );

    connect( cbPointClouds, SIGNAL(    currentIndexChanged( int ) )
           , this         , SLOT  ( processSelectionUpdate( int ) ) );

    if( server.hasService( PointClouds::serviceID) )
    {
        PointCloudsClient( server ).connectPointCloudAdded( this, SLOT( rebuildList( PointCloud& ) ) );
    }

    rebuildList();
}


void PointCloudChooser::rebuildList( PointCloud& )
{
    rebuildList();
}


void PointCloudChooser::rebuildList()
{
    ignoreSelectionChanges = true;

    cbPointClouds->clear();

    cbPointClouds->addItem( "( none )" );

    unsigned int prev_selected_object_new_index = 0;

    try
    {

        for( unsigned int i = 0; i < PointCloudsClient( server ).getPointClouds().size(); ++i )
        {
            PointCloud& pointCloud = *( PointCloudsClient( server ).getPointClouds()[ i ] );

            if( &pointCloud == selectedPointCloud )
            {
                prev_selected_object_new_index = i + 1;
            }

            const std::string& name = pointCloud.getName();
            cbPointClouds->addItem( QString::fromStdString( name ) );

            connect( &pointCloud, SIGNAL( renamed() ), this, SLOT( rebuildList() ) );
        }

        cbPointClouds->setCurrentIndex( prev_selected_object_new_index );
        cbPointClouds->setEnabled( true );

    }
    catch( const std::runtime_error& )
    {
        prev_selected_object_new_index = 0;
        cbPointClouds->setCurrentIndex( 0 );
        cbPointClouds->setEnabled( false );
    }

    ignoreSelectionChanges = false;

    processSelectionUpdate( prev_selected_object_new_index );
}


PointCloud& PointCloudChooser::getSelectedPointCloud() const
{
    if( !selectedPointCloud )
    {
        throw std::logic_error( "no PointCloud selected" );
    }

    return *selectedPointCloud;
}


void PointCloudChooser::processSelectionUpdate( int item_index )
{
    if( ignoreSelectionChanges )
    {
        return;
    }

    if( item_index == 0 )
    {
        if( selectedPointCloud )
        {
            selectedPointCloud = 0;

            emit selectionDissolved();
        }
    }
    else
    {
        PointCloud* const newSelectedPointCloud = PointCloudsClient( server ).getPointClouds()[ item_index - 1 ];
        if( selectedPointCloud != newSelectedPointCloud )
        {
            selectedPointCloud = newSelectedPointCloud;

            emit selectionChanged( *selectedPointCloud );
            emit selectionChanged();
        }
    }
}
