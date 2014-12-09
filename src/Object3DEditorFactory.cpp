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

#include "Object3DEditorFactory.h"
#include "Object3DEditor.h"
#include "Point3DEditor.h"
#include "PointCloud3DEditor.h"
#include "PointCloud3D.h"
#include "Pointer3DEditor.h"
#include "Pointer3D.h"
#include <Carna/Point3D.h>



// ----------------------------------------------------------------------------------
// Object3DEditorFactory
// ----------------------------------------------------------------------------------

Object3DEditorFactory::Object3DEditorFactory( EditorType requiredEditorType )
    : createdEditorType( requiredEditorType )
{
}


Object3DEditor* Object3DEditorFactory::create( Carna::Object3D& object ) const
{
    switch( createdEditorType )
    {

        case genericEditor:
        {
            return new Object3DEditor( object );
        }

        case pointEditor:
        {
            return new Point3DEditor( static_cast< Carna::Point3D& >( object ) );
        }

        case pointCloudEditor:
        {
            return new PointCloud3DEditor( static_cast< PointCloud3D& >( object ) );
        }

        case pointerEditor:
        {
            return new Pointer3DEditor( static_cast< Pointer3D& >( object ) );
        }

        default:
        {
            throw std::logic_error( "Unsupported editor type." );
        }

    }
}
