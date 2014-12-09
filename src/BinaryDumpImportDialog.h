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
// ImportDialog
// ----------------------------------------------------------------------------------

/** \brief	Presents data set importing options to the user.
  *
  * Lets the user select:
  * - The \ref ExportDialog::IntegerFormat "voxel format"
  * - The dataset dimensions (width, height, depth)
  * - The dataset spacings
  *
  * \see    \ref MainWindow
  * \author Leonid Kostrykin
  * \date   26.7.12
  */
class ImportDialog : public QDialog
{

    Q_OBJECT

public:

    /** \brief	Instantiates.
      */
    ImportDialog( QWidget* parent = nullptr, Qt::WindowFlags f = 0 );


    /** \brief	Accesses the currently selected voxel format.
      */
    IntegerFormatChooser& voxelFormat;

    /** \brief	Tells the currently set dataset width.
      */
    unsigned int getDataSetWidth() const
    {
        return width;
    }

    /** \brief	Tells the currently set dataset height.
      */
    unsigned int getDataSetHeight() const
    {
        return height;
    }

    /** \brief	Tells the currently set dataset depth.
      */
    unsigned int getDataSetDepth() const
    {
        return depth;
    }

    /** \brief	Tells the currently set width-spacing.
      */
    double getSpacingX() const
    {
        return spacingX;
    }

    /** \brief	Tells the currently set height-spacing.
      */
    double getSpacingY() const
    {
        return spacingY;
    }

    /** \brief	Tells the currently set depth-spacing.
      */
    double getSpacingZ() const
    {
        return spacingZ;
    }

    /** \brief	Tells whether the selected file has a meta data header.
      */
    bool isMetaDataHeaderEnabled() const
    {
        return metaDataFromFile;
    }


public slots:

    /** \brief	Accepts the dialog if all inputs are valid.
      */
    virtual void accept() override;


private:

    /** \brief	Holds the currently set dataset width.
      */
    unsigned int width;

    /** \brief	Holds the currently set dataset height.
      */
    unsigned int height;

    /** \brief	Holds the currently set dataset depth.
      */
    unsigned int depth;

    /** \brief	Holds the currently set width-spacing.
      */
    double spacingX;

    /** \brief	Holds the currently set height-spacing.
      */
    double spacingY;

    /** \brief	Holds the currently set depth-spacing.
      */
    double spacingZ;

    /** \brief	Holds whether the selected binary dump has a meta data header.
      */
    bool metaDataFromFile;


private slots:

    void setDataSetWidth( const QString& );

    void setDataSetHeight( const QString& );

    void setDataSetDepth( const QString& );

    void setSpacingX( const QString& );

    void setSpacingY( const QString& );

    void setSpacingZ( const QString& );

    void setMetaDataFromFile( bool );
    
}; // ExportDialog
