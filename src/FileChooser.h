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

#include <QFileDialog>

class QCloseEvent;



// ----------------------------------------------------------------------------------
// FileChooser
// ----------------------------------------------------------------------------------

class FileChooser : public QFileDialog
{

    Q_OBJECT

public:

    FileChooser();


public slots:

    virtual void accept() override;

    virtual void reject() override;


protected:

    virtual void closeEvent( QCloseEvent* ) override;
    
}; // FileChooser
