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



// ----------------------------------------------------------------------------------
// Notifications
// ----------------------------------------------------------------------------------

/** \brief	Defines the interface of the notifications service.
  *
  * \see    \ref Record::Server
  *
  * \author Leonid Kostrykin
  * \date   30.7.12
  */
class Notifications : public Record::GenericService< Notifications >
{

public:

    virtual void connectServiceProvided( QObject* target, const char* slot ) = 0;

    virtual void connectServiceRemoved( QObject* target, const char* slot ) = 0;

}; // Notifications


template< >
const std::string Record::GenericService< Notifications >::serviceID = "Notifications";
