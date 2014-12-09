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

/** \file   Object3DEditorFactory.h
  * \brief  Defines Object3DEditorFactory.
  */

#include <Carna/Carna.h>

class Object3DEditor;



// ----------------------------------------------------------------------------------
// Object3DEditorFactory
// ----------------------------------------------------------------------------------

/** \brief  Creates \ref Object3DEditor instances based on the requested editor type.
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class Object3DEditorFactory
{

public:

    /** \brief  Lists available editor types.
      */
    enum EditorType
    {
        genericEditor,
        pointEditor,
        pointCloudEditor,
        pointerEditor
    };


    /** \brief  Instantiates.
      */
    Object3DEditorFactory( EditorType requiredEditorType );


    /** \brief  Holds the editor type that shall be created by this factory when \ref create is called.
      *
      * The value of this attribute is specified by the constructor.
      */
    const EditorType createdEditorType;


    /** \brief  Creates an editor from \ref createdEditorType "specified type".
      */
    Object3DEditor* create( Carna::base::model::Object3D& ) const;

}; // Object3DEditorFactory
