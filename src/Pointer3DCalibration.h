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

#include <Carna/Transformation.h>
#include <QObject>
#include <vector>
#include <memory>



// ----------------------------------------------------------------------------------
// Pointer3DCalibration
// ----------------------------------------------------------------------------------

/** \brief  ...
  *
  * \author Leonid Kostrykin
  * \date   9.8.12
  */
class Pointer3DCalibration : public QObject
{

    Q_OBJECT

public:

    Pointer3DCalibration( Carna::RotatableObject3D& pointer );

    ~Pointer3DCalibration();


    Carna::RotatableObject3D& pointer;


    /** \brief  Tells the (normalized) calibrated shaft direction.
      *
      * \throws std::logic_error when the calibration has not been computed yet and
      *                          cannot be computed since too few points have been
      *                          captured.
      */
    const Carna::Tools::Vector& getShaftDirection();

    /** \brief  Tells the (normalized) calibrated shaft direction.
      *
      * \throws std::logic_error when the calibration has not been computed yet.
      */
    const Carna::Tools::Vector& getShaftDirection() const;


public slots:

    /** \brief  Captures the current pointer position.
      */
    void capture();
    
    /** \brief  Computes the result.
      *
      * \throws std::logic_error when too few points have been captured.
      *
      * \see    \ref reset, \ref getShaftDirection
      */
    void compute();

    void reset();


signals:

    void computed( const Carna::Tools::Vector& );


private:

    std::unique_ptr< Carna::Tools::Vector > shaftDirection;

    std::vector< Carna::Tools::Vector > capturedPoints;

}; // Pointer3DCalibration
