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
#include <CRA/Tool.h>
#include <QWidget>
#include "Server.h"

class QComboBox;



// ----------------------------------------------------------------------------------
// ToolChooser
// ----------------------------------------------------------------------------------

/** \brief  Widget which shows existing \c Tool instances and lets the user
  *         choose one or none.
  *
  * \author Leonid Kostrykin
  * \date   19.4.12
  */
class ToolChooser : public QWidget
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      */
    ToolChooser( Record::Server&, QWidget* parent = 0 );


    /** \brief  Tells whether some \c Tool instance is selected.
      */
    bool isToolSelected() const
    {
        return selectedTool != 0;
    }

    /** \brief  References the currently selected \c Tool.
      *
      * \throws std::logic_error    thrown when no \c Tool instance is selected
      */
    CRA::Tool& getSelectedTool() const;


public slots:

    /** \brief	Ensures that no tool is selected.
      *
      * \post <code>isToolSelected() == false</code>
      */
    void selectNone();


private:

    Record::Server& server;

    /** \brief  Internally used widget.
      */
    QComboBox* const cbTools;

    /** \brief  References the currently selected Object3D instance.
      */
    CRA::Tool* selectedTool;

    /** \brief  If \c true, no signals will be emitted when the selection changes.
      */
    bool ignoreSelectionChanges;


private slots:

    /** \brief  Updates the list of \c Tool instances presented to the user.
      */
    void rebuildList();

    /** \brief  Occasionally emits signals if \c ignoreSelectionChanges is not set to
      *         \c true.
      */
    void processSelectionUpdate( int );

    void rebuildList( CRA::Tool& );

    void providerInstalled( const std::string& interfaceID );

    void providerRemoved( const std::string& interfaceID );


signals:

    /** \brief  Some \c Tool has been selected.
      */
    void selectionChanged();

    /** \brief  The given \c Tool has been selected.
      */
    void selectionChanged( CRA::Tool& );

    /** \brief  No \c Tool is selected now.
      */
    void selectionDissolved();

}; // ToolChooser
