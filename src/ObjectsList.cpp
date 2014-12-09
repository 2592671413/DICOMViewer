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

#include "ObjectsList.h"
#include "ObjectsListItem.h"
#include "CarnaContextClient.h"
#include <Carna/base/model/Scene.h>
#include <Carna/base/model/Object3DEvent.h>
#include <QVBoxLayout>
#include <QApplication>



// ----------------------------------------------------------------------------------
// ObjectsList :: Details
// ----------------------------------------------------------------------------------

class ObjectsList::Details : public QListWidget
{

public:


    Details( ObjectsList& self )
        : self( self )
    {
    }


    std::vector< ObjectsListItem* > items;

    int itemIndexByObject( Carna::base::model::Object3D& ) const;

    static const int ITEM_NOT_IN_LIST = -1;


    bool doesObjectExist( Carna::base::model::Object3D& ) const;


private:

    ObjectsList& self;

}; // ObjectsList :: Details


int ObjectsList::Details::itemIndexByObject( Carna::base::model::Object3D& object ) const
{
    for( auto it = items.begin(); it != items.end(); ++it )
    {
        ObjectsListItem* const item = static_cast< ObjectsListItem* >( *it );
        if( &( item->object ) == &object )
        {
            return it - items.begin();
        }
    }

    return ITEM_NOT_IN_LIST;
}


bool ObjectsList::Details::doesObjectExist( Carna::base::model::Object3D& object ) const
{
    Carna::base::model::Scene& model = CarnaContextClient( self.server ).model();
    for( unsigned int i = 0; i < model.objectsCount(); ++i )
    {
        const Carna::base::model::Object3D& existingObject = model.objectByIndex( i );
        if( &existingObject == &object )
        {
            return true;
        }
    }
    return false;
}



// ----------------------------------------------------------------------------------
// ObjectsList
// ----------------------------------------------------------------------------------

ObjectsList::ObjectsList( Record::Server& server, QWidget* parent )
    : QWidget( parent )
    , server( server )
    , pimpl( new Details( *this ) )
{
    connect( &( CarnaContextClient( server ).model() ), SIGNAL( objectsChanged( Carna::base::model::Object3DEvent& ) ), this, SLOT( processObjectsEvent( Carna::base::model::Object3DEvent& ) ) );
    connect( pimpl.get(), SIGNAL( itemSelectionChanged() ), this, SLOT( processChangedSelection() ) );
    connect( pimpl.get(), SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( processDoubleClickedItem( QListWidgetItem* ) ) );

    pimpl->setSelectionMode( QAbstractItemView::ExtendedSelection );

    this->setLayout( new QVBoxLayout() );
    this->layout()->setContentsMargins( 0, 0, 0, 0 );
    this->layout()->addWidget( pimpl.get() );
}


ObjectsList::~ObjectsList()
{
    this->layout()->removeWidget( pimpl.get() );
}


void ObjectsList::fetchSelectedObjects( std::vector< Carna::base::model::Object3D* >& selectedObjects ) const
{
    selectedObjects.clear();

    const QList< QListWidgetItem* >& items = pimpl->selectedItems();
    for( auto it = items.begin(); it != items.end(); ++it )
    {
        ObjectsListItem* const item = static_cast< ObjectsListItem* >( *it );
        selectedObjects.push_back( &( item->object ) );
    }
}


void ObjectsList::selectNone()
{
    pimpl->clearSelection();
}


void ObjectsList::fetchObjects( std::vector< Carna::base::model::Object3D* >& objects ) const
{
    objects.clear();

    for( auto it = pimpl->items.begin(); it != pimpl->items.end(); ++it )
    {
        ObjectsListItem* const item = static_cast< ObjectsListItem* >( *it );
        objects.push_back( &( item->object ) );
    }
}


void ObjectsList::addObject( Carna::base::model::Object3D& object )
{
    if( pimpl->itemIndexByObject( object ) != Details::ITEM_NOT_IN_LIST )
    {
        return;
    }

    ObjectsListItem* const item = new ObjectsListItem( server, object );
    pimpl->addItem( item );
    pimpl->items.push_back( item );
}
    

void ObjectsList::removeObject( Carna::base::model::Object3D& object )
{
    while( true )
    {
        const int itemIndex = pimpl->itemIndexByObject( object );
        if( itemIndex != Details::ITEM_NOT_IN_LIST )
        {
            delete pimpl->takeItem( itemIndex );
            pimpl->items.erase( pimpl->items.begin() + itemIndex );
        }
        else
        {
            break;
        }
    }
}


void ObjectsList::clear()
{
    pimpl->clear();
    pimpl->items.clear();
}


void ObjectsList::processChangedSelection()
{
    emit selectionChanged();
}


void ObjectsList::processDoubleClickedItem( QListWidgetItem* item )
{
    emit objectDoubleClicked( static_cast< ObjectsListItem* >( item )->object );
}


void ObjectsList::processObjectsEvent( Carna::base::model::Object3DEvent& ev )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    if( ev.mightAffect( Carna::base::model::Object3DEvent::existence ) )
    {
        bool restart;
        do 
        {
            restart = false;
            for( auto it = pimpl->items.begin(); it != pimpl->items.end(); ++it )
            {
                Carna::base::model::Object3D& object = ( **it ).object;
                if( !pimpl->doesObjectExist( object ) )
                {
                    ObjectsList::removeObject( object );
                    restart = true;
                    break;
                }
            }
        }
        while( restart );
    }

    QApplication::restoreOverrideCursor();
}
