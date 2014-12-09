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

#include "glew.h"
#include "Pointer3D.h"
#include "CarnaContextClient.h"
#include <Carna/Transformation.h>
#include <Carna/Position.h>
#include <Carna/Object3DEvent.h>



// ----------------------------------------------------------------------------------
// Pointer3D
// ----------------------------------------------------------------------------------

Carna::Tools::RotatingColor Pointer3D::nextHeadColor( Carna::Tools::RotatingColor::violet );


Pointer3D::Pointer3D( Record::Server& server )
    : Carna::RotatableObject3D( CarnaContextClient( server ).model(), "Pointer" )
    , length( 290. /* millimeters */ )
    , width( 4. )
    , headColor( nextHeadColor )
    , shaftColor( 255, 0, 0, 0 )
    , shaftDirection( 0, 0, -1 )
    , inverseDirection( false )
{
    ++nextHeadColor;
}


Pointer3D::~Pointer3D()
{
}


void Pointer3D::draw( const QColor& headColor, const QColor& shaftColor ) const
{
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    using Carna::Tools::Transformation;
    using Carna::Tools::Vector;
    using Carna::Position;

    const Vector& t = this->position().toVolumeUnits();
    const Transformation m = Transformation( this->rotation() ).translate( t.x(), t.y(), t.z() );

    const Vector p0 = m * Position::fromMillimeters( this->model, 0, 0, 0 ).toVolumeUnits();
    const Vector p1 = m * Position::fromMillimeters( this->model, shaftDirection * this->length * ( inverseDirection ? -1 : 1 ) ).toVolumeUnits();

    glLineWidth( width );

    glBegin( GL_LINES );

    glColor4f( headColor.redF(), headColor.greenF(), headColor.blueF(), headColor.alphaF() );
    glVertex3f( p0.x(), p0.y(), p0.z() );

    glColor4f( shaftColor.redF(), shaftColor.greenF(), shaftColor.blueF(), shaftColor.alphaF() );
    glVertex3f( p1.x(), p1.y(), p1.z() );

    glEnd();

    glPopAttrib();

}

    
void Pointer3D::paint( const Carna::Renderer& ) const
{
    draw( this->headColor, this->shaftColor );
}
    

void Pointer3D::paintFalseColor( const Carna::Renderer&, const Carna::Tools::Vector3ui& color ) const
{
    const QColor qcolor( color.x, color.y, color.z );
    draw( qcolor, qcolor );
}


double Pointer3D::getLength() const
{
    return length;
}


void Pointer3D::setLength( double length )
{
    if( std::abs( this->length - length ) > 1e-6 )
    {
        this->length = length;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit lengthChanged( this->length );
    }
}


double Pointer3D::getWidth() const
{
    return width;
}


void Pointer3D::setWidth( double width )
{
    if( std::abs( this->width - width ) > 1e-4 )
    {
        this->width = width;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit widthChanged( this->width );
    }
}


const QColor& Pointer3D::getHeadColor() const
{
    return headColor;
}


void Pointer3D::setHeadColor( const QColor& headColor )
{
    if( this->headColor != headColor )
    {
        this->headColor = headColor;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit headColorChanged( this->headColor );
    }
}


const QColor& Pointer3D::getShaftColor() const
{
    return shaftColor;
}


void Pointer3D::setShaftColor( const QColor& shaftColor )
{
    if( this->shaftColor != shaftColor )
    {
        this->shaftColor = shaftColor;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit shaftColorChanged( this->shaftColor );
    }
}


bool Pointer3D::isInverseDirection() const
{
    return inverseDirection;
}


void Pointer3D::setInverseDirection( bool inverseDirection )
{
    if( this->inverseDirection != inverseDirection )
    {
        this->inverseDirection = inverseDirection;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit inverseDirectionChanged( this->inverseDirection );
    }
}


const Carna::Tools::Vector& Pointer3D::getShaftDirection() const
{
    return shaftDirection;
}


void Pointer3D::setShaftDirection( const Carna::Tools::Vector& shaftDirection )
{
    if( std::abs( shaftDirection.norm() - 1. ) > 0.1 )
    {
        return;
    }
    if( ( this->shaftDirection - shaftDirection ).norm() > 1e-6 )
    {
        this->shaftDirection = shaftDirection;

        invalidateObjects3D( Carna::Object3DEvent( Carna::Object3DEvent::shape ) );

        emit shaftDirectionChanged( this->shaftDirection );
    }
}
