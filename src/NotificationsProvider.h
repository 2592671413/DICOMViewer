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
#include <QObject>



// ----------------------------------------------------------------------------------
// NotificationsProvider
// ----------------------------------------------------------------------------------

/** \brief  Defines the Notifications service provider.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.7.12
  */
class NotificationsProvider : public QObject, public Record::Provider< Notifications >
{

    Q_OBJECT

public:
    
    /** \copydoc Record::Client::Client
      */
    NotificationsProvider( Record::Server& server, QObject* parent = nullptr )
        : QObject( parent )
        , Provider( server )
    {
    }


    void broadcastProvidedService( const std::string& serviceID )
    {
        emit serviceProvided( serviceID );
    }

    void broadcastRemovedService( const std::string& serviceID )
    {
        emit serviceRemoved( serviceID );
    }


    virtual void connectServiceProvided( QObject* target, const char* slot ) override
    {
        connect( this, SIGNAL( serviceProvided( const std::string& ) ), target, slot );
    }

    virtual void connectServiceRemoved( QObject* target, const char* slot ) override
    {
        connect( this, SIGNAL( serviceRemoved( const std::string& ) ), target, slot );
    }


signals:

    void serviceProvided( const std::string& serviceID );

    void serviceRemoved( const std::string& serviceID );

}; // NotificationsProvider
