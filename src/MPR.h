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

/** \file   MPR.h
  * \brief  Defines MPR.
  */

#include "RegistredComponent.h"
#include <Carna/Carna.h>

class MPREmbeddablePlacer;



// ----------------------------------------------------------------------------------
// MPR
// ----------------------------------------------------------------------------------

/** \brief  Defines multi planar reformat (MPR) component.
  *
  * \image  html    MPR.png
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 25.3.13
  */
class MPR : public RegistredComponent< MPR >, private MenuFactory
{

    Q_OBJECT

public:

    typedef Carna::AxialPlane::AxialPlaneVisualization View;


    /** \brief  Instantiates.
      */
    MPR( Record::Server& server, ComponentWindowFactory& factory );


private slots:

    /** \brief  Shows the dynamic view.
      */
    void showDynamicView();

    /** \brief  Indicates that the dynamic view is closed.
      */
    void makeDynamicViewShowable();


private:

    /** \brief  Builds the context menu.
      */
    virtual void buildMenu( MenuBuilder& );


    /** \brief  Invokes ThreePanelView::showDynamicView when triggered.
      */
    QAction* const showDynamicViewAction;

    Carna::AxialPlane::AxialPlaneControllerUI* const controller;

    const std::unique_ptr< MPREmbeddablePlacer > embeddablePlacer;


    /** \brief  References the window which holds the dynamic view.
      */
    ComponentEmbeddable* dynamicViewWindow;

    std::vector< View* > mainViews;


    unsigned int initializedRenderersCount;

}; // MPR
