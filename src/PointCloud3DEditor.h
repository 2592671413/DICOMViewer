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

/** \file   PointCloud3DEditor.h
  * \brief  Defines PointCloud3DEditor.
  */

#include "Object3DEditor.h"

class PointCloud3D;



// ----------------------------------------------------------------------------------
// PointCloud3DEditor
// ----------------------------------------------------------------------------------

/** \brief  Visual \c Carna::Point3D editor
  *
  * \author Leonid Kostrykin
  * \date   8.8.12
  */
class PointCloud3DEditor : public Object3DEditor
{

public:

    /** \brief  Instantiates.
      */
    PointCloud3DEditor( PointCloud3D& editedObject, QWidget* parent = nullptr );


    /** \brief  References the edited point cloud.
      */
    PointCloud3D& editedPointCloud;

}; // PointCloud3DEditor
