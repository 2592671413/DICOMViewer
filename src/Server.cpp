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

#include "Server.h"
#include "NotificationsProvider.h"
#include <sstream>
#include <QTimer>

#pragma warning( push )
#pragma warning( disable : 4290 )

namespace Record
{



// ----------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------

Server::Server()
    : notifications( new NotificationsProvider( *this ) )
{
}


Server::~Server()
{
}


Service& Server::queryInterface( const std::string& serviceID ) throw( Exception )
{
    assureProviderExists( serviceID );

    return *getProvider( serviceID );
}


bool Server::hasService( const std::string& serviceID ) const
{
    return getProvider( serviceID ) != 0;
}


void Server::assureProviderExists( const std::string& serviceID ) const throw( Exception )
{
    if( !getProvider( serviceID ) )
    {
        std::stringstream ss;

        ss << "The server does not provide the requested service: \"" << serviceID << "\"";

        throw Exception( ss.str() );
    }
}


void Server::assureProviderNotExists( const std::string& serviceID ) const throw( Exception )
{
    if( getProvider( serviceID ) )
    {
        std::stringstream ss;

        ss << "The service \"" << serviceID << "\" is already provided by the server.";

        throw Exception( ss.str() );
    }
}


void Server::provide( Service& provider ) throw( Exception )
{
    assureProviderNotExists( provider.serviceId() );

    providers[ provider.serviceId() ] = &provider;

    newProviders.push( provider.serviceId() );

    QTimer::singleShot( 0, this, SLOT( processNewProviders() ) );
}


void Server::processNewProviders()
{
    while( !newProviders.empty() )
    {
        const std::string& serviceID = newProviders.front();

        notifications->broadcastProvidedService( serviceID );

        newProviders.pop();
    }
}


void Server::withdraw( Service& provider ) throw( Exception )
{
    assureProviderExists( provider.serviceId() );

    providers.erase( provider.serviceId() );

    notifications->broadcastRemovedService( provider.serviceId() );
}


Service* Server::getProvider( const std::string& serviceID ) const
{
    StringToProvider::const_iterator it = providers.find( serviceID );

    if( it == providers.end() )
    {
        return nullptr;
    }
    else
    {
        return it->second;
    }
}



}  // namespace Record

#pragma warning( pop )
