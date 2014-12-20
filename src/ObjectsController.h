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

/** \file   ObjectsController.h
  * \brief  Defines ObjectsController.
  */

#include "Server.h"
#include <Carna/Carna.h>
#include <QWidget>

class Object3D;
class Object3DEditor;
class ObjectsComponent;
class ObjectsView;
class Embeddable;
class ComponentEmbeddable;

class QAction;
class QFrame;



// ----------------------------------------------------------------------------------
// ObjectsController
// ----------------------------------------------------------------------------------

/** \brief	Widget which lets the user manage \c Carna::base::model::Object3D instances.
  *
  * \see    \ref ObjectsComponent
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class ObjectsController : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    ObjectsController( Record::Server& server, ObjectsComponent& component );

    /** \brief  Does nothing.
      */
    virtual ~ObjectsController();


public slots:
    
    /** \brief	Removes all selected objects.
      */
    void releaseObjects();
    
    /** \brief	Renames the selected object to a name prompted from the user.
      */
    void renameObject();
    
    /** \brief	Renames the given object to a name prompted from the user.
      */
    void renameObject( Carna::base::model::Object3D& );
    
    /** \brief	Presents details of the selected object to the user.
      *
      * If more than one object is selected, the details are presented for the first selected object.
      */
    void openObjectEditor();
    
    /** \brief	Presents details of the given object to the user.
      */
    void openObjectEditor( Carna::base::model::Object3D& );

    /** \brief  Closes the object editor if it is open.
      */
    void closeObjectEditor();

    /** \brief  If an editor is open, it is put into a stand-alone window.
      */
    void detachEditor();

    /** \brief  Instantiates \c Carna::base::view::Point3D.
      */
    void createPoint3D();
    
#ifndef NO_CRA

    /** \brief  Instantiates \ref Pointer3D.
      */
    void createPointer3D();

#endif

    /** \brief  Instantiates \c Carna::base::view::Polyline.
      */
    void importPolyline();

    void createTempPoint1();


private:

    /** \brief	References the server.
      */
    Record::Server& server;

    /** \brief	References the component which which this controller belongs to.
      */
    ObjectsComponent& component;

    /** \brief	Widget which lists all loaded \c Carna::base::model::Object3D instances.
      */
    ObjectsView* const view;


    QAction* const objectReleasing; ///< \brief  Removes all selected objects.
    QAction* const objectRenaming;  ///< \brief  Renames the selected object to a name prompted from the user.
    QAction* const editorDetaching; ///< \brief  Invokes \ref detachEditor when triggered.

    QAction* const pointCreation;   ///< \brief  Instantiates \c Carna::base::view::Point3D when triggered.
    QAction* const pointerCreation; ///< \brief  Instantiates \ref Pointer3D when triggered.
    QAction* const polylineImport;  ///< \brief  Imports \c Carna::base::view::Polyline when triggered.

    QAction* const tempPointCreation;


    /** \brief  UI elements which holds the \ref currentObjectEditor "currently open object editor".
      */
    QFrame* const editorContainer;

    /** \brief  References the currently open object editor.
      *
      * \see    \ref openObjectEditor, \ref closeObjectEditor, \ref detachEditor
      */
    Object3DEditor* currentObjectEditor;

    
    /** \brief  Creates an editor for the given object and returns it.
      */
    Object3DEditor* createObjectEditor( Carna::base::model::Object3D& );


private slots:

    /** \brief	Updates the enabled-state of this controller's actions based on the current selection.
      */
    void objectsSelectionChanged();

}; // ObjectsController
