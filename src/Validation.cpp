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

#include "Validation.h"
#include "PointCloud.h"
#include "Registration.h"
#include "RegistrationClient.h"
#include "RegistrationComponent.h"
#include "RegistrationArguments.h"
#include "RegistrationController.h"
#include <Carna/common.h>



// ----------------------------------------------------------------------------------
// Validation
// ----------------------------------------------------------------------------------

Validation::Validation( Record::Server& server )
    : RegistrationOperation( "Validation", "Compute RMS", server )
{
}


QWidget* Validation::createController()
{
    return nullptr;
}


void Validation::perform( const RegistrationArguments& args
                        , RegistrationController& controller
                        , QWidget* dialogParent )
{
    if( args.recordedPoints.getList().size() != args.virtualPoints.getList().size() )
    {
        throw std::runtime_error( "The point clouds must be of same size." );
    }

    if( !server.hasService( Registration::serviceID ) )
    {
        throw std::runtime_error( "No registration present to be validated." );
    }

    if( &( RegistrationClient( server )->getReferenceBase() ) != &args.referenceBase )
    {
        std::stringstream ss;
        ss << "You cannot validate respectively to another reference base than you performed your registration.";
        ss << std::endl << std::endl;
        ss << "The registration was performed respectively to: " << RegistrationClient( server )->getReferenceBase().getName();
        throw std::runtime_error( ss.str() );
    }

    double square = 0;

    const Carna::Tools::Transformation registration = RegistrationClient( server )->getTransformation();
    for( unsigned int i = 0; i < args.recordedPoints.getList().size(); ++i )
    {
        const PointCloud::Point& recordedPoint = args.recordedPoints.getList()[ i ];
        const PointCloud::Point& virtualPoint = args.virtualPoints.getList()[ i ];
        const PointCloud::Point validatedPoint = registration * recordedPoint;

        const PointCloud::Point difference = validatedPoint - virtualPoint;
        square += difference.dot( difference );
    }

    double mean = square / args.recordedPoints.getList().size();

    double root = std::sqrt( mean );

    controller.setRegistration( args.referenceBase, registration, root );
}
