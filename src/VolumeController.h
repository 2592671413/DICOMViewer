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
#include <Carna/Carna.h>
#include <CRA/LinkedObject.h>
#include <CRA/LinkedCamera.h>

class VolumeView;
class ToolChooser;
class VolumeViewCameraController;

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QSpinBox;



// ----------------------------------------------------------------------------------
// VolumeController
// ----------------------------------------------------------------------------------

/** \brief  Provides \c Carna::VolumeController with some extra options.
  *
  * \see    VolumeView
  * \author Leonid Kostrykin
  * \date   17.7.12 - 24.4.13
  */
class VolumeController : public QWidget
{

    Q_OBJECT

public:

    enum PreferredCameraMode
    {
        perspectiveProjection = 0,
        orthogonalProjection = 1,
        undefinedPreferredProjection = 2
    };


    VolumeController( Carna::VolumeVisualization& view, Record::Server& server, VolumeView& tool );

    virtual ~VolumeController();

    VolumeView& tool;


public slots:

    /** \brief
      * If \c CRA::LinkedCamera is currently being used,
      * it is replaced by new instance of \c Carna::DefaultCamera through \ref releaseLinkedCamera.
      * Otherwise the current camera is replaced by new instance of \c Carna::DefaultCamera through \ref setDefaultCamera.
      */
    void resetCamera();


private:

    Carna::VolumeVisualization& view;

    Record::Server& server;

    VolumeViewCameraController* const cameraController;


    QComboBox* const cbCameraChooser;

    ToolChooser* const linkedCameraChooser;

    std::unique_ptr< CRA::LinkedCamera > linkedCamera;

    PreferredCameraMode preferredCameraMode;

    QCheckBox* const cbAutoRotate;

    QDoubleSpinBox* const sbSecondsPerRotation;

    QSpinBox* const sbTargetFramesPerSecond;

    
    QComboBox* const cbRenderMode;

    enum RenderMode
    {
        monoscopic = 0,
        philips = 1,
        zalman = 2
    };


    QPushButton* const buGulsun;


    /** \brief
      * Creates new \c Carna::DefaultCamera instance and utilizes it as current camera.
      */
    void setDefaultCamera();

    /** \brief
      * If \c CRA::LinkedCamera is currently being used, and \c CRA::Registration is provided,
      * the camera-link is removed from the registration. Otherwise, nothing happens.
      */
    void unlinkCamera();


private slots:

    void processAddedService( const std::string& serviceID );

    void processRemovedService( const std::string& serviceID );


    /** \brief
      * Creates new \c CRA::LinkedCamera and utilizes it as current camera.
      * The newly created camera is also linked with the supplied tool.
      * If no \c CRA::Registration is provided, nothing happens.
      */
    void setLinkedCamera( CRA::Tool& );

    /** \brief
      * If \c CRA::LinkedCamera is currently being used,
      * than first it is unlinked through \ref unlinkCamera,
      * second \c Carna::DefaultCamera is utilized through \ref setDefaultCamera
      * and finally the \c CRA::LinkedCamera is destroyed.
      */
    void releaseLinkedCamera();


    void setRenderMode( int );

    void setPreferredCameraMode( int );

    /** \brief
      * Updates the projection mode of the current camera if it is instance of \c Carna::DefaultCamera.
      * Otherwise, nothing happens.
      */
    void updateCamera();

    void openGulsun();

    void gulsunClosed();

}; // VolumeController
