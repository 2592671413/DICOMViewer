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

/** \file   MainWindow.h
  * \brief  Defines MainWindow.
  */

#include "Component.h"
#include <QMainWindow>
#include <QThread>
#include <vector>
#include <Carna/base/noncopyable.h>
#include <Carna/Carna.h>

class QTabWidget;
class QStackedWidget;

class MainWindow;
class ViewWindow;
class CarnaContextProvider;
class ComponentsProvider;
class PointCloudsProvider;
class CarnaModelFactory;



// ----------------------------------------------------------------------------------
// GlobalComponentWindowFactory
// ----------------------------------------------------------------------------------

/** \class  GlobalComponentWindowFactory
  * \brief  \ref ComponentWindowFactory implementation used by MainWindow
  *
  * Windows created by this factory are visible in each \ref ViewWindow "view tab".
  *
  * \see    ViewWindow
  * \see    MainWindow
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class GlobalComponentWindowFactory : public ComponentWindowFactory
{

public:

    /** \brief	Instantiates.
      */
    GlobalComponentWindowFactory( MainWindow& window );

    virtual ComponentEmbeddable* createEmbedable( QWidget* child
                                                , EmbeddablePlacer&
                                                , const QString& suffix
                                                , const QString& suffixConnector
                                                , MenuFactory& menuFactory ) override;

    virtual ComponentDockable* createDockable( QWidget* child
                                             , Qt::DockWidgetArea area
                                             , QDockWidget::DockWidgetFeatures features ) override;


private:

    /** \brief	References the window which shall contain the constructed
      *         embeddables and dockables.
      */
    MainWindow& window;

}; // GlobalComponentWindowFactory



// ----------------------------------------------------------------------------------
// MainWindow
// ----------------------------------------------------------------------------------

/** \brief  Central application window.
  *
  * Provides multiple tabs. Tabs can be added or removed by simple user interaction.
  * Each tab contains an instance of ViewWindow, offering up to four EmbedArea
  * instances:
  *
  * \image  html    mainwindow.png
  *
  * Each instance is composed with a \ref Record::Server "server", which organizes
  * the data model the application operates on the internal communication between the
  * application's components. Either none or exactly one server is associated at a
  * certain time. They can be loaded and unloaded through the user interface.
  *
  * \see    \ref GlobalComponentWindowFactory
  * \see    \ref ViewWindow
  * \author Leonid Kostrykin
  * \date   2010-2012
  */
class MainWindow : public QMainWindow
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates
      */
    MainWindow( QWidget* parent = 0, Qt::WFlags flags = 0 );

    /** \brief  Releases the data model.
      */
    ~MainWindow();


private:

    QStackedWidget* const content;

    /** \brief  References the tab widget which holds the ViewWindow instances.
      */
    QTabWidget* tabWidget;

    /** \brief  Holds the index which will be used by the next acquired view tab.
      */
    unsigned int nextTabId;


    CarnaModelFactory* const carnaModelFactory;

    /** \brief	Holds the factory which constructs component windows.
      */
    GlobalComponentWindowFactory componentWindowFactory;


    /** \brief	Exports DICOM data.
      */
    QAction* const exporting;

    /** \brief	Normalizes volume data.
      */
    QAction* const normalizing;

    /** \brief  Masks volume data.
      */
    QAction* const masking;

    /** \brief	Releases current data model.
      */
    QAction* const closing;

    /** \brief	Acquires new view tab.
      */
    QAction* const acquiringViewTab;

    /** \brief	Acquires the localizer component.
      */
    QAction* const acquiringLocalizer;

    /** \brief	Acquires the record info component.
      */
    QAction* const acquiringModelInfo;

    /** \brief	Acquires the \c Carna::base::model::Object3D management component.
      */
    QAction* const acquiringObjectsManager;

    /** \brief	Acquires the point clouds management component.
      */
    QAction* const acquiringPointClouds;

    /** \brief	Acquires the registrations management component.
      */
    QAction* const acquiringRegistration;

    QAction* const acquiringGulsun;

    QAction* const maskExporting;

    QAction* const maskImporting;


    /** \brief	References the record service.
      */
    Record::Server server;

    /** \brief	Holds the carna context service provider.
      */
    std::unique_ptr< CarnaContextProvider > carna;
    
    /** \brief	Holds the components service provider.
      */
    std::unique_ptr< ComponentsProvider > components;
    
    /** \brief	Holds the point clouds service provider.
      */
    std::unique_ptr< PointCloudsProvider > pointClouds;


private slots:

    /** \brief	Initializes the user interface from a newly created Carna model.
      */
    void init( Carna::base::model::Scene* );


    /** \brief  Releases current data model.
      */
    void closeRecord();

    /** \brief  Exports DICOM data.
      */
    void exportRecord();
    
    /** \brief  Normalizes the volume data.
      */
    void normalize();

    /** \brief  Masks the volume data.
      */
    void mask();

    /** \brief  Closes the application.
      */
    void exit();

    /** \brief  Acquires new view tab.
      */
    ViewWindow* acquireViewTab();

    /** \brief  Closure of certain view tab has been requested by the user.
      */
    void tabCloseRequested( int index );

    /** \brief  Acquires the localizer component.
      *
      * Disables user interface option for localizer component acquisition.
      */
    void acquireLocalizer();

    /** \brief  Indicates that the localizer component has been released.
      *
      * Enabled user interface option for localizer component acquisition.
      */
    void localizerReleased();

    /** \brief  Acquires the record info component.
      *
      * Disables user interface option for record info component acquisition.
      */
    void acquireModelInfo();

    /** \brief  Acquires the \c Carna::base::model::Object3D management component.
      *
      * Disables user interface option for \c Carna::base::model::Object3D management component acquisition.
      */
    void acquireObjectsManager();

    /** \brief  Indicates that the record info component has been released.
      *
      * Enabled user interface option for record info component acquisition.
      */
    void modelInfoReleased();

    /** \brief  Indicates that the \c Carna::base::model::Object3D management component has been released.
      *
      * Enabled user interface option for \c Carna::base::model::Object3D management component acquisition.
      */
    void objectsManagerReleased();

    /** \brief	Acquires the point clouds management component.
      *
      * Disables user interface option for point clouds management component acquisition.
      */
    void acquirePointClouds();
    
    /** \brief  Indicates that the point clouds management component has been released.
      *
      * Enabled user interface option for point clouds management component acquisition.
      */
    void pointCloudsReleased();
    
    /** \brief	Acquires the registrations management component.
      *
      * Disables user interface option for registrations management component acquisition.
      */
    void acquireRegistration();
    
    /** \brief  Indicates that the registrations management component has been released.
      *
      * Enabled user interface option for registrations management component acquisition.
      */
    void registrationReleased();

    void acquireGulsun();

    void gulsunReleased();

    void exportMask();

    void importMask();

    void updateMaskExporting();


    /** \brief  Opens an \ref OptionsDialog instance.
      */
    void showSettings();


protected:

    /** \brief  Closes the application.
      */
    void closeEvent( QCloseEvent* );

}; // MainWindow
