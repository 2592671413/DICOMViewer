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

#include "Server.h"
#include <Carna/Carna.h>
#include <QWidget>

class QSpinBox;



// ----------------------------------------------------------------------------------
// SurfaceExtractionDialog
// ----------------------------------------------------------------------------------

/** \brief	Provides UI for surface segmentation.
  *
  * \image  html    SurfaceExtractionDialog.png
  *
  * The points, extracted from a CT dataset, visualized:
  *
  * \image  html    PointCloud3D.png
  *
  * \author Leonid Kostrykin
  * \date   5.6.12
  */
class SurfaceExtractionDialog : public QWidget
{

    Q_OBJECT

public:

    SurfaceExtractionDialog( Record::Server&, QWidget* parent = 0 );

    virtual ~SurfaceExtractionDialog();


private:

    Record::Server& server;

    Carna::Object3DChooser* const seedPointSelector;

    QSpinBox* const sbHuv0;

    QSpinBox* const sbHuv1;


private slots:

    void run();

}; // SurfaceExtractionDialog
