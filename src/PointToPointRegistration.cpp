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

#include "PointToPointRegistration.h"
#include "PointCloud.h"
#include "RegistrationComponent.h"
#include "RegistrationArguments.h"
#include "RegistrationController.h"
#include <TRTK/EstimateRigidTransformation3D.hpp>



// ----------------------------------------------------------------------------------
// PointToPointRegistration
// ----------------------------------------------------------------------------------

PointToPointRegistration::PointToPointRegistration( Record::Server& server )
    : RegistrationOperation( "Point To Point Registration", "Register", server )
{
}


QWidget* PointToPointRegistration::createController()
{
    return nullptr;
}


void PointToPointRegistration::perform( const RegistrationArguments& args
                                      , RegistrationController& controller
                                      , QWidget* dialogParent )
{
    if( args.recordedPoints.getList().size() != args.virtualPoints.getList().size() )
    {
        throw std::runtime_error( "The point clouds must be of same size." );
    }

    TRTK::EstimateRigidTransformation3D< double > trafo( args.recordedPoints.getList(), args.virtualPoints.getList() );

    trafo.compute();

    Carna::base::Transformation result( trafo.getTransformationMatrix() );

    controller.setRegistration( args.referenceBase, trafo.getTransformationMatrix(), trafo.getRMS() );
}
