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

#include "ToolChooser.h"
#include "LocalizerClient.h"
#include "NotificationsClient.h"
#include <QHBoxLayout>
#include <QComboBox>



// ----------------------------------------------------------------------------------
// ToolChooser
// ----------------------------------------------------------------------------------

ToolChooser::ToolChooser( Record::Server& server, QWidget* parent )
    : QWidget( parent )
    , cbTools( new QComboBox() )
    , server( server )
    , selectedTool( 0 )
    , ignoreSelectionChanges( false )
{
    QHBoxLayout* const layout = new QHBoxLayout();
    this->setLayout( layout );

    cbTools->setEditable( false );
    cbTools->setMinimumWidth( 130 );

    layout->addWidget( cbTools );
    layout->setContentsMargins( 0, 0, 0, 0 );

    NotificationsClient( server ).connectServiceProvided( this, SLOT( providerInstalled( const std::string& ) ) );
    NotificationsClient( server ).connectServiceRemoved ( this, SLOT( providerRemoved  ( const std::string& ) ) );

    connect( cbTools, SIGNAL(    currentIndexChanged( int ) )
           , this         , SLOT  ( processSelectionUpdate( int ) ) );

    if( server.hasService( Localizer::serviceID ) )
    {
        LocalizerClient( server ).connectToolAdded  ( this, SLOT( rebuildList( CRA::Tool& ) ) );
        LocalizerClient( server ).connectToolRemoved( this, SLOT( rebuildList( CRA::Tool& ) ) );
    }

    rebuildList();
}


void ToolChooser::providerInstalled( const std::string& serviceID )
{
    if( serviceID == Localizer::serviceID )
    {
        LocalizerClient( server ).connectToolAdded  ( this, SLOT( rebuildList( CRA::Tool& ) ) );
        LocalizerClient( server ).connectToolRemoved( this, SLOT( rebuildList( CRA::Tool& ) ) );

        cbTools->setEnabled( true );
    }
}


void ToolChooser::providerRemoved( const std::string& interfaceID )
{
    rebuildList();
}


void ToolChooser::rebuildList( CRA::Tool& )
{
    rebuildList();
}


void ToolChooser::rebuildList()
{
    ignoreSelectionChanges = true;

    cbTools->clear();

    cbTools->addItem( "( none )" );

    unsigned int prev_selected_object_new_index = 0;

    try
    {

        for( unsigned int i = 0; i < LocalizerClient( server ).getTools().size(); ++i )
        {
            CRA::Tool& rb = *( LocalizerClient( server ).getTools()[ i ] );

            if( &rb == selectedTool )
            {
                prev_selected_object_new_index = i + 1;
            }

            const std::string& name = rb.getName();
            cbTools->addItem( QString::fromStdString( name ) );

            connect( &rb, SIGNAL( renamed() ), this, SLOT( rebuildList() ) );
        }

        cbTools->setCurrentIndex( prev_selected_object_new_index );
        cbTools->setEnabled( true );

    }
    catch( const std::runtime_error& )
    {
        prev_selected_object_new_index = 0;
        cbTools->setCurrentIndex( 0 );
        cbTools->setEnabled( false );
    }

    ignoreSelectionChanges = false;

    processSelectionUpdate( prev_selected_object_new_index );
}


CRA::Tool& ToolChooser::getSelectedTool() const
{
    if( !selectedTool )
    {
        throw std::logic_error( "no Tool selected" );
    }

    return *selectedTool;
}


void ToolChooser::selectNone()
{
    if( selectedTool )
    {
        cbTools->setCurrentIndex( 0 );
    }
}


void ToolChooser::processSelectionUpdate( int item_index )
{
    if( ignoreSelectionChanges )
    {
        return;
    }

    if( item_index == 0 )
    {
        if( selectedTool )
        {
            selectedTool = 0;

            emit selectionDissolved();
        }
    }
    else
    {
        CRA::Tool* const newSelectedTool = LocalizerClient( server ).getTools()[ item_index - 1 ];
        if( selectedTool != newSelectedTool )
        {
            selectedTool = newSelectedTool;

            emit selectionChanged( *selectedTool );
            emit selectionChanged();
        }
    }
}
