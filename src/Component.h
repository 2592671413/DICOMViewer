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

/** \file   Component.h
  * \brief  Defines ComponentDockable, ComponentWindowFactory, ComponentComposition, Component
  */

#include <QDockWidget>
#include <set>
#include "Server.h"
#include "ComponentEmbeddable.h"
#include "EmbeddablePlacer.h"
#include <Carna/base/noncopyable.h>

class Component;



// ----------------------------------------------------------------------------------
// ComponentDockable
// ----------------------------------------------------------------------------------

/** \brief  \ref Component specific implementation of \c QDockWidget
  *
  * \author Leonid Kostrykin
  * \date   16.2.2011 - 31.7.12
  */
class ComponentDockable : public QDockWidget
{

protected:

    /** \brief  Emits the ComponentDockable::closed signal.
      */
    virtual void closeEvent( QCloseEvent* ev ) override;

}; // ComponentDockable



// ----------------------------------------------------------------------------------
// ComponentWindowFactory
// ----------------------------------------------------------------------------------

/** \brief  \ref ComponentEmbedable and \c QDockWidget factory interface
  *
  * An interface which provides functionality for acquiring context-specifically
  * configured instances of \ref ComponentEmbedable and \c QDockWidget classes.
  *
  * \see    \ref Component
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class ComponentWindowFactory
{

public:

    /** \brief  Default third argument to \ref ComponentWindowFactory::createDockable
      */
    static const QDockWidget::DockWidgetFeatures defaultDockableFeatures;


    /** \brief	Does nothing.
      */
    virtual ~ComponentWindowFactory()
    {
    }


    /** \brief  Creates a ComponentEmbedable object.
      *
      * \param[in]  child           Widget to be put inside of the window
      * \param[in]  placer          Defines how to layout the widget
      * \param[in]  suffix          Suffix to be passed to \ref ComponentEmbedable::ComponentEmbeddable "ComponentEmbeddable constructor"
      * \param[in]  suffixConnector Suffix connector to be passed to \ref ComponentEmbedable::ComponentEmbeddable "ComponentEmbeddable constructor"
      * \param[in]  menuFactory     Builds the custom context menu
      */
    virtual ComponentEmbeddable* createEmbedable( QWidget* child
                                                , EmbeddablePlacer& placer = NullEmbeddablePlacer::instance()
                                                , const QString& suffix = ""
                                                , const QString& suffixConnector = ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
                                                , MenuFactory& menuFactory = NullMenuFactory::instance() ) = 0;

    /** \brief  Creates a \c QDockWidget object.
      *
      * \param[in]  child       Widget to be put inside of the window
      * \param[in]  area        Defines how to layout the widget
      * \param[in]  features    Flags which \c QDockWidget features to use
      */
    virtual ComponentDockable* createDockable( QWidget* child
                                             , Qt::DockWidgetArea area
                                             , QDockWidget::DockWidgetFeatures features
                                                    = defaultDockableFeatures ) = 0;

}; // ComponentWindowFactory



// ----------------------------------------------------------------------------------
// Component
// ----------------------------------------------------------------------------------

/** \brief  View and controller components base class
  *
  * Provides any number of \ref ComponentEmbedable and/or \c QDockWidget objects and
  * also handles the communication between those objects.
  *
  * Common Component base class specializations are \ref CountedComponent and
  * \ref RegistredComponent.
  *
  * \see    \ref Record::Service
  * \see    \ref ComponentComposition
  * \see    \ref ComponentWindowFactory
  * \author Leonid Kostrykin
  * \date   2010-2013
  */
class Component : public QObject
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief  Instantiates.
      *
      * Does not take care of putting oneself into the supplied
      * \ref ComponentComposition. This must be accomplished by the user of this class.
      */
    Component( Record::Server& server, ComponentWindowFactory&, const QString& name = "" );

    /** \brief  Releases all associated windows.
      */
    virtual ~Component();


    /** \brief  Returns the name of this component.
      */
    const QString& getName() const
    {
        return name;
    }


    /** \brief  Creates a \ref ComponentEmbedable object.
      *
      * Creates an embeddable which, when it gets closed by the user, closes the whole
      * component and therefore any window that has been created by either \c createEmbeddable,
      * \c createVitalEmbeddable or \c createDockable. Also removes itself from the
      * \ref ComponentComposition which was supplied to the constructor.
      *
      * \param[in]  child           references the widget which is placed within this window.
      * \param[in]  placer          Defines how to layout the widget
      * \param[in]  titleSuffix     is the suffix which is appended to this window's \ref baseTitle "base title".
      * \param[in]  suffixConnector is the string which is placed in between of the title and it's suffix when the suffix is not empty.
      * \param[in]  menuFactory     defines the context menu creation strategy.
      */
    ComponentEmbeddable& createVitalEmbeddable( QWidget* child
                                              , EmbeddablePlacer& placer = NullEmbeddablePlacer::instance()
                                              , const QString& suffix = ""
                                              , const QString& suffixConnector = ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
                                              , MenuFactory& menuFactory = NullMenuFactory::instance() );

    /** \brief  Creates a ComponentEmbedable object.
      *
      * Creates an embeddable that gets closed when the component is released.
      *
      * \param[in]  child           references the widget which is placed within this window.
      * \param[in]  placer          Defines how to layout the widget
      * \param[in]  titleSuffix     is the suffix which is appended to this window's \ref baseTitle "base title".
      * \param[in]  suffixConnector is the string which is placed in between of the title and it's suffix when the suffix is not empty.
      * \param[in]  menuFactory     defines the context menu creation strategy.
      */
    ComponentEmbeddable& createEmbeddable( QWidget* child
                                         , EmbeddablePlacer& placer = NullEmbeddablePlacer::instance()
                                         , const QString& suffix = ""
                                         , const QString& suffixConnector = ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
                                         , MenuFactory& menuFactory = NullMenuFactory::instance() );

    /** \brief  Creates a \c QDockWidget object.
      *
      * Any QDochWidget created by this method will be closed when the component gets closed.
      *
      * \param[in]  child       Widget to be put inside of the window
      * \param[in]  area        Defines how to layout the widget
      * \param[in]  features    Flags which \c QDockWidget features to use
      */
    ComponentDockable& createDockable( QWidget* child
                                     , Qt::DockWidgetArea area
                                     , QDockWidget::DockWidgetFeatures features = ComponentWindowFactory::defaultDockableFeatures );


public slots:

    /** \brief  Sets the name of this component.
      *
      * Delegates to \ref ComponentEmbeddable::setSuffixedName on all embeddable windows of this component.
      */
    void setName( const QString& name );


    /** \brief  Closes this component.
      *
      * Also removes itself from the \ref ComponentComposition which was supplied to the constructor.
      */
    void close();


protected:

    /** \brief  Invoked when a new name was set.
      *
      * This is \b only done by \c setName but \b not by the constructor.
      */
    virtual void nameChangeEvent( const QString& new_name );


    /** \brief  References the \ref ComponentWindowFactory which was supplied to the constructor.
      */
    ComponentWindowFactory& windowFactory;


    /** \brief  References the model this view operates on.
      */
    Record::Server& server;


signals:

    /** \brief  Emitted when a new name was set.
      *
      * This is \b only done by \c setName but \b not by the constructor.
      */
    void renamed( const QString& new_name );


private:

    /** \brief  Holds the human-readable name of this component.
      */
    QString name;


    /** \brief  Defines a set of embeddable component windows.
      */
    typedef std::set< ComponentEmbeddable* > EmbedableSet;

    /** \brief  Defines a set of dockable component windows.
      */
    typedef std::set<  ComponentDockable* > DockableSet;


    /** \brief  Holds all the embeddable component windows, which belong to this component.
      */
    EmbedableSet embeddables;

    /** \brief  Holds all the dockable component windows, which belong to this component.
      */
    DockableSet  dockables;


private slots:

    /** \brief  Makes the associated component composition release this component.
      *
      * The component composition is supplied to the constructor when the class is instantiated.
      */
    void releaseMyself();

    void removeDestroyedEmbeddable( QObject* );

    void removeDestroyedDockable( QObject* );

}; // Component



// ----------------------------------------------------------------------------------
// CountedComponent
// ----------------------------------------------------------------------------------

/** \brief  Counts instances and appends index to each instance's name.
  *
  * \see    Base class \ref Component
  * \see    Specialized sub class \ref RegistredComponent
  *
  * \param  ConcreteComponent   The component whose instances shall be counted
  *
  * \author Leonid Kostrykin
  * \date   8.3.2011
  */
template< typename ConcreteComponent >
class CountedComponent : public Component
{

public:

    /** \brief  Instantiates.
      *
      * Refer to \ref Component::Component for documentation.
      */
    CountedComponent( Record::Server& server, ComponentWindowFactory& factory, const QString& name = "" )
        : Component
            ( server
            , factory
            , QString( name ).append( QString( " " ).append( QString().setNum( nextID++ ) ) ) )
    {
    }


private:

    /** \brief  Holds the index of next object from this class template's instantiation.
      */
    static unsigned int nextID;

}; // CountedComponent


template< typename ConcreteComponent >
unsigned int CountedComponent< ConcreteComponent >::nextID = 1;
