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

/** \file   RegistredComponent.h
  * \brief  Defines ComponentFactory, GenericComponentFactory, ComponentRegister, ComponentRegistrator, RegistredComponent
  */

#include "Component.h"
#include <Carna/base/Singleton.h>



// ----------------------------------------------------------------------------------
// ComponentFactory
// ----------------------------------------------------------------------------------

/** \brief  Defines an interface for \ref Component class factories.
  *
  * \see    \ref GenericComponentFactory
  * \see    \ref ComponentRegister
  * \author Leonid Kostrykin
  * \date   9.3.2011
  */
class ComponentFactory
{

public:

    /** \brief  Defines an unordered set of component factories.
      */
    typedef std::set< ComponentFactory* > Set;


    /** \brief  Does nothing.
      */
    virtual ~ComponentFactory()
    {
    }

    /** \brief  Instantiates the associated component class.
      */
    virtual Component* createComponent( Record::Server& server, ComponentWindowFactory& factory ) const = 0;

    /** \brief  Returns the associated component class' name or description.
      */
    virtual const std::string& getName() const = 0;

}; // ComponentFactory



// ----------------------------------------------------------------------------------
// GenericComponentFactory
// ----------------------------------------------------------------------------------

/** \brief  Generic implementation of \ref ComponentFactory 
  *
  * \param  ConcreteComponent    specifies the component class which is to instantiate.
  *
  * \see    \ref ComponentRegister
  * \author Leonid Kostrykin
  * \date   9.3.2011
  */
template< typename ConcreteComponent >
class GenericComponentFactory : public ComponentFactory
{

public:

    /** \brief  Instantiates the associated component class, which is \a ConcreteComponent.
      */
    virtual Component* createComponent( Record::Server& server, ComponentWindowFactory& factory ) const
    {
        return new ConcreteComponent( server, factory );
    }

    /** \brief  Returns the associated component class' name or description.
      *
      * \see    RegistredComponent::name
      */
    virtual const std::string& getName() const
    {
        return ConcreteComponent::name;
    }

}; // GenericComponentFactory



// ----------------------------------------------------------------------------------
// ComponentRegister
// ----------------------------------------------------------------------------------

/** \brief  Singleton which holds a set of component factories.
  *
  * \see    \ref ComponentFactory
  * \author Leonid Kostrykin
  * \date   9.3.2011
  */
class ComponentRegister : public Carna::base::Singleton< ComponentRegister >
{

public:

    /** \brief  Represents the set of registered components.
      */
    ComponentFactory::Set factories;

}; // ComponentRegister



// ----------------------------------------------------------------------------------
// ComponentRegistrator
// ----------------------------------------------------------------------------------

/** \brief  Creates and registers a component factory.
  *
  * \param  ConcreteComponent   specifies the component which is a factory to create for.
  *
  * Acts like a \em static \em constructor for \ref RegistredComponent implementations.
  *
  * \see    \ref ComponentRegister
  * \see    \ref GenericComponentFactory
  * \author Leonid Kostrykin
  * \date   9.3.2011
  */
template< typename ConcreteComponent >
class ComponentRegistrator
{

public:

    /** \brief  Creates and registers component factory for \a ConcreteComponent.
      */
    ComponentRegistrator()
    {
        ComponentFactory* const factory = new GenericComponentFactory< ConcreteComponent >();
        ComponentRegister::instance().factories.insert( factory );
    }

}; // ComponentRegistrator



// ----------------------------------------------------------------------------------
// RegistredComponent
// ----------------------------------------------------------------------------------

/** \brief  Components which are derived from this class do register themselves.
  *
  * If you derive from this class, lets say your class' name is \c MyComponent, you will
  * have to define the static attribute \c name for \c MyComponent - see
  * \ref RegistredComponent::name for details.
  *
  * \see    \ref ComponentRegister
  * \author Leonid Kostrykin
  * \date   9.3.2011
  */
template< typename ConcreteComponent >
class RegistredComponent : public CountedComponent< ConcreteComponent >
{

public:

    /** \copydoc Component::Component
      */
    RegistredComponent( Record::Server& server, ComponentWindowFactory& factory )
        : CountedComponent< ConcreteComponent >( server, factory, QString::fromStdString( name ) )
    {
        Q_UNUSED( __registrator );
    }


    /** \brief  To be defined to hold this components' name when sub classing.
      *
      * Think of it like a \em "pure-virtual static attribute".
      *
      * \code
      * class MyComponent : public RegistredComponent< MyComponent >
      * {
      *     MyComponent( Record::Server& server, ComponentWindowFactory& factory )
      *         : RegistredComponent< MyComponent >( server, factory )
      *     {
      *     }
      * };
      *
      * template< >
      * const std::string RegistredComponent< MyComponent >::name = "My Component";
      * \endcode
      */
    static const std::string name;


private:

    /** \brief  Static constructor
      */
    static ComponentRegistrator< ConcreteComponent > __registrator;

}; // RegistredComponent


template< typename ConcreteComponent >
ComponentRegistrator< ConcreteComponent > RegistredComponent< ConcreteComponent >::__registrator;
