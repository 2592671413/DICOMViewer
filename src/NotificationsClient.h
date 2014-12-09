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

#include "Notifications.h"



// ----------------------------------------------------------------------------------
// NotificationsClient
// ----------------------------------------------------------------------------------

/** \brief  Defines the Notifications client.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.7.12
  */
class NotificationsClient : public Record::Client< Notifications >
{

public:
    
    /** \copydoc Record::Client::Client
      */
    NotificationsClient( Record::Server& server )
        : Client( server )
    {
    }


    virtual void connectServiceProvided( QObject* target, const char* slot ) override
    {
        destination.connectServiceProvided( target, slot );
    }

    virtual void connectServiceRemoved( QObject* target, const char* slot ) override
    {
        destination.connectServiceRemoved( target, slot );
    }

}; // NotificationsClient
