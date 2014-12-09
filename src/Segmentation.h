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

#include <Carna/Carna.h>
#include <Carna/base/noncopyable.h>
#include "ScalarField3ui.h"

class PointCloud;



// ----------------------------------------------------------------------------------
// Segmentation
// ----------------------------------------------------------------------------------

class Segmentation
{

    NON_COPYABLE

public:

    typedef ScalarField3ui< unsigned short > MaskType;


    Segmentation( const Carna::base::model::Scene&
                , const Carna::base::model::Object3D& seedPoint
                , int huv0
                , int huv1 );


    const MaskType& getMask() const
    {
        return mask;
    }


private:

    MaskType mask;

}; // Segmentation
