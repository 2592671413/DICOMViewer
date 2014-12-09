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

#include "Server.h"
#include "Segmentation.h"
#include "PointCloud.h"

class QProgressDialog;



// ----------------------------------------------------------------------------------
// SurfaceExtraction
// ----------------------------------------------------------------------------------

class SurfaceExtraction
{

    NON_COPYABLE

public:

    SurfaceExtraction( QProgressDialog& progress, Record::Server& server, const Segmentation::MaskType& );


    const PointCloud& getPointCloud() const
    {
        return *cloud;
    }


private:

    PointCloud* cloud;

}; // SurfaceExtraction
