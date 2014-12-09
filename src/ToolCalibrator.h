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

/** \file   ToolCalibrator.h
  * \brief  Defines ToolCalibrator.
  */

#include "Server.h"
#include <CRA/Tool.h>
#include <TRTK/PivotCalibration.hpp>
#include <QWidget>
#include <memory>

class LocalizerProvider;

class QTimer;
class QLabel;



// ----------------------------------------------------------------------------------
// ToolCalibrator
// ----------------------------------------------------------------------------------

/** \brief  ...
  *
  * \author Leonid Kostrykin
  * \date   9.8.12
  */
class ToolCalibrator : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    ToolCalibrator( Record::Server& server, CRA::Tool& rb );

    /** \brief  Releases acquired resources.
      */
    ~ToolCalibrator();


    /** \brief  References the server.
      */
    Record::Server& server;

    /** \brief  References the edited pointer.
      */
    CRA::Tool& tool;


private:

    typedef TRTK::PivotCalibration< double > Calibration;
    typedef Calibration::Matrix3T Matrix;
    typedef Calibration::Vector3T Vector;

    std::unique_ptr< QTimer > shotTimer;

    std::vector< Matrix > rotations;
    std::vector< Vector > locations;

    QLabel* const laState;


private slots:

    void capture();

    void begin();

    void end();

    void setCapturing( bool on );

}; // ToolCalibrator
