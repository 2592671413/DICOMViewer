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

#include "SlicePlane.h"
#include "CarnaContextClient.h"
#include <Carna/SlicePlanes/DefaultSlicePlaneVisualization.h>
#include <Carna/SlicePlanes/SlicePlaneControllerUI.h>
#include <Carna/base/qt/Display.h>



// ----------------------------------------------------------------------------------
// SlicePlane
// ----------------------------------------------------------------------------------

template< >
const std::string RegistredComponent< SlicePlane >::name = "Slice Plane View";


SlicePlane::SlicePlane( Record::Server& server, ComponentWindowFactory& componentWindowFactory )
    : RegistredComponent< SlicePlane >( server, componentWindowFactory )
{
    Carna::SlicePlanes::SlicePlaneVisualization* const view = new Carna::SlicePlanes::DefaultSlicePlaneVisualization();

    view->doAfterInitialization( [&]
        {
            Carna::SlicePlanes::SlicePlaneControllerUI* const controller
                = new Carna::SlicePlanes::SlicePlaneControllerUI( view->renderer() );
            createDockable( controller, Qt::RightDockWidgetArea );
        }
    );

    createVitalEmbeddable
        ( new Carna::base::qt::Display( view, CarnaContextClient( server ).scene() )
        , SingleEmbeddablePlacer::instance() );
}
