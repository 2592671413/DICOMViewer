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

#pragma once

/** \file   ObjectsList.h
  * \brief  Defines ObjectsList.
  */

#include "Server.h"
#include <Carna/Carna.h>
#include <QWidget>

class QListWidget;
class QListWidgetItem;



// ----------------------------------------------------------------------------------
// ObjectsList
// ----------------------------------------------------------------------------------

/** \brief  Widget which lists \c Carna::base::model::Object3D instances.
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class ObjectsList : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    ObjectsList( Record::Server&, QWidget* parent = nullptr );

    /** \brief  Does nothing.
      */
    virtual ~ObjectsList();

    /** \brief  References the server.
      */
    Record::Server& server;


    /** \brief  Writes the currently selected objects to the given list.
      */
    void fetchSelectedObjects( std::vector< Carna::base::model::Object3D* >& ) const;

    /** \brief  Writes all objects to the given list.
      */
    void fetchObjects( std::vector< Carna::base::model::Object3D* >& ) const;


public slots:
    
    /** \brief  Adds the given object to the list.
      *
      * Each object may only be added once.
      * If the given object already is on the list, nothing happens.
      */
    virtual void addObject( Carna::base::model::Object3D& );
    
    /** \brief  Removes the given object from the list.
      */
    virtual void removeObject( Carna::base::model::Object3D& );

    /** \brief  Removes all objects from the list.
      */
    virtual void clear();

    /** \brief  Invalidates selection.
      */
    void selectNone();


signals:

    /** \brief  The objects selection has changed.
      */
    void selectionChanged();

    /** \brief  An object has been double clicked.
      */
    void objectDoubleClicked( Carna::base::model::Object3D& );


protected:

    /** \brief  Shortcut for \c Carna::base::model::Object3DEvent
      */
    typedef Carna::base::model::Object3DEvent Object3DEvent;


protected slots:
    
    /** \brief  Processes an event considering the objects stock.
      */
    virtual void processObjectsEvent( Object3DEvent& );


private:

    /** \brief  Holds the implementation details.
      */
    class Details;

    /** \brief  PIMPL idiom
      */
    std::unique_ptr< Details > pimpl;


private slots:

    /** \brief  Emits the \ref selectionChanged signal.
      */
    void processChangedSelection();
    
    /** \brief  Emits the \ref objectDoubleClicked signal.
      */
    void processDoubleClickedItem( QListWidgetItem* );

}; // ObjectsList
