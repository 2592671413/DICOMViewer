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

/** \file   ObjectsView.h
  * \brief  Defines ObjectsView.
  */

#include "ObjectsList.h"



// ----------------------------------------------------------------------------------
// ObjectsView
// ----------------------------------------------------------------------------------

/** \brief  Widget which lists all currently present \c Carna::Object3D instances.
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class ObjectsView : public ObjectsList
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    ObjectsView( Record::Server&, QWidget* parent = nullptr );


public slots:
    
    /** \brief  Does nothing.
      */
    virtual void addObject( Carna::Object3D& ) override;
    
    /** \brief  Does nothing.
      */
    virtual void removeObject( Carna::Object3D& ) override;
    
    /** \brief  Does nothing.
      */
    virtual void clear() override;


protected:

    /** \brief  Shortcut for \c Carna::Object3DEvent
      */
    typedef Carna::Object3DEvent ObjectsView::Objects3DEvent;


protected slots:

    /** \brief  Processes an event considering the objects stock.
      */
    virtual void processObjectsEvent( Objects3DEvent& ) override;


private:

    /** \brief  Completes the missing objects within the list.
      */
    void completeList();

}; // ObjectsView
