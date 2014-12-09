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

#include <QObject>
#include <QDebug>

#pragma warning( push )
#pragma warning( disable : 4290 )

#include <Carna/base/noncopyable.h>
#include <map>
#include <queue>

class NotificationsProvider;

namespace Record
{

class Service;
class Server;



// ----------------------------------------------------------------------------------
// Service
// ----------------------------------------------------------------------------------

/** \brief	Server facets base class.
  *
  * \see    \ref Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class Service
{

    NON_COPYABLE

public:

    /** \brief	Does nothing.
      */
    virtual ~Service()
    {
    }

    
    /** \brief	Tells this service's ID.
      */
    virtual const std::string& serviceId() const = 0;

}; // Service



// ----------------------------------------------------------------------------------
// GenericService
// ----------------------------------------------------------------------------------

/** \brief	Server facets base class.
  *
  * \see    \ref Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
template< typename ConcreteService >
class GenericService : public Service
{

public:
    
    /** \brief	Holds this service's ID.
      */
    static const std::string serviceID;


    virtual const std::string& serviceId() const override
    {
        return serviceID;
    }

}; // GenericService



// ----------------------------------------------------------------------------------
// Provider
// ----------------------------------------------------------------------------------

/** \brief	Provides some \ref Service "service interface" implementation.
  *
  * \see    \ref Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
template< typename ConcreteService >
class Provider : public ConcreteService
{

    NON_COPYABLE

public:

    /** \brief	Lists available provider installation methods.
      *
      * \see    \ref Provider::Provider
      */
    enum InstallationMethod
    {
        automaticInstallation,
        manualInstallation
    };


    /** \brief	Installs the server facet provided by this class within the given server.
      *
      * The installation will be skipped if \a installation is not set to \c automaticInstallation.
      * In this case the user will have to perform it on himself, e.g. by:
      *
      * \code
      * server.provide( *this );
      * \endcode
      */
    Provider( Server& server, InstallationMethod installation = automaticInstallation );
    
    /** \brief	Removes the server facet provided by this class from the server.
      *
      * Does nothing if this provider had not been installed, e.g. by supplying \c manualInstallation
      * to the constructor.
      */
    virtual ~Provider();


    /** \brief	References the server.
      */
    Server& server;

}; // Provider


template< typename ConcreteService >
Provider< typename ConcreteService >::Provider( Server& server, InstallationMethod installation )
    : server( server )
{
    if( installation == automaticInstallation )
    {
        server.provide( *this );
    }
}


template< typename ConcreteService >
Provider< typename ConcreteService >::~Provider()
{
    try
    {
        server.withdraw( *this );
    }
    catch( const Server::Exception& ex )
    {
        qDebug() << "Failed to withdraw service provider: " << QString::fromStdString( ex.what() );
    }
}



// ----------------------------------------------------------------------------------
// Server
// ----------------------------------------------------------------------------------

/** \brief	Provides a service mediator (server).
  *
  * The communication within the DICOMViewer 3 and between its components is organized
  * through server-client based approach: The server's instance behaves like a mediator,
  * by connecting \ref Client "service clients" to the appropriate \ref Provider
  * "service providers". The server has many different facets, each defined by some
  * \ref Service "service interface". Some concrete service can be
  * \ref Server::queryInterface "queried from the server" by asking for it's \em service \em ID.
  *
  * \ref Client "Clients" usually do work like proxies: They implement a certain
  * service interface and delegate all calls to the service provider, which is queried
  * from the server.
  *
  * \see    \ref RecordFacets
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class Server : public QObject
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief	Creates a new server.
      */
    Server();

    /** \brief	Destroys this server.
      *
      * Does nothing.
      */
    ~Server();


    /** \brief	Defines the utilized exception class type.
      */
    typedef std::runtime_error Exception;


    /** \brief	Accesses some server facet.
      *
      * \throws Exception   when there is no such facet.
      */
    Service& queryInterface( const std::string& serviceID ) throw( Exception );


    /** \brief	Installs some interface implementation.
      *
      * \throws Exception   when there already is another implementation installed for this interface.
      *
      * \see    \ref withdraw
      */
    void provide( Service& ) throw( Exception );

    /** \brief	Withdraws some interface implementation.
      *
      * \throws Exception   when none implementation is installed for this interface.
      *
      * \see    \ref provide
      */
    void withdraw( Service& ) throw( Exception );

    
    /** \brief	Assures whether an implementation is installed for given interface.
      *
      * \throws Exception   when none implementation is installed for this interface.
      */
    void assureProviderExists( const std::string& serviceID ) const throw( Exception );
    
    /** \brief	Assures whether none implementation is installed for given interface.
      *
      * \throws Exception   when there is some implementation installed for this interface.
      */
    void assureProviderNotExists( const std::string& serviceID ) const throw( Exception );

    
    /** \brief	Tells whether an implementation is installed for given service.
      */
    bool hasService( const std::string& serviceID ) const;


private:

    /** \brief	Maps strings to service providers.
      */
    typedef std::map< std::string, Service* > StringToProvider;

    /** \brief	Provides access to some service provider by it's interface ID.
      */
    StringToProvider providers;


    /** \brief	Defines a queue of strings.
      */
    typedef std::queue< std::string > StringQueue;

    /** \brief	Holds the interfaces which have been installed made available since the last
      *         \ref processNewProviders call.
      */
    StringQueue newProviders;


    /** \brief	References the service provider installed for the specified interface.
      *
      * Returns \c nullptr if there is no provider installed for the given interface.
      */
    Service* getProvider( const std::string& serviceID ) const;


    /** \brief	Holds the notifications service provider.
      */
    const std::unique_ptr< NotificationsProvider > notifications;


private slots:

    /** \brief	Emits the \c providerInstalled signal.
      *
      * Emits the \c providerInstalled signal for every provider, identified by all
      * interfaces picked from \c newProviders.
      */
    void processNewProviders();

}; // Server



// ----------------------------------------------------------------------------------
// Client
// ----------------------------------------------------------------------------------

/** \brief	Provides access to a certain server facet.
  *
  * Implement this class in the manner of the proxy pattern:
  * delegate the interface calls to \ref destination.
  *
  * \see    \ref Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
template< typename ConcreteService >
class Client : public ConcreteService
{

public:

    /** \brief	Obtains \ref destination from the \a server using it's
      *         \ref Server::queryInterface "queryInterface" method.
      */
    Client( Server& server ) throw ( Server::Exception )
        : destination( static_cast< ConcreteService& >( server.queryInterface( serviceId() ) ) )
    {
    }

    /** \brief	Does nothing.
      */
    virtual ~Client()
    {
    }


protected:

    /** \brief	References the server facet's interface implementation.
      */
    ConcreteService& destination;

}; // Client



}  // namespace Record

#pragma warning( pop )
