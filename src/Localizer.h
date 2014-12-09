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
#if 0
#include <CRA/Registration.h>
#endif
#include <CRA/Localizer.h>
#include <CRA/Tool.h>
#include <deque>



// ----------------------------------------------------------------------------------
// Localizer
// ----------------------------------------------------------------------------------

/** \brief	Defines the localizer service.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class Localizer : public Record::GenericService< Localizer >
{

public:
    
    /** \brief	Defines list of tool representing objects.
      */
    typedef std::deque< CRA::Tool* > ToolList;


    /** \brief	References the CRA localizer.
      */
    virtual CRA::Localizer& getLocalizer() = 0;

    /** \brief	References list of tools.
      */
    virtual const ToolList& getTools() const = 0;

    virtual void connectToolAdded( QObject* receiver, const char* slot ) = 0;

    virtual void connectToolRemoved( QObject* receiver, const char* slot ) = 0;

}; // Localizer


template< >
const std::string Record::GenericService< Localizer >::serviceID = "Localizer";
