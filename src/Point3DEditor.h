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

/** \file   Point3DEditor.h
  * \brief  Defines Point3DEditor.
  */

#include "Object3DEditor.h"



// ----------------------------------------------------------------------------------
// Point3DEditor
// ----------------------------------------------------------------------------------

/** \brief  Visual \c Carna::base::view::Point3D editor
  *
  * \author Leonid Kostrykin
  * \date   8.8.12
  */
class Point3DEditor : public Object3DEditor
{

public:

    /** \brief  Instantiates.
      */
    Point3DEditor( Carna::base::view::Point3D& editedObject, QWidget* parent = nullptr );


    /** \brief  References the edited point.
      */
    Carna::base::view::Point3D& editedPoint;

}; // Point3DEditor
