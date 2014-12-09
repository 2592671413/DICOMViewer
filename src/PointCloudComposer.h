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
#include <QWidget>
#include <vector>

class PointCloud;
class PointCloudComposerSlot;

class QVBoxLayout;



// ----------------------------------------------------------------------------------
// PointCloudComposer
// ----------------------------------------------------------------------------------

/** \brief	Widget which lets the user compose \ref PointCloud "point clouds" out of
  *         \c Carna::Object3D other objects, such as points.
  *
  * \ref PointCloudComposerSlot "Slots" are user-interface elements which represent points.
  *
  * \image  html    PointCloudComposer.png
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 31.5.12
  */
class PointCloudComposer : public QWidget
{

    Q_OBJECT

public:

    /** \brief	Instantiates and clears the given \a pointCloud.
      */
    PointCloudComposer( Record::Server& server, PointCloud& pointCloud, QWidget* parent = 0 );

    /** \brief	Does nothing.
      */
    virtual ~PointCloudComposer();


private:

    /** \brief	References the server.
      */
    Record::Server& server;

    /** \brief	References the point cloud which is being composed.
      */
    PointCloud& cloud;

    /** \brief	References the layout which the slots are put in.
      */
    QVBoxLayout* slotContainer;


    /** \brief	Provides an array list of \ref PointCloudComposerSlot "slots".
      */
    typedef std::vector< PointCloudComposerSlot* > SlotList;

    /** \brief	Lists all existing \ref PointCloudComposerSlot "slots".
      */
    SlotList slotList;


private slots:

    /** \brief	Creates a new \ref PointCloudComposerSlot "slot".
      */
    void addObject();

    /** \brief	Closes this composer and updates the edited point cloud.
      */
    void accept();

    /** \brief	Removes given \ref PointCloudComposerSlot "slot".
      */
    void release( PointCloudComposerSlot* );

}; // PointCloudComposer
