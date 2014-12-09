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

/** \file   Object3DEditor.h
  * \brief  Defines Object3DEditor.
  */

#include <Carna/Carna.h>
#include <QMainWindow>



// ----------------------------------------------------------------------------------
// Object3DEditor
// ----------------------------------------------------------------------------------

/** \brief  Visual \c Carna::base::model::Object3D editor
  *
  * \author Leonid Kostrykin
  * \date   7.8.12
  */
class Object3DEditor : public QMainWindow
{

public:

    /** \brief  Instantiates.
      */
    Object3DEditor( Carna::base::model::Object3D& editedObject, QWidget* parent = nullptr );

    /** \brief  Releases acquired resources.
      */
    ~Object3DEditor();


    /** \brief  References the server.
      */
    Carna::base::model::Object3D& editedObject;


    /** \brief  Adds given page to this editor.
      */
    void addPage( QWidget* );


private:

    /** \brief  \ref Object3DEditor implementation details
      *
      * \author Leonid Kostrykin
      * \date   7.8.12
      */
    class Details;

    /** \brief  PIMPL idiom
      */
    std::unique_ptr< Details > pimpl;

}; // Object3DEditor
