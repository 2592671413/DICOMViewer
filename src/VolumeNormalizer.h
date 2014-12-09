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

#include <Carna/base/noncopyable.h>
#include <Carna/Carna.h>
#include <QObject>

class QWidget;



// ----------------------------------------------------------------------------------
// VolumeNormalizer
// ----------------------------------------------------------------------------------

/** \brief  Builds a new volume from the current one while leaving out the
  *         surrounding empty space.
  *
  * The empty space is determined by an HUV threshold.
  *
  * \author Leonid Kostrykin
  * \date   10.8.12
  */
class VolumeNormalizer : public QObject
{

    Q_OBJECT

    NON_COPYABLE

public:

    /** \brief	Instantiates.
      */
    VolumeNormalizer( const Carna::base::model::Scene&, QWidget* parent = nullptr );

    ~VolumeNormalizer();


    int getThreshold() const;

    int getMinimalSize() const;

    Carna::base::model::Scene* getResult() const;

    double getSizeLoss() const;


public slots:

    void setThreshold( int );

    void setMinimalSize( int );

    void compute();


signals:

    void thresholdChanged( int );

    void minimalSizeChanged( int );

    void computed();

    void computed( double sizeLoss );


private:

    const Carna::base::model::Scene& model;

    QWidget* const parent;

    signed int threshold;

    unsigned minimalSize;


    unsigned minX,
             maxX,
             minY,
             maxY,
             minZ,
             maxZ;

    double sizeLoss;


    bool discardsSliceX( unsigned ) const;

    bool discardsSliceY( unsigned ) const;

    bool discardsSliceZ( unsigned ) const;
    
}; // VolumeNormalizer
