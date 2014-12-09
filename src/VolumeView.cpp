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

#include "VolumeView.h"
#include "VolumeController.h"
#include "CarnaContextClient.h"
#include <Carna/base/qt/Display.h>
#include <Carna/VolumeRenderings/DefaultVolumeVisualization.h>
#include <Carna/VolumeRenderings/DRR/XRay.h>
#include <Carna/VolumeRenderings/MIP/MaximumIntensityProjection.h>
#include <Carna/VolumeRenderings/DVR/DirectVolumeRendering.h>



// ----------------------------------------------------------------------------------
// VolumeView
// ----------------------------------------------------------------------------------

template< >
const std::string RegistredComponent< VolumeView >::name = "Volume View";


VolumeView::VolumeView( Record::Server& server, ComponentWindowFactory& factory )
    : RegistredComponent< VolumeView >( server, factory )
    , view( new Carna::VolumeRenderings::DefaultVolumeVisualization() )
{
    ComponentEmbeddable& viewWindow = createVitalEmbeddable
        ( new Carna::base::qt::Display( view, CarnaContextClient( server ).scene() )
        , SingleEmbeddablePlacer::instance()
        , "" );

    viewWindow.resize( QSize( 400, 400 ) );

    Carna::VolumeRenderings::DRR::XRay* const xray
        = new Carna::VolumeRenderings::DRR::XRay( CarnaContextClient( server ).model() );
    Carna::VolumeRenderings::MIP::MaximumIntensityProjection* const mip
        = new Carna::VolumeRenderings::MIP::MaximumIntensityProjection();
    Carna::VolumeRenderings::DVR::DirectVolumeRendering* const absorption
        = new Carna::VolumeRenderings::DVR::DirectVolumeRendering( CarnaContextClient( server ).model() );

 // ----------------------------------------------------------------------------------

    VolumeView& self = *this;
    
    view->doAfterInitialization( [&]
        {
            view->renderer().installMode( xray );
            view->renderer().installMode( absorption );
            view->renderer().installMode( mip );

            xray->setWaterAttenuation( 1.7 );
            xray->setBrightness( 1.0 );
            xray->setLowerThreshold( 70 );
            xray->setUpperThreshold( 600 );
            xray->setUpperMultiplier( 2.0 );
            xray->setMaskContrast( 5.0 );

            view->renderer().setMode( *xray );

            VolumeController* const controller = new VolumeController( *view, server, self );

            createDockable( controller, Qt::RightDockWidgetArea );
        }
    );
}
