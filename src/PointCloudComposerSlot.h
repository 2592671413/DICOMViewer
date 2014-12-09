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
#include <QWidget>
#include <Carna/Object3DChooser.h>

class QLabel;



// ----------------------------------------------------------------------------------
// PointCloudComposerSlot
// ----------------------------------------------------------------------------------

/** \brief	Widget which represents a point within a \ref PointCloudComposer.
  *
  * The main UI-element of this widget is a \c Carna::Object3DChooser:
  *
  * \image  html    PointCloudComposerSlot.png
  *
  * \author Leonid Kostrykin
  * \date   30.3.12 - 31.5.12
  */
class PointCloudComposerSlot : public QWidget
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    PointCloudComposerSlot( Record::Server&, int number );


    /** \brief	Accesses the visual \c Carna::Object3D choosing component.
      */
    const Carna::Object3DChooser& getObjectChooser() const
    {
        return *objectChooser;
    }


    virtual QSize sizeHint() const;


public slots:

    void setNumber( int );


private:
    
    /** \brief	References the visual \c Carna::Object3D choosing component.
      */
    Carna::Object3DChooser* const objectChooser;

    /** \brief	Tells the number of this slot.
      */
    QLabel* const laNumber;


signals:

    /** \brief	This slot should be removed from the containing \ref PointCloudComposer.
      */
    void releaseRequested( PointCloudComposerSlot* );


private slots:

    /** \brief	Emits the \ref releaseRequested signal.
      */
    void remove();
    
}; // PointCloudComposerSlot
