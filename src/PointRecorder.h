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
#include <Carna/base/Transformation.h>
#include <QWidget>
#include <deque>

class ToolChooser;
class PointRecorderView;
class PointCloud;



// ----------------------------------------------------------------------------------
// PointRecorder
// ----------------------------------------------------------------------------------

/** \brief	Widget which allows the user to record \ref PointCloud "point clouds"
  *         using the \ref LocalizerInterface "localizer service".
  *
  * \image  html    PointRecorder.png
  *
  * \author Leonid Kostrykin
  * \date   2012
  */
class PointRecorder : public QWidget
{

    Q_OBJECT

public:

    PointRecorder( Record::Server& server, PointCloud&, QWidget* parent = 0 );

    virtual ~PointRecorder();


private:

    Record::Server& server;

    PointCloud& cloud;

    ToolChooser* const pointerSelector,
                            * const baseSelector;

    PointRecorderView* const view;

    QAction* const shotting,
           * const recording,
           * const accepting;

    std::unique_ptr< QTimer > shotTimer;

    double minimumPositionDelta;


    bool isToolSelectionValid() const;

    void fetchRelativeBearing( Carna::base::Transformation& ) const;


private slots:

    void updateEnability();

    void shot();

    void recordSingleShot();

    void toggleRecording( bool );

    void beginRecording();

    void endRecording();

    void accept();

}; // PointRecorder
