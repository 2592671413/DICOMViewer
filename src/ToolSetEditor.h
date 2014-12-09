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

/** \file   ToolSetEditor.h
  * \brief  Defines ToolSetEditor.
  */

#include "Server.h"
#include <CRA/Tool.h>
#include <set>
#include <QWidget>

class LocalizerProvider;
class ToolEditor;
class LocalizerComponent;

class QVBoxLayout;



// ----------------------------------------------------------------------------------
// ToolSetEditor
// ----------------------------------------------------------------------------------

/** \brief	Container and manager of multiple ToolEditor instances
  *
  * \date   2012
  * \author Leonid Kostrykin
  */
class ToolSetEditor : public QWidget
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    ToolSetEditor( LocalizerComponent& component, Record::Server&, LocalizerProvider&, QWidget* parent = 0 );


public slots:

    /** \brief	Invokes \ref ToolEditor::updateView on all contained ToolEditor instances.
      */
    void updateViews();

    /** \brief	Adds a new \ref ToolEditor instance to the list.
      */
    void addTool();

    /** \brief	Removes all containing \ref ToolEditor instances.
      */
    void clear();

    /** \brief	Saves current configuration to a XML-file prompted from the user.
      *
      * The saved entities are:
      * - all loaded tools
      * - the currently set localizer integrity timeout
      */
    void save();
    
    /** \brief	Loads configuration from XML-file prompted from the user.
      *
      * The loaded entities are:
      * - tool set
      * - localizer integrity timeout
      */
    void load();


    /** \brief	Sets whether the localizer querying is paused.
      */
    void setPaused( bool );
    
    /** \brief	Pauses the localizer querying.
      */
    void pause();
    
    /** \brief	Resumes the localizer querying.
      */
    void resume();


private slots:

    /** \brief	Removes the given ToolEditor instance from the list.
      */
    void remove( ToolEditor* );

    /** \brief	Creates a new ToolEditor instance with given parameters and puts it on the list.
      */
    CRA::Tool& add( const std::string& port, const std::string& name );


private:
    
    /** \brief	References the server.
      */
    Record::Server& server;

    LocalizerComponent& component;
    
    /** \brief	References the localizer service provider.
      */
    LocalizerProvider& localizer;


    /** \brief	References the layout the ToolEditor instances are added to.
      */
    QVBoxLayout* const editors;

    /** \brief	Keeps a list of all ToolEditor instances.
      */
    std::set< ToolEditor* > editorsSet;


    /** \brief	Prompts a tool port from the user and returns it.
      */
    std::string promptPort();


    /** \brief	Holds whether the localizer querying is paused.
      */
    bool paused;

}; // ToolSetEditor
