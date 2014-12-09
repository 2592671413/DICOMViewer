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
#include <Carna/SlicePlaneVisualization.h>
#include <Carna/SlicePlaneControllerUI.h>
#include <Carna/Display.h>



// ----------------------------------------------------------------------------------
// SlicePlane
// ----------------------------------------------------------------------------------

template< >
const std::string RegistredComponent< SlicePlane >::name = "Slice Plane View";


SlicePlane::SlicePlane( Record::Server& server, ComponentWindowFactory& componentWindowFactory )
    : RegistredComponent< SlicePlane >( server, componentWindowFactory )
{
    Carna::SlicePlaneVisualization* const view = new Carna::SlicePlaneVisualization();

    view->doAfterInitialization( [&]
        {
            Carna::SlicePlaneControllerUI* const controller = new Carna::SlicePlaneControllerUI( view->renderer() );
            createDockable( controller, Qt::RightDockWidgetArea );
        }
    );

    createVitalEmbeddable
        ( new Carna::Display( view, CarnaContextClient( server ).scene() )
        , SingleEmbeddablePlacer::instance() );
}
