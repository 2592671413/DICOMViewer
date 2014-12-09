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

#include "ViewWindow.h"
#include "ComponentEmbeddable.h"
#include "EmbedAreaArray.h"
#include "EmbedManager.h"
#include "RegistredComponent.h"
#include "ComponentLauncher.h"
#include "ComponentsClient.h"
#include "CarnaContextClient.h"
#include <Carna/Point3D.h>
#include <sstream>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QMenu>



// ----------------------------------------------------------------------------------
// ViewWindowComponentWindowFactory
// ----------------------------------------------------------------------------------

/** \class  ViewWindowComponentWindowFactory
  * \brief  \ref ComponentWindowFactory implementation used by \ref ViewWindow
  *
  * Provides the current EmbedArea layout to the EmbedablePlacer passed to \c createEmbedable.
  * Windows created by this factory are only visible in the view tab this factory belongs to.
  *
  * \see    \ref ViewWindow
  * \see    \ref GlobalComponentWindowFactory
  * \author Leonid Kostrykin
  * \date   2010-2011
  */
class ViewWindowComponentWindowFactory : public ComponentWindowFactory
{

public:

    ViewWindowComponentWindowFactory( ViewWindow& win, EmbedAreaArray& areaArray )
        : win( win )
        , areaArray( areaArray )
    {
    }

    virtual ComponentEmbeddable* createEmbedable( QWidget* child, EmbeddablePlacer& placer, const QString& suffix, const QString& suffixConnector, MenuFactory& menuFactory ) override
    {
        prepare();

        ComponentEmbeddable* const embedable = new ComponentEmbeddable( child, suffix, suffixConnector, menuFactory );
        placer.place( *embedable, embedAreas );
        return embedable;
    }

    virtual ComponentDockable* createDockable( QWidget* child, Qt::DockWidgetArea area, QDockWidget::DockWidgetFeatures features ) override
    {
        prepare();

        ComponentDockable* const dockable = new ComponentDockable();
        dockable->setAllowedAreas( Qt::AllDockWidgetAreas );
        dockable->setFeatures( features );
        dockable->setWidget( child );
        win.addDockWidget( area, dockable );
        return dockable;
    }


private:

    ViewWindow& win;
    EmbedAreaArray& areaArray;
    EmbeddablePlacer::GriddedEmbedAreaSet embedAreas;


    void prepare()
    {
        embedAreas.clear();
        for( EmbedManager::EmbedAreaSet::const_iterator
            it  = EmbedManager::instance().embedAreas().begin();
            it != EmbedManager::instance().embedAreas().end();
        ++it )
        {
            GriddedEmbedArea* pGriddedEmbedArea;
            if( ( pGriddedEmbedArea = dynamic_cast<GriddedEmbedArea*>( *it ) ) )
            {
                if( &( pGriddedEmbedArea->parentArray ) == &areaArray )
                {
                    embedAreas.insert( pGriddedEmbedArea );
                }
            }
        }
    }

}; // ViewWindowComponentWindowFactory



// ----------------------------------------------------------------------------------
// ViewWindow
// ----------------------------------------------------------------------------------

ViewWindow::ViewWindow( Record::Server& server, QWidget* parent, Qt::WFlags flags )
    : QMainWindow( parent, flags )
    , server( server )
    , pShow1x1( new QAction( QIcon( ":/icons/1x1.png" ), "1x1", this ) )
    , pShow2x1( new QAction( QIcon( ":/icons/2x1.png" ), "2x1", this ) )
    , pShow1x2( new QAction( QIcon( ":/icons/1x2.png" ), "1x2", this ) )
    , pShow2x2( new QAction( QIcon( ":/icons/2x2.png" ), "2x2", this ) )
{
    QToolBar* toolbar = addToolBar( "View" );
    toolbar->setMovable( false );

    toolbar->addAction( pShow1x1 );
    toolbar->addAction( pShow2x1 );
    toolbar->addAction( pShow1x2 );
    toolbar->addAction( pShow2x2 );

    connect( pShow1x1, SIGNAL( triggered() ), this, SLOT( show1x1() ) );
    connect( pShow2x1, SIGNAL( triggered() ), this, SLOT( show2x1() ) );
    connect( pShow1x2, SIGNAL( triggered() ), this, SLOT( show1x2() ) );
    connect( pShow2x2, SIGNAL( triggered() ), this, SLOT( show2x2() ) );

    pShow1x1->setCheckable( true );
    pShow2x1->setCheckable( true );
    pShow1x2->setCheckable( true );
    pShow2x2->setCheckable( true );

    // -----------------------------------------------------------------

    QToolButton* pAcquireButton = new QToolButton( toolbar );
    QMenu* pAcquireMenu = new QMenu( pAcquireButton );
    pAcquireButton->setMenu( pAcquireMenu );
    pAcquireButton->setPopupMode( QToolButton::InstantPopup );
    pAcquireButton->setText( "&Acquire" );
    pAcquireButton->setMinimumHeight( 32 );

    toolbar->setIconSize( QSize( 24, 24 ) );
    toolbar->addSeparator();
    toolbar->addWidget( pAcquireButton );

    for( auto it  = ComponentRegister::instance().factories.begin();
              it != ComponentRegister::instance().factories.end();
            ++it )
    {
        ComponentLauncher* launcher = new ComponentLauncher( **it, *this, this );
        pAcquireMenu->addAction( launcher->action );
    }

    // -----------------------------------------------------------------

    areaArray = new EmbedAreaArray();
    this->setCentralWidget( areaArray );

    // -----------------------------------------------------------------

    componentWindowfactory.reset( new ViewWindowComponentWindowFactory( *this, *areaArray ) );

    pShow1x1->trigger();
}


ViewWindow::~ViewWindow()
{
    while( !myComponents.empty() )
    {
        Component& component = **myComponents.begin();
        component.close();
    }
}


void ViewWindow::show1x1()
{
    areaArray->setDimensions( 1, 1 );

    pShow1x1->setChecked( true );
    pShow1x2->setChecked( false );
    pShow2x1->setChecked( false );
    pShow2x2->setChecked( false );
}


void ViewWindow::show1x2()
{
    areaArray->setDimensions( 1, 2 );

    pShow1x1->setChecked( false );
    pShow1x2->setChecked( true );
    pShow2x1->setChecked( false );
    pShow2x2->setChecked( false );
}


void ViewWindow::show2x1()
{
    areaArray->setDimensions( 2, 1 );

    pShow1x1->setChecked( false );
    pShow1x2->setChecked( false );
    pShow2x1->setChecked( true );
    pShow2x2->setChecked( false );
}


void ViewWindow::show2x2()
{
    areaArray->setDimensions( 2, 2 );

    pShow1x1->setChecked( false );
    pShow1x2->setChecked( false );
    pShow2x1->setChecked( false );
    pShow2x2->setChecked( true );
}


void ViewWindow::launch( ComponentLauncher& launcher )
{
    Component* component = launcher.factory.createComponent( server, *componentWindowfactory );

    ComponentsClient( server ).takeComponent( component );

    myComponents.insert( component );

    connect( component, SIGNAL( destroyed( QObject* ) ), this, SLOT( removeReleasedComponent( QObject* ) ) );
}


void ViewWindow::removeReleasedComponent( QObject* object )
{
    Component* const component = static_cast< Component* >( object );
    myComponents.erase( component );
}
