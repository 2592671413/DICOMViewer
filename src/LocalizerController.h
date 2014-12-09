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

/** \file   LocalizerComponent.h
  * \brief  Defines LocalizerController.
  */

#include "Server.h"
#include <QWidget>

class LocalizerComponent;
class LocalizerProvider;
class ToolSetEditor;
class QDoubleSpinBox;
class QAction;
class QTimer;



// ----------------------------------------------------------------------------------
// LocalizerController
// ----------------------------------------------------------------------------------

/** \brief	Provides an user-interface for localizer configuration.
  *
  * \date   2012
  * \author Leonid Kostrykin
  */
class LocalizerController : public QWidget
{

    Q_OBJECT

public:

    /** \brief  Instantiates.
      */
    LocalizerController( LocalizerComponent& component, Record::Server& server );

    /** \brief  Does nothing.
      */
    virtual ~LocalizerController();


private:

    /** \brief	References the server.
      */
    Record::Server& server;

    /** \brief  References the component this controller belongs to.
      */
    LocalizerComponent& component;

    /** \brief	References the localizer service provider.
      */
    std::auto_ptr< LocalizerProvider > localizer;

    /** \brief	Provides or withdraws the localizer service provider.
      */
    QAction* const connectionToggling;

    /** \brief	Creates tool.
      */
    QAction* const addingTool;

    /** \brief	Saves currently loaded tools and cache integrity timeout to XML file.
      */
    QAction* const saving;

    /** \brief	Loads tools and cache integrity timeout from XML file.
      */
    QAction* const loading;

    QAction* const pausing;

    /** \brief	Configures the localizer cache integrity timeout in milliseconds.
      */
    QDoubleSpinBox* const sbCacheIntegrityTimeout;

    /** \brief	References the tools editor's user-interface.
      */
    ToolSetEditor* toolSetEditor;

    /** \brief	Queries the localizer every time it's cache integrity has timed out.
      *
      * Querying is done by calling \ref ToolSetEditor::updateViews.
      */
    std::auto_ptr< QTimer > viewUpdateTimer;


    /** \brief	Updates the user-interface, depending on whether the localizer service
      *         is provided or not.
      *
      * This is told by the \a ready argument.
      */
    void requestState( bool ready );


private slots:

    /** \brief	Supplies the localizer service provider, using the configuration loaded
      *         from an INI file prompted from the user.
      */
    void loadFromIni();

    /** \brief	Sets the localizer cache integrity timeout to the given \a timeout in seconds.
      */
    void setCacheIntegrityTimeout( double timeout );

    /** \brief	Withdraws the localizer service provider.
      */
    void shutDown();

    /** \brief	Loads tools and cache integrity timeout from XML file.
      *
      * The loading is delegated to \ref ToolSetEditor::load if this controller's
      * \ref requestState "user-interface is in ready-state". Else, nothing happens.
      *
      * \post
      * This controller's user-interface is updated to the cache integrity timeout loaded.
      */
    void load();
    
    /** \brief	Provides or withdraws the localizer service provider.
      */
    void toggleConnection();

}; // LocalizerController
