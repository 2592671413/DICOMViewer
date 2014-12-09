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
#include <Carna/CarnaException.h>
#include <CRA/DefaultLocalizer.h>
#include <CRA/LazyLocalizer.h>
#include <set>



// ----------------------------------------------------------------------------------
// LocalizerProvider
// ----------------------------------------------------------------------------------

/** \brief  Defines localizer service provider.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 30.7.12
  */
class LocalizerProvider : public QObject, public Record::Provider< Localizer >
{

    Q_OBJECT

public:
    
    /** \copydoc Record::Provider::Provider
      */
    LocalizerProvider( Record::Server& server, const std::string& iniFile )
        : Provider( server, Provider::manualInstallation )
        , localizer( iniFile )
        , localizerProxy( localizer )
    {
        server.provide( *this );
    }


    virtual CRA::Localizer& getLocalizer() override
    {
        return localizerProxy;
    }

    virtual const ToolList& getTools() const override
    {
        return tools;
    }

    virtual void connectToolAdded( QObject* receiver, const char* slot ) override
    {
        connect( this, SIGNAL( toolAdded( CRA::Tool& ) ), receiver, slot );
    }

    virtual void connectToolRemoved( QObject* receiver, const char* slot ) override
    {
        connect( this, SIGNAL( toolRemoved( CRA::Tool& ) ), receiver, slot );
    }


    void loadFromIni( const std::string& iniFile )
    {
        localizer.loadFromIni( iniFile );
    }

    void setLazynessThreshold( unsigned int ms )
    {
        localizerProxy.setLazynessThreshold( ms );
    }

    unsigned int getLazynessThreshold() const
    {
        return localizerProxy.getLazynessThreshold();
    }


    void addTool( CRA::Tool& rb )
    {
        CARNA_ASSERT( std::find( tools.begin(), tools.end(), &rb ) == tools.end() );

        tools.push_back( &rb );

        emit toolAdded( rb );
    }

    void removeTool( CRA::Tool& rb )
    {
        ToolList::iterator it = std::find( tools.begin(), tools.end(), &rb );

        CARNA_ASSERT( it != tools.end() );
        CARNA_ASSERT( std::find( it + 1, tools.end(), &rb ) == tools.end() );

        tools.erase( it );

        emit toolRemoved( rb );
    }


private:

    /** \brief	Holds the CRA localizer object.
      */
    CRA::DefaultLocalizer localizer;

    /** \brief	Lazy proxy to \ref localizer.
      */
    CRA::LazyLocalizer localizerProxy;

    /** \brief	Holds all known tools.
      */
    ToolList tools;


signals:

    /** \brief	New tool has been provided.
      */
    void toolAdded( CRA::Tool& rb );

    /** \brief	Tool has been removed.
      */
    void toolRemoved( CRA::Tool& rb );

}; // LocalizerProvider
