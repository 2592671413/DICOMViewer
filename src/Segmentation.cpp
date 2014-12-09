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

#include "Segmentation.h"
#include "BinaryVolumeMask.h"
#include <Carna/base/model/Scene.h>
#include <Carna/base/model/Volume.h>
#include <Carna/base/model/Object3D.h>
#include <Carna/base/CarnaException.h>
#include <TRTK/RegionGrowing3D.hpp>
#include <QApplication>
#include <QDebug>



// ----------------------------------------------------------------------------------
// Segmentation
// ----------------------------------------------------------------------------------

Segmentation::Segmentation( const Carna::base::model::Scene& model
                          , const Carna::base::model::Object3D& seedPointObject
                          , int huv0
                          , int huv1 )
    : mask( Carna::base::Vector3ui( model.volume().size ) )
{
    BinaryVolumeMask binaryVolumeMask( huv0, huv1, model.volume() );

    TRTK::RegionGrowing3D< bool
                         , MaskType::VoxelType
                         , BinaryVolumeMask& > regionGrowing3D
            ( binaryVolumeMask
            , model.volume().size.x
            , model.volume().size.y
            , model.volume().size.z );

    regionGrowing3D.setNeighborhoodSize( 2 );

    QApplication::setOverrideCursor( Qt::WaitCursor );

    TRTK::Coordinate< unsigned > seedPoint( 0, 0, 0 );
    seedPoint.x() = unsigned( seedPointObject.position().toVolumeUnits().x() * ( model.volume().size.x - 1 ) + 0.5 );
    seedPoint.y() = unsigned( seedPointObject.position().toVolumeUnits().y() * ( model.volume().size.y - 1 ) + 0.5 );
    seedPoint.z() = unsigned( seedPointObject.position().toVolumeUnits().z() * ( model.volume().size.z - 1 ) + 0.5 );

    regionGrowing3D.compute( seedPoint );

    if( !regionGrowing3D.getRegions().empty() )
    {
        const unsigned int maskedCount = regionGrowing3D.getRegions()[ 0 ].size();
        const unsigned int voxelsCount = model.volume().size.x * model.volume().size.y * model.volume().size.z;
        const unsigned int maskedRatio = unsigned( ( maskedCount / static_cast< double >( voxelsCount ) ) * 100 + 0.5 );

        qDebug() << "Segmentation: " << maskedRatio << "% have been masked.";
    }


    // fetch mask

    std::copy( regionGrowing3D.getLabelMask()
             , regionGrowing3D.getLabelMask() + ( mask.getData().size() - 1 )
             , mask.getData().begin() );

    QApplication::restoreOverrideCursor();
}
