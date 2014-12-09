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

#include "Localizer.h"



// ----------------------------------------------------------------------------------
// LocalizerClient
// ----------------------------------------------------------------------------------

/** \brief	Defines the localizer service client.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class LocalizerClient : public Record::Client< Localizer >
{

public:
    
    /** \copydoc Record::Client::Client
      */
    LocalizerClient( Record::Server& server )
        : Client( server )
    {
    }


    virtual CRA::Localizer& getLocalizer() override
    {
        return destination.getLocalizer();
    }

    virtual const ToolList& getTools() const override
    {
        return destination.getTools();
    }

    virtual void connectToolAdded( QObject* receiver, const char* slot ) override
    {
        destination.connectToolAdded( receiver, slot );
    }

    virtual void connectToolRemoved( QObject* receiver, const char* slot ) override
    {
        destination.connectToolRemoved( receiver, slot );
    }

}; // LocalizerClient
