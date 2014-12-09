#version 120

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

void main()
{
    vec4 pos = ftransform();

    float z_rel = ( ( pos.z / pos.w ) - gl_DepthRange.near ) / gl_DepthRange.diff;
    vec3 color = vec3( gl_Color ) * ( 1.0 - z_rel );

    gl_Position = pos;
    gl_FrontColor = vec4( color, 1.0 );
}
