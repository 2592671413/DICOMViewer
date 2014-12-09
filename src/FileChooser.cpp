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

#include "FileChooser.h"
#include <QCloseEvent>



// ----------------------------------------------------------------------------------
// FileChooser
// ----------------------------------------------------------------------------------

FileChooser::FileChooser()
{
}


void FileChooser::accept()
{
    emit done( QDialog::Accepted );
    this->setVisible( true );
}


void FileChooser::reject()
{
    emit done( QDialog::Rejected );
}


void FileChooser::closeEvent( QCloseEvent* ev )
{
    ev->ignore();
}
