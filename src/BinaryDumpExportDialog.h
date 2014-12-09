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

#include "IntegerFormatChooser.h"
#include <QDialog>



// ----------------------------------------------------------------------------------
// ExportDialog
// ----------------------------------------------------------------------------------

/** \brief	Presents data set exporting options to the user.
  *
  * Lets the user select a \ref IntegerFormat "voxel format".
  *
  * \see    \ref MainWindow
  * \author Leonid Kostrykin
  * \date   26.7.12
  */
class ExportDialog : public QDialog
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    ExportDialog( QWidget* parent = nullptr, Qt::WindowFlags f = 0 );


    /** \brief	Accesses the currently selected voxel format.
      */
    IntegerFormatChooser& voxelFormat;

    /** \brief	Tells whether a meta data header shall be written.
      */
    bool isMetaDataExportEnabled() const
    {
        return metaDataExport;
    }


private slots:

    void setMetaDataExport( bool );


private:

    bool metaDataExport;
    
}; // ExportDialog
