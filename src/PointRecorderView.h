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

#include <Carna/base/Transformation.h>
#include <QGLWidget>

class PointCloud;



// ----------------------------------------------------------------------------------
// PointRecorderView
// ----------------------------------------------------------------------------------

class PointRecorderView : public QGLWidget
{

    Q_OBJECT

public:

    PointRecorderView( const PointCloud& cloud, QWidget* parent = 0 );

    virtual ~PointRecorderView();


public slots:

    void setPointerBearing( const Carna::base::Transformation& );

    void setPointerVisibility( bool );


protected:

    void initializeGL();

    void paintGL();

    void resizeGL( int w, int h );


private:

    const PointCloud& cloud;

    Carna::base::Transformation pointerBearing;

    bool pointerVisibility;

}; // PointRecorderView
