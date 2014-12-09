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

#include "PointCloudComposer.h"
#include "PointCloudComposerSlot.h"
#include "PointCloud.h"
#include "ComponentEmbeddable.h"
#include <Carna/base/model/Object3D.h>
#include <QVBoxLayout>
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include <QScrollArea>
#include <QTimer>



// ----------------------------------------------------------------------------------
// PointCloudComposer
// ----------------------------------------------------------------------------------

PointCloudComposer::PointCloudComposer( Record::Server& server
                                    , PointCloud& cloud
                                    , QWidget* parent )
    : QWidget( parent )
    , server( server )
    , cloud( cloud )
    , slotContainer( new QVBoxLayout() )
{
    QVBoxLayout* const global = new QVBoxLayout();
    this->setLayout( global );

    QToolBar* const toolBar = new QToolBar();

    QAction* const slotCreating = new QAction( QIcon( ":/icons/add.png" ), "Add", this );
    QAction* const accepting = new QAction( QIcon( ":/icons/save.png" ), "Accept", this );

    connect( slotCreating, SIGNAL( triggered() ), this, SLOT( addObject() ) );
    connect( accepting, SIGNAL( triggered() ), this, SLOT( accept() ) );

    toolBar->addAction( slotCreating );
    toolBar->addAction( accepting );

    QWidget* const container = new QWidget();
    container->setLayout( slotContainer );
    container->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::MinimumExpanding );
    container->setAutoFillBackground( true );
    container->setBackgroundRole( QPalette::Base );

    slotContainer->addStretch();

    QScrollArea* const scrollable_container = new QScrollArea();
    scrollable_container->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    scrollable_container->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scrollable_container->setWidgetResizable( true );
    scrollable_container->setWidget( container );
    scrollable_container->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    scrollable_container->setLineWidth( 1 );

    global->addWidget( scrollable_container );
    global->addWidget( toolBar );

    cloud.getList().clear();
}


PointCloudComposer::~PointCloudComposer()
{
}


void PointCloudComposer::addObject()
{
    PointCloudComposerSlot* const slot = new PointCloudComposerSlot( server, slotList.size() + 1 );

    slotContainer->insertWidget( slotList.size(), slot );

    slotList.push_back( slot );

    connect( slot, SIGNAL( releaseRequested( PointCloudComposerSlot* ) ), this, SLOT( release( PointCloudComposerSlot* ) ) );
}


void PointCloudComposer::accept()
{
    for( SlotList::iterator it = slotList.begin(); it != slotList.end(); ++it )
    {
        const Carna::base::qt::Object3DChooser& chooser = ( **it ).getObjectChooser();

        if( !chooser.isObject3DSelected() )
        {
            continue;
        }

        const Carna::base::model::Object3D& object = chooser.selectedObject3D();

        cloud.getList().push_back( object.position().toVolumeUnits() );
    }

    // ----------------------------------------------------------------------------------

    QObject* widget = this;

    while( dynamic_cast< ComponentEmbeddable* >( widget ) == 0 )
    {
        widget = widget->parent();
    }

    QTimer::singleShot( 0, widget, SLOT( close() ) );
}


void PointCloudComposer::release( PointCloudComposerSlot* slot )
{
    slot->hide();
    slotContainer->removeWidget( slot );
    
    SlotList::iterator it = std::find( slotList.begin(), slotList.end(), slot );
    slotList.erase( it );

    delete slot;
    slot = nullptr;

    for( unsigned int i = 0; i < slotList.size(); ++i )
    {
        PointCloudComposerSlot& slot = *slotList[ i ];
        slot.setNumber( i + 1 );
    }
}
