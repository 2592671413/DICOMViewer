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

#include <QDialog>



// ----------------------------------------------------------------------------------
// IntegerFormatChooser
// ----------------------------------------------------------------------------------

/** \brief	Presents data set exporting options to the user.
  *
  * Lets the user select a \ref IntegerFormat "voxel format".
  *
  * \see    \ref MainWindow
  * \author Leonid Kostrykin
  * \date   26.7.12
  */
class IntegerFormatChooser : public QWidget
{

    Q_OBJECT

public:

    /** \brief	Lists integer formats.
      */
    enum IntegerFormat
    {
        native16bit = 0,    ///< \brief Represents 16bit integer with system native byte order.
        native32bit = 1     ///< \brief Represents 32bit integer with system native byte order.
    };


    /** \brief	Instantiates.
      */
    IntegerFormatChooser( QWidget* parent = nullptr );


    /** \brief	Tells the currently selected voxel format.
      */
    IntegerFormat value() const
    {
        return format;
    }


private:

    /** \brief	Holds the currently selected voxel format.
      */
    IntegerFormat format;


private slots:

    /** \brief	Sets the currently selected voxel format.
      *
      * This method does not update the UI.
      */
    void setCurrentFormat( int index );
    
}; // IntegerFormatChooser
