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
#include <Carna/ModelFactory.h>
#include <QDialog>

class PointCloud;
class QPushButton;



// ----------------------------------------------------------------------------------
// MaskingDialog
// ----------------------------------------------------------------------------------

/** \brief	Lets the user select a mask.
  *
  * \see    \ref MainWindow
  * \author Leonid Kostrykin
  * \date   21.2.13
  */
class MaskingDialog : public QDialog
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    MaskingDialog( Record::Server& server, QWidget* parent = nullptr, Qt::WindowFlags f = 0 );


    bool hasMask() const
    {
        return mask.get();
    }

    const Carna::ModelFactory::BinarizedScalarField& getMask() const
    {
        return *mask;
    }


    Record::Server& server;


private slots:

    void createMask( PointCloud& );

    void releaseMask();


private:

    std::unique_ptr< Carna::ModelFactory::BinarizedScalarField > mask;

    QPushButton* acceptButton;
    
}; // ExportDialog
