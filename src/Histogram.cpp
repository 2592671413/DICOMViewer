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

#include "Histogram.h"
#include "HistogramView.h"
#include "HistogramController.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QTimer>



// ----------------------------------------------------------------------------------
// Histogram
// ----------------------------------------------------------------------------------

template< >
const std::string RegistredComponent< Histogram >::name = "Histogram";


Histogram::Histogram( Record::Server& server, ComponentWindowFactory& componentWindowFactory )
    : RegistredComponent< Histogram >( server, componentWindowFactory )
{
    HistogramView* const view = new HistogramView( server );

    QFrame* framedView = new QFrame();
    framedView->setLayout( new QHBoxLayout() );
    framedView->layout()->addWidget( view );
    framedView->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    framedView->setLineWidth( 1 );
    framedView->layout()->setContentsMargins( 0, 0, 0, 0 );

    createVitalEmbeddable( framedView, SingleEmbeddablePlacer::instance() );

    createDockable( new HistogramController( *view )
                  , Qt::RightDockWidgetArea
                  , ComponentWindowFactory::defaultDockableFeatures
                  | QDockWidget::DockWidgetFloatable );

    QTimer::singleShot( 0, view, SLOT( setAutoZoom() ) );
}
