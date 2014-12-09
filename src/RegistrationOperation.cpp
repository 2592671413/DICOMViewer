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

#include "RegistrationOperation.h"
#include <QMessageBox>
#include <QApplication>



// ----------------------------------------------------------------------------------
// RegistrationOperation
// ----------------------------------------------------------------------------------

RegistrationOperation::RegistrationOperation( const std::string& name, const std::string& predicate, Record::Server& server )
    : name( name )
    , predicate( predicate )
    , server( server )
{
}


RegistrationOperation::~RegistrationOperation()
{
}


bool RegistrationOperation::performSafely( const RegistrationArguments& args
                                         , RegistrationController& controller
                                         , QWidget* dialogParent )
{
    const QString msgBoxTitle = QString::fromStdString( name );

    QApplication::setOverrideCursor( Qt::WaitCursor );

    try
    {
        perform( args, controller, dialogParent );

        QApplication::restoreOverrideCursor();
        
        return true;
    }
    catch( const std::exception& ex )
    {
        QMessageBox::critical( dialogParent, msgBoxTitle, QString::fromStdString( ex.what() ) );
    }
    catch( ... )
    {
        QMessageBox::critical( dialogParent, msgBoxTitle, "Unknown error" );
    }

    QApplication::restoreOverrideCursor();

    return false;
}
