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

class ComponentFactory;
class QAction;
class ComponentComposition;
class ComponentWindowFactory;
class ViewWindow;

#include <QObject>
#include <Carna/base/noncopyable.h>



// ----------------------------------------------------------------------------------
// ComponentLauncher
// ----------------------------------------------------------------------------------

/** \brief  Provides a \c QAction for the acquisition of a certain component.
  *
  * \see    \ref GenericComponentFactory
  * \see    \ref ComponentRegister
  * \author Leonid Kostrykin
  * \date   9.3.2011
  */
class ComponentLauncher : public QObject
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      *
      * \param  factory     specifies the factory which creates the component.
      * \param  view_win    specifies the view window which is to take the component.
      * \param  parent      specifies the \c QObject which possesses this launcher.
      */
    ComponentLauncher( const ComponentFactory& factory
                     , ViewWindow& view_win
                     , QObject* parent = nullptr );


    /** \brief  The \c QAction which triggers this launcher
      */
    QAction* const action;

    /** \brief  The component factory which was supplied to the constructor.
      */
    const ComponentFactory& factory;


public slots:

    /** \brief  Triggers this launcher.
      */
    void launch();


private:

    /** \brief  References the view window, which the instantiated component is to associate with.
      */
    ViewWindow& view_window;

}; // ComponentLauncher
