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

/** \file   ToolEditor.h
  * \brief  Defines ToolEditor.
  */

#include "Server.h"
#include <CRA/ToolByPort.h>
#include <Carna/Carna.h>
#include <CRA/LinkedObject.h>
#include <QWidget>

class LocalizerProvider;
class LocalizerComponent;
class ComponentEmbeddable;

class QPushButton;
class QLabel;



// ----------------------------------------------------------------------------------
// ToolEditor
// ----------------------------------------------------------------------------------

/** \brief	User-interface representation of a tool.
  *
  * \date   2012
  * \author Leonid Kostrykin
  */
class ToolEditor : public QWidget
{

    Q_OBJECT

public:

    /** \brief	Creates a new tool.
      */
    ToolEditor( LocalizerComponent& component, Record::Server&, const std::string& port, const std::string& name, LocalizerProvider&, QWidget* parent = 0 );

    /** \brief	Releases acquired resources, including the tool.
      */
    ~ToolEditor();


    /** \brief	References the tool managed by this editor.
      */
    const CRA::ToolByPort& getTool() const
    {
        return tool;
    }

    /** \brief	References the tool managed by this editor.
      */
    CRA::ToolByPort& getTool()
    {
        return tool;
    }


public slots:

    /** \brief	Queries data from associated tool and updates the user-interface.
      */
    void updateView();

    /** \brief	Emits the \ref removeRequested signal with the intention to be removed
      *         by a \ref ToolSetEditor object.
      */
    void removeMyself();


private:

    /** \brief	References the server.
      */
    Record::Server& server;

    LocalizerComponent& component;

    /** \brief	Holds the tool.
      */
    CRA::ToolByPort tool;

    /** \brief	References the localizer service provider.
      */
    LocalizerProvider& localizer;


    /** \brief	User-interface element
      */
    QLabel* const laVisibility;
    
    /** \brief	User-interface element
      */
    QLabel* const laPort;
    
    /** \brief	User-interface element
      */
    QPushButton* const buName;
    
    /** \brief	User-interface element
      */
    Carna::base::qt::Object3DChooser* objectChooser;


    /** \brief	Holds the description of the scene element which the represented tool has been linked to lastly.
      */
    std::auto_ptr< CRA::LinkedObject > recentlyLinkedObject;

    ComponentEmbeddable* calibratorWindow;

    bool recentlyVisible;


    /** \brief	Type of a Carna 3D-object.
      */
    typedef Carna::base::model::Object3D Object3D;


private slots:

    /** \brief	Renames the represented tool to the given name.
      */
    void setName( const std::string& );
    
    /** \brief	Renames the represented tool to a name prompted from the user.
      */
    void rename();

    /** \brief	Links the represented tool to a scene element.
      */
    void setLinkedObject3D( Object3D& );
    
    /** \brief	Releases the link of the represented tool with a scene element.
      */
    void removeLinkedObject3D();

    /** \brief  Enables linking to scene elements for the represented tool if
      *         registration service is provided.
      */
    void processProvidedService( const std::string& );
    
    /** \brief  Disables linking to scene elements for the represented tool if
      *         registration service is removed.
      */
    void processRemovedService( const std::string& );


    void calibrate();

    void calibrationClosed();


signals:
    
    /** \brief	Posts an intention to be removed by a \ref ToolSetEditor object.
      */
    void removeRequested( ToolEditor* );

}; // ToolEditor
