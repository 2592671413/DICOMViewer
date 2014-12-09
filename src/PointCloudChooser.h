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

#include <Carna/base/noncopyable.h>
#include <QWidget>
#include "Server.h"

class PointCloud;

class QComboBox;



// ----------------------------------------------------------------------------------
// PointCloudChooser
// ----------------------------------------------------------------------------------

/** \brief  Widget which shows existing \ref PointCloud instances and lets the user
  *         choose one or none.
  *
  * \author Leonid Kostrykin
  * \date   24.4.12
  */
class PointCloudChooser : public QWidget
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      */
    PointCloudChooser( Record::Server&, QWidget* parent = 0 );


    /** \brief  Tells whether a \ref PointCloud instance is selected.
      */
    bool isPointCloudSelected() const
    {
        return selectedPointCloud != 0;
    }

    /** \brief  References the currently selected \ref PointCloud.
      *
      * \throws std::logic_error    thrown when no \ref PointCloud instance is selected
      */
    PointCloud& getSelectedPointCloud() const;


private:

    /** \brief	References the server.
      */
    Record::Server& server;

    /** \brief  Internally used widget.
      */
    QComboBox* const cbPointClouds;

    /** \brief  References the currently selected PointCloud instance.
      */
    PointCloud* selectedPointCloud;

    /** \brief  If \c true, no signals will be emitted when the selection changes.
      */
    bool ignoreSelectionChanges;


private slots:

    /** \brief  Updates the list of \ref PointCloud instances presented to the user.
      */
    void rebuildList();

    /** \brief  Occasionally emits signals if \c ignoreSelectionChanges is not set to
      *         \c true.
      */
    void processSelectionUpdate( int );
    
    /** \brief	Invoked when a new point cloud has been created.
      *
      * Rebuilds the \ref cbPointClouds "point clouds list" by calling \ref rebuildList.
      */
    void rebuildList( PointCloud& );


signals:

    /** \brief  Some \ref PointCloud has been selected.
      */
    void selectionChanged();

    /** \brief  The given \ref PointCloud has been selected.
      */
    void selectionChanged( PointCloud& );

    /** \brief  No \ref PointCloud is selected now.
      */
    void selectionDissolved();

}; // PointCloudChooser
