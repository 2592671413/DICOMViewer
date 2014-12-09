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

#include "MPR.h"
#include "ComponentEmbeddable.h"
#include "EmbedAreaArray.h"
#include "CarnaContextClient.h"
#include <Carna/AxialPlane/AxialPlaneVisualization.h>
#include <Carna/AxialPlane/AxialPlaneController.h>
#include <Carna/AxialPlane/AxialPlaneControllerUI.h>
#include <Carna/AxialPlane/IntersectionSynchronisationFactory.h>
#include <Carna/Display.h>
#include <Carna/Scene.h>
#include <QAction>



// ----------------------------------------------------------------------------------
// MPREmbeddablePlacer
// ----------------------------------------------------------------------------------

/** \class  MPREmbeddablePlacer
  * \brief  EmbedablePlacer implementation used by MPR
  *
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class MPREmbeddablePlacer : public EmbeddablePlacer
{

public:

    /** \brief  Lists the components of a three panel view.
      */
    enum View
    {
        front, top, left, dynamic
    };


    /** \brief  Instantiates.
      */
    MPREmbeddablePlacer()
        : bInit( true )
    {
    }

    /** \brief  Pretend that no windows have been placed yet.
      */
    void reset()
    {
        bInit = true;
    }

    /** \brief  Configures which component is going to be placed next.
      *
      * Detailed description.
      */
    void prepareFor( View v )
    {
        view = v;
    }

    virtual void place( Embeddable& em, const GriddedEmbedAreaSet& eas )
    {
        if( bInit )
        {
            if( !( bFloating = ( eas.size() != 4 ) ) )
            {
                for( GriddedEmbedAreaSet::const_iterator
                     it  = eas.begin();
                     it != eas.end();
                   ++it )
                {
                    GriddedEmbedArea& ea = **it;
                    
                    if( ea.hasClient() )
                    {
                        bFloating = true;
                        break;
                    }

                    if( ea.x == 0 && ea.y == 0 ) pTopLeft = &ea;
                    else
                    if( ea.x == 1 && ea.y == 0 ) pTopRight = &ea;
                    else
                    if( ea.x == 0 && ea.y == 1 ) pBottomLeft = &ea;
                    else
                    if( ea.x == 1 && ea.y == 1 ) pBottomRight = &ea;
                }
            }
            bInit = false;
        }

        if( bFloating )
        {
            // TODO: handle case that area array is already occupied
            //   e.g. create tab with a new area, or simply lay out on screen
        }
        else
        {
            switch( view )
            {

            case front:
                pTopLeft->setClient( em );
                break;

            case left:
                pTopRight->setClient( em );
                break;

            case top:
                pBottomLeft->setClient( em );
                break;

            case dynamic:
                if( !pBottomRight->hasClient() )
                {
                    pBottomRight->setClient( em );
                }
                break;

            }
        }
    }


private:

    bool bInit;
    bool bFloating;
    View view;

    EmbedArea* pTopLeft;
    EmbedArea* pTopRight;
    EmbedArea* pBottomLeft;
    EmbedArea* pBottomRight;

}; // ThreePanelViewPlacer



// ----------------------------------------------------------------------------------
// adjustInteractionStrategy
// ----------------------------------------------------------------------------------

static void adjustInteractionStrategy( MPR::View* view )
{
    view->doAfterInitialization( [view]
        {
            static_cast< Carna::AxialPlane::AxialPlaneController& >
                ( view->rawController() )
                .setParentHavingContextMenu( true );
        }
    );
}



// ----------------------------------------------------------------------------------
// MPR
// ----------------------------------------------------------------------------------

template< >
const std::string RegistredComponent< MPR >::name = "MPR";


MPR::MPR( Record::Server& server, ComponentWindowFactory& factory )
    : RegistredComponent< MPR >( server, factory )
    , showDynamicViewAction( new QAction( "Show 3&D View", this ) )
    , controller( new Carna::AxialPlane::AxialPlaneControllerUI( CarnaContextClient( server ).model() ) )
    , embeddablePlacer( new MPREmbeddablePlacer() )
    , dynamicViewWindow( nullptr )
{

 // create main view widgets

    CarnaContextClient carna( server );

    View* const   frontView = new View( carna.model() ),
        * const     topView = new View( carna.model() ),
        * const    leftView = new View( carna.model() );

    frontView->makeFrontView();
    topView  ->makeTopView();
    leftView ->makeLeftView();

    adjustInteractionStrategy( frontView );
    adjustInteractionStrategy( topView );
    adjustInteractionStrategy( leftView );

    mainViews.push_back( frontView );
    mainViews.push_back( topView );
    mainViews.push_back( leftView );

 // create embeddables

    embeddablePlacer->prepareFor( MPREmbeddablePlacer::front );
    createVitalEmbeddable
        ( new Carna::Display( frontView, carna.scene() )
        , *embeddablePlacer
        , "Front"
        , ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
        , *this );

    embeddablePlacer->prepareFor( MPREmbeddablePlacer::top );
    createVitalEmbeddable
        ( new Carna::Display( topView, carna.scene() )
        , *embeddablePlacer
        , "Top"
        , ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
        , *this );

    embeddablePlacer->prepareFor( MPREmbeddablePlacer::left );
    createVitalEmbeddable
        ( new Carna::Display( leftView, carna.scene() )
        , *embeddablePlacer
        , "Left"
        , ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
        , *this );

 // link actions
    
    QObject::connect( showDynamicViewAction, SIGNAL( triggered() )
                    , this, SLOT( showDynamicView() ) );

 // create config widget

    frontView->doAfterInitialization( [&](){ controller->addRenderer( frontView->renderer() ); } );
    topView  ->doAfterInitialization( [&](){ controller->addRenderer( topView  ->renderer() ); } );
    leftView ->doAfterInitialization( [&](){ controller->addRenderer( leftView ->renderer() ); } );

 // create dynamic view

    showDynamicView();

 // finish initialization

    createDockable( controller, Qt::TopDockWidgetArea );

}


void MPR::buildMenu( MenuBuilder& mb )
{
    mb.addAction( *showDynamicViewAction );
}


void MPR::showDynamicView()
{
    if( !dynamicViewWindow )
    {
        View* const dynamicView = new View( CarnaContextClient( server ).model() );
        dynamicView->makeRotatableView();

        embeddablePlacer->prepareFor( MPREmbeddablePlacer::dynamic );
        dynamicViewWindow = &createEmbeddable
            ( new Carna::Display( dynamicView, CarnaContextClient( server ).scene() )
            , *embeddablePlacer
            , "3D"
            , ComponentEmbeddable::DEFAULT_SUFFIX_CONNECTOR
            , *this );

        QObject::connect
            ( dynamicViewWindow
            , SIGNAL( destroyed() )
            , this
            , SLOT( makeDynamicViewShowable() ) );

     // sync intersections

        Carna::AxialPlane::IntersectionSynchronisationFactory intersectionSync;

        initializedRenderersCount = 0;
        const auto onInitialized = [&]
        {
            if( ++initializedRenderersCount == 4 )
            {
                for( auto it = mainViews.begin(); it != mainViews.end(); ++it )
                {
                    intersectionSync.addRenderer( ( **it ).renderer() );
                }
                intersectionSync.addRenderer( dynamicView->renderer() );
                intersectionSync.create();

                dynamicView->renderer().setIntersection( mainViews[ 0 ]->renderer().intersection() );
            }
        };

        for( auto it = mainViews.begin(); it != mainViews.end(); ++it )
        {
            ( **it ).doAfterInitialization( onInitialized );
        }
        dynamicView->doAfterInitialization( onInitialized );

     // update controller

        dynamicView->doAfterInitialization( [&](){ controller->addRenderer( dynamicView->renderer() ); } );
    }

    dynamicViewWindow->raise();
    dynamicViewWindow->activateWindow();

    showDynamicViewAction->setEnabled( false );
}


void MPR::makeDynamicViewShowable()
{
    showDynamicViewAction->setEnabled( true );
    dynamicViewWindow = nullptr;
}
