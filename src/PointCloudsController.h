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

/** \file   PointCloudsController.h
  * \brief  Defines PointCloudsController.
  */

#include "Server.h"
#include <QWidget>

class PointCloud;
class PointCloudsComponent;
class ComponentEmbeddable;

class QListWidget;
class QAction;



// ----------------------------------------------------------------------------------
// PointCloudsController
// ----------------------------------------------------------------------------------

/** \brief	Widget which lets the user manage \ref PointCloud "point clouds".
  *
  * \image  html    PointCloudsController.png
  *
  * \see    \ref PointCloudsComponent
  * \author Leonid Kostrykin
  * \date   30.3.12 - 18.7.12
  */
class PointCloudsController : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    PointCloudsController( Record::Server& server, PointCloudsComponent& component );

    /** \brief  Does nothing.
      */
    virtual ~PointCloudsController();


public slots:

    /** \brief	Saves selected point cloud.
      */
    void savePointCloud();
    
    /** \brief	Loads some point cloud from a file, prompted from the user.
      */
    void loadPointCloud();
    
    /** \brief	Removes the selected point cloud.
      */
    void releasePointCloud();
    
    /** \brief	Renames selected point cloud to a name prompted from the user.
      */
    void renamePointCloud();
    
    /** \brief	Presents details of selected point cloud to the user.
      */
    void showCloudDetails();


private:

    /** \brief	References the server.
      */
    Record::Server& server;

    /** \brief	References the component which which this controller belongs to.
      */
    PointCloudsComponent& component;

    /** \brief	Widget which lists all loaded point clouds.
      */
    QListWidget* pointCloudsListWidget;


    QAction* const cloudSaving;     ///< \brief  Saves selected point cloud.
    QAction* const cloudLoading;    ///< \brief  Loads some point cloud from a file, prompted from the user.
    QAction* const cloudReleasing;  ///< \brief  Removes the selected point cloud.
    QAction* const cloudRenaming;   ///< \brief  Renames selected point cloud to a name prompted from the user.
    QAction* const cloudDetails;    ///< \brief  Presents details of selected point cloud to the user.
#ifndef NO_CRA
    QAction* const cloudRecording;  ///< \brief  Opens a new \ref PointRecorder "point recorder dialog".
#endif
    QAction* const cloudExtracting; ///< \brief  Opens a new \ref SurfaceExtractionDialog "surface extraction dialog".
    QAction* const cloudBuilding;   ///< \brief  Opens a new \ref PointCloudCreator "point cloud composer".
    QAction* const object3dCreation;///< \brief  Creates a new \ref PointCloud3D "visual representation" of the selected point cloud.
    QAction* const point3dCreation; ///< \brief  Creates one \c Carna::base::view::Point3D instance for each point within the selected point cloud.

#ifndef NO_CRA
    /** \brief	References the \ref PointRecorder "point recorder dialog" if it is open or is \c nullptr.
      */
    ComponentEmbeddable* recorderWindow;
#endif
    
    /** \brief	References the \ref PointCloudCreator "point cloud composer" if it is open or is \c nullptr.
      */
    ComponentEmbeddable* cloudCreatorWindow;
    
    /** \brief	References the \ref SurfaceExtractionDialog "surface extraction dialog" if it is open or is \c nullptr.
      */
    ComponentEmbeddable* surfaceExtractionWindow;


    /** \brief	References the currently selected point cloud or gives \c nullptr if none is selected.
      */
    PointCloud* getSelectedPointCloud();


private slots:

    /** \brief	Rebuilds the \ref pointCloudsListWidget "point clouds list" by calling \ref updateListWidget.
      *
      * Invoked when a new point cloud has been created.
      */
    void processAddedPointCloud( PointCloud& );
    
    /** \brief	Rebuilds the \ref pointCloudsListWidget "point clouds list" by calling \ref updateListWidget.
      *
      * Invoked when a point cloud is about to be removed.
      */
    void processRemovedPointCloud( PointCloud& );

    /** \brief	Updates the enabled-state of this controller's actions based on whether some point cloud is selected or not.
      */
    void pointCloudSelectionChanged();

#ifndef NO_CRA

    /** \brief	Invoked when a \ref PointRecorder "point recorder dialog" is closed.
      *
      * Enables this controller and sets \ref recorderWindow to \c nullptr.
      *
      * \see    \ref recordPointCloud
      */
    void pointRecorderClosed();

#endif
    
    /** \brief	Invoked when a \ref PointCloudCreator "point cloud composer" is closed.
      *
      * Enables this controller and sets \ref cloudCreatorWindow to \c nullptr.
      *
      * \see    \ref createFrom3dObjects
      */
    void cloudCreatorClosed();
    
    /** \brief	Invoked when a \ref SurfaceExtractionDialog "surface extraction dialog" is closed.
      *
      * Enables this controller and sets \ref surfaceExtractionWindow to \c nullptr.
      *
      * \see    \ref extractSurface
      */
    void surfaceExtractorClosed();

    
    /** \brief	Rebuilds the \ref pointCloudsListWidget "point clouds list".
      */
    void updateListWidget();

    
#ifndef NO_CRA

    /** \brief	 Opens a new \ref PointRecorder "point recorder dialog".
      *
      * Disables this controller until the dialog is closed.
      *
      * \see    \ref pointRecorderClosed
      */
    void recordPointCloud();

#endif
    
    /** \brief	Opens a new \ref SurfaceExtractionDialog "surface extraction dialog".
      *
      * Disables this controller until the dialog is closed.
      *
      * \see    \ref surfaceExtractorClosed
      */
    void extractSurface();
    
    /** \brief	Creates a new \ref PointCloud3D "visual representation" of the selected point cloud.
      */
    void create3dObject();
    
    /** \brief	Creates one \c Carna::base::view::Point3D instance for each point within the selected point cloud.
      */
    void create3dPoints();
    
    /** \brief	Opens a new \ref PointCloudCreator "point cloud composer".
      *
      * Disables this controller until the dialog is closed.
      *
      * \see    \ref cloudCreatorClosed
      */
    void createFrom3dObjects();

}; // PointCloudsController
