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
#include <QListWidgetItem>
#include <QObject>



// ----------------------------------------------------------------------------------
// ObjectsListItem
// ----------------------------------------------------------------------------------

/** \brief  Represents a \c Carna::Object3D instance within \ref ObjectsList
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class ObjectsListItem : public QObject, public QListWidgetItem
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    ObjectsListItem( Record::Server& server, Carna::Object3D& object );


    /** \brief  References the represented \c Carna::Object3D instance.
      */
    Carna::Object3D& object;


public slots:

    /** \brief  Overloads \c QListWidgetItem as slot.
      */
    void setText( const QString& );


private:

    /** \brief  Shortcut for \c Carna::Object3DEvent
      */
    typedef Carna::Object3DEvent Object3DEvent;


private slots:
    
    /** \brief  Processes an event considering the objects stock.
      */
    void processObjectsEvent( Object3DEvent& );

}; // ObjectsListItem
