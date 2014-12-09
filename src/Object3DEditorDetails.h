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

/** \file   Object3DEditorDetails.h
  * \brief  Defines Object3DEditor::Details.
  */

#include "Object3DEditor.h"
#include <Carna/base/Transformation.h>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>

class QDockWidget;



// ----------------------------------------------------------------------------------
// Object3DEditor :: Details
// ----------------------------------------------------------------------------------

class Object3DEditor :: Details : public QObject
{

    Q_OBJECT

public:

    /** \brief  Holds the minimum a spin box value must be changed for the associated
      *         coordinate to be updated when \ref currentUnits "unit" is set to
      *         \c millimeters.
      */
    const static double MIN_STEP_MILLIMETERS;

    /** \brief  Holds the minimum a spin box value must be changed for the associated
      *         coordinate to be updated when \ref currentUnits "unit" is set to
      *         \c volumeUnits.
      */
    const static double MIN_STEP_VOLUMEUNITS;


    /** \brief  Lists supported units.
      */
    enum Units
    {
        millimeters = 0,
        volumeUnits  = 1
    };

    /** \brief  Instantiates.
      */
    Details( Object3DEditor& self );

    /** \brief  References the UI element which lets the user switch among the
      *         available \ref Units "units".
      */
    QComboBox* const cbUnit;

    /** \brief  References the spin box which provides access to the \f$x\f$
      *         coordinate of the edited object's position.
      */
    QDoubleSpinBox* const sbPositionX;

    /** \brief  References the spin box which provides access to the \f$y\f$
      *         coordinate of the edited object's position.
      */
    QDoubleSpinBox* const sbPositionY;

    /** \brief  References the spin box which provides access to the \f$z\f$
      *         coordinate of the edited object's position.
      */
    QDoubleSpinBox* const sbPositionZ;

    QLabel* const laHUV;

    /** \brief  Holds the unit, the coordinates in the UI are provided in.
      */
    Units currentUnits;


    /** \brief  References the dock of the lastly added page.
      */
    QDockWidget* lastAddedDock;


private:

    /** \brief  References the editor, these implementation details belong to.
      */
    Object3DEditor& self;

    /** \brief  Holds the current position of the edited object in the
      *         \ref currentUnits "currently specified units".
      */
    Carna::base::Vector currentPosition;


    /** \brief  Configures the given spin box to correspond to the currently
      *         set units.
      */
    void configure( QDoubleSpinBox*, float singleStepInVolumeSpace );

    /** \brief  Returns \ref MIN_STEP_MILLIMETERS if the \ref currentUnits
      *         "current unit" is set to \c millimeters or
      *         \ref SINGLE_STEP_VOLUMEUNITS if the current unit is set to
      *         \c volumeUnits.
      */
    double getCurrentEpsilon() const;

    /** \brief  Updates the given coordinate of the edited object's position,
      *         if it is going to change by at least
      *         \ref getCurrentEpsilon "a certain minimum".
      */
    void setCurrentCoordinate( double value, int coordinate );


private slots:

    /** \brief  Updates \ref currentUnits.
      */
    void setUnits( int );

    /** \brief  Updates the UI elements' values.
      */
    void updatePosition();

    /** \brief  Delegates to \ref setCurrentCoordinate.
      */
    void setPositionX( double );
    
    /** \brief  Delegates to \ref setCurrentCoordinate.
      */
    void setPositionY( double );
    
    /** \brief  Delegates to \ref setCurrentCoordinate.
      */
    void setPositionZ( double );

}; // Object3DEditor :: Details
