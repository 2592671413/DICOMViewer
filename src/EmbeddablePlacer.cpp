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

#include "EmbeddablePlacer.h"
#include "EmbedAreaArray.h"



// ----------------------------------------------------------------------------------
// SingleEmbedablePlacer
// ----------------------------------------------------------------------------------

void SingleEmbeddablePlacer::place( Embeddable& em, const GriddedEmbedAreaSet& set )
{
    if( set.size() == 1 && !( **( set.begin() ) ).hasClient() )
    {
        ( **( set.begin() ) ).setClient( em );
    }
}
