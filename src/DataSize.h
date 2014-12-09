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
// DataSize
// ----------------------------------------------------------------------------------

/** \brief	Represents data size.
  *
  * \author Leonid Kostrykin
  * \date   26.7.12
  */
class DataSize
{

public:

    DataSize( unsigned long size_in_bytes )
        : size_in_bytes( size_in_bytes )
    {
    }


    operator QString() const
    {
        unsigned int sizeUnit;

        const static unsigned int      BYTES = 0;
        const static unsigned int KILO_BYTES = 1;
        const static unsigned int MEGA_BYTES = 2;

        unsigned long size = size_in_bytes;
        for( sizeUnit = BYTES; size > 1024 && sizeUnit <= MEGA_BYTES; ++sizeUnit )
        {
            size = static_cast< unsigned long >( std::ceil( size / 1024. ) );
        }

        QString sizeUnitStr;
        switch( sizeUnit )
        {

        case BYTES:
            sizeUnitStr = "bytes";
            break;

        case KILO_BYTES:
            sizeUnitStr = "KB";
            break;

        case MEGA_BYTES:
            sizeUnitStr = "MB";
            break;

        default:
            throw std::logic_error( "Size computation failed." );

        }

        return QString::number( size ) + " " + sizeUnitStr;
    }

    operator std::string() const
    {
        QString qstr = ( *this );
        return qstr.toStdString();
    }


private:

    unsigned long size_in_bytes;

}; // DataSize
