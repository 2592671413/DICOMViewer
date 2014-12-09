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
#include <Carna/base/model/RotatableObject3D.h>
#include <Carna/base/qt/RotatingColor.h>
#include <QColor>



// ----------------------------------------------------------------------------------
// Pointer3D
// ----------------------------------------------------------------------------------

/** \brief	Pointer scene element
  *
  * \author Leonid Kostrykin
  * \date   9.8.12
  */
class Pointer3D : public Carna::base::model::RotatableObject3D
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    Pointer3D( Record::Server& server );

    /** \brief	Releases acquired resources.
      */
    virtual ~Pointer3D();

    
    /** \brief  Draws this object.
      */
    virtual void paint( const Carna::base::view::Renderer& ) const override;
    
    /** \brief  Draw this object using all-over the specified color.
      */
    virtual void paintFalseColor( const Carna::base::view::Renderer&, const Carna::base::Vector3ui& color ) const override;


    /** \brief  Tells the length of this pointer in millimeters.
      */
    double getLength() const;

    /** \brief  Tells the width of this pointer.
      */
    double getWidth() const;

    const QColor& getHeadColor() const;

    const QColor& getShaftColor() const;

    const Carna::base::Vector& getShaftDirection() const;

    bool isInverseDirection() const;


public slots:
    
    /** \brief  Sets the length of this pointer in millimeters.
      */
    void setLength( double );

    /** \brief  Sets the width of this pointer.
      */
    void setWidth( double );

    void setHeadColor( const QColor& );

    void setShaftColor( const QColor& );

    void setShaftDirection( const Carna::base::Vector& );

    void setInverseDirection( bool );


signals:

    void lengthChanged( double );

    void widthChanged( double );

    void headColorChanged( const QColor& );

    void shaftColorChanged( const QColor& );

    void shaftDirectionChanged( const Carna::base::Vector& );

    void inverseDirectionChanged( bool );


private:
    
    /** \brief  Holds the length of this pointer in millimeters.
      */
    double length;

    double width;

    QColor headColor;

    QColor shaftColor;

    Carna::base::Vector shaftDirection;

    bool inverseDirection;


    static Carna::base::qt::RotatingColor nextHeadColor;


    void draw( const QColor& headColor, const QColor& shaftColor ) const;

}; // Pointer3D
