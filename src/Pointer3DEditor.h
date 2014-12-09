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

/** \file   Pointer3DEditor.h
  * \brief  Defines Pointer3DEditor.
  */

#include "Object3DEditor.h"
#include <memory>

class Pointer3D;
class Pointer3DCalibration;



// ----------------------------------------------------------------------------------
// Point3DEditor
// ----------------------------------------------------------------------------------

/** \brief  Visual \ref Pointer3D editor
  *
  * \author Leonid Kostrykin
  * \date   9.8.12
  */
class Pointer3DEditor : public Object3DEditor
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    Pointer3DEditor( Pointer3D& editedPointer, QWidget* parent = nullptr );

    /** \brief  Releases acquired resources.
      */
    ~Pointer3DEditor();


    /** \brief  References the edited pointer.
      */
    Pointer3D& editedPointer;


private:

    std::unique_ptr< Pointer3DCalibration > calibration;


private slots:

    void captureAndComputeCalibration();

}; // Pointer3DEditor
