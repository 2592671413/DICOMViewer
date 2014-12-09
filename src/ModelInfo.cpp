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

#include "ModelInfo.h"
#include "CarnaContextClient.h"
#include <Carna/base/model/Scene.h>
#include <Carna/base/model/Volume.h>
#include <QLabel>
#include <QFormLayout>



// ----------------------------------------------------------------------------------
// ModelInfoWidget
// ----------------------------------------------------------------------------------

ModelInfoWidget::ModelInfoWidget( Record::Server& server )
    : QWidget()
    , server( server )
{
    QFormLayout* const layout = new QFormLayout();

    layout->setLabelAlignment( Qt::AlignRight );

    Carna::base::model::Scene& model = CarnaContextClient( server ).model();

    layout->addRow( "Width:" , new QLabel( QString::number( model.volume().size.x ) ) );
    layout->addRow( "Height:", new QLabel( QString::number( model.volume().size.y ) ) );
    layout->addRow( "Depth:" , new QLabel( QString::number( model.volume().size.z ) ) );

    layout->addRow( "Spacing X:", new QLabel( QString::number( model.spacingX() ) ) );
    layout->addRow( "Spacing Y:", new QLabel( QString::number( model.spacingY() ) ) );
    layout->addRow( "Spacing Z:", new QLabel( QString::number( model.spacingZ() ) ) );

    this->setLayout( layout );
}


ModelInfoWidget::~ModelInfoWidget()
{
}



// ----------------------------------------------------------------------------------
// ModelInfo
// ----------------------------------------------------------------------------------

ModelInfo::ModelInfo( Record::Server& server, ComponentWindowFactory& factory )
    : Component( server, factory, "Model Info" )
{
    ModelInfoWidget* view = new ModelInfoWidget( server );

    ComponentDockable& dockable = createDockable( view
        , Qt::LeftDockWidgetArea
        , ComponentWindowFactory::defaultDockableFeatures
        | QDockWidget::DockWidgetFloatable
        | QDockWidget::DockWidgetClosable );
}


ModelInfo::~ModelInfo()
{
}
