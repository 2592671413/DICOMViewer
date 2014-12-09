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

#include "PointRecorderView.h"
#include "PointCloud.h"
#include <QDebug>

using Carna::Tools::glMultMatrix;



// ----------------------------------------------------------------------------------
// PointRecorderView
// ----------------------------------------------------------------------------------

PointRecorderView::PointRecorderView( const PointCloud& cloud, QWidget* parent )
    : QGLWidget( parent )
    , cloud( cloud )
    , pointerVisibility( false )
{
}


PointRecorderView::~PointRecorderView()
{
}


void PointRecorderView::initializeGL()
{
    glClearColor( 1, 1, 1, 0 );

    const float zNear = 10.f;
    const float zFar  = 2000.f;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -500, 500, -500, 500, zNear, zFar );
    glMatrixMode( GL_MODELVIEW );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


void PointRecorderView::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();
    glTranslatef( 0, 0, -1000.f );
    glRotatef( -45, 1, 0, 0 );
    glRotatef( 90 + 45, 0, 1, 0 );

    // paint reference base

    const float alpha = 0.1f;
    const float axisLength = 100; // in millimeters

    glEnable( GL_BLEND );

    glLineWidth( 0.5f );
    glBegin( GL_LINES );

        glColor4f( 1, 0, 0, alpha );
        glVertex3f( -1000, 0, 0 );
        glVertex3f( +1000, 0, 0 );

        glColor4f( 0, 1, 0, alpha );
        glVertex3f( 0, -1000, 0 );
        glVertex3f( 0, +1000, 0 );

        glColor4f( 0, 0, 1, alpha );
        glVertex3f( 0, 0, -1000 );
        glVertex3f( 0, 0, +1000 );

    glEnd();

    glLineWidth( 3.5f );
    glBegin( GL_LINES );

        glColor4f( 1, 0, 0, alpha );
        glVertex3f( 0, 0, 0 );
        glVertex3f( axisLength, 0, 0 );

        glColor4f( 0, 1, 0, alpha );
        glVertex3f( 0, 0, 0 );
        glVertex3f( 0, axisLength, 0 );

        glColor4f( 0, 0, 1, alpha );
        glVertex3f( 0, 0, 0 );
        glVertex3f( 0, 0, axisLength );

    glEnd();

    glDisable( GL_BLEND );

    glPointSize( 5.f );
    glBegin( GL_POINTS );

        glColor4f( 1, 1, 1, 1 );
        glVertex3f( 0, 0, 0 );

    glEnd();

    if( this->pointerVisibility )
    {
        const Carna::Tools::Vector pos( pointerBearing.a14()
                                      , pointerBearing.a24()
                                      , pointerBearing.a34() );

        glLineWidth( 0.5f );
        glBegin( GL_LINES );

            glColor4f( 1, 0, 0, 1 );
            glVertex3f( pos.x(), 0, 0 );
            glVertex3f( pos.x(), 0, pos.z() );

            glColor4f( 0, 0, 1, 1 );
            glVertex3f( 0, 0, pos.z() );
            glVertex3f( pos.x(), 0, pos.z() );

            glColor4f( 0, 1, 0, 1 );
            glVertex3f( pos.x(), 0, pos.z() );
            glVertex3f( pos.x(), pos.y(), pos.z() );

        glEnd();
    }

    // paint point cloud

    const float brightness = 0.6f;

    glPointSize( 2.5f );
    glColor4f( brightness, brightness, brightness, 1 );
    glBegin( GL_POINTS );

    for( PointCloud::PointList::const_iterator it = cloud.getList().begin(); it != cloud.getList().end(); ++it )
        {
            const Carna::Tools::Vector& point = *it;

            glVertex3f( point.x(), point.y(), point.z() );
        }

    glEnd();

    // paint pointer

    if( this->pointerVisibility )
    {

        const float pointerLength = 800; // in millimeters

        glMultMatrix( this->pointerBearing );
        glLineWidth( 2.f );
        glColor4f( 1, 1, 0, 1 );
        glBegin( GL_LINES );

            glVertex3f( 0, 0, 0 );
            glVertex3f( pointerLength, 0, 0 );

        glEnd();

    }
}


void PointRecorderView::resizeGL( int width, int height )
{
    this->makeCurrent();

    const float mod_w = 1.f;
    const float mod_h = 1.f;

    const unsigned int w = unsigned int( width  * mod_w + 0.5f );
    const unsigned int h = unsigned int( height * mod_h + 0.5f );
    const unsigned int short_side = std::min( w, h );

    glViewport( ( w - short_side ) / 2
              , ( h - short_side ) / 2
              ,   short_side
              ,   short_side );
}


void PointRecorderView::setPointerBearing( const Carna::Tools::Transformation& pointerBearing )
{
    this->pointerBearing = pointerBearing;
}


void PointRecorderView::setPointerVisibility( bool pointerVisibility )
{
    this->pointerVisibility = pointerVisibility;
}
