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

#include <Carna/Vector3.h>
#include <vector>



// ----------------------------------------------------------------------------------
// ScalarField3ui
// ----------------------------------------------------------------------------------

/** \brief  Provides \f$\mathbb{Z}_0^3 \to K\f$ where \f$K\f$ is \a VoxelType.
  *
  * \author Leonid Kostrykin
  * \date   2012
  */
template< typename VoxelType >
class ScalarField3ui
{

    NON_COPYABLE

public:

    /** \brief	Holds the voxel type.
      */
    typedef VoxelType VoxelType;


    /** \brief	Instantiates.
      */
    ScalarField3ui( const Carna::Tools::Vector3ui& size )
        : data( size.x * size.y * size.z )
        , size( size )
    {
    }


    /** \brief	References some voxel.
      */
    VoxelType& operator()( unsigned int x, unsigned int y, unsigned int z )
    {
        return data[ getIndex( x, y, z ) ];
    }
    
    /** \brief	References some voxel.
      */
    const VoxelType& operator()( unsigned int x, unsigned int y, unsigned int z ) const
    {
        return data[ getIndex( x, y, z ) ];
    }

    /** \brief	References the underlying voxel buffer.
      *
      * \invariant
      * <code>getData()[ z * getSize().x * getSize().y + y * getSize().x + x ] == operator()( x, y, z )</code>
      */
    std::vector< VoxelType >& getData()
    {
        return data;
    }
    
    /** \brief	References the underlying voxel buffer.
      *
      * \invariant
      * <code>getData()[ z * getSize().x * getSize().y + y * getSize().x + x ] == operator()( x, y, z )</code>
      */
    const std::vector< VoxelType >& getData() const
    {
        return data;
    }

    /** \brief	Tells this volume's size.
      */
    const Carna::Tools::Vector3ui& getSize() const
    {
        return size;
    }


private:

    /** \brief	Holds the voxel data.
      *
      * \invariant
      * <code>data[ z * size.x * size.y + y * size.x + x ] == operator()( x, y, z )</code>
      */
    std::vector< VoxelType > data;

    /** \brief	Holds this volume's size.
      */
    const Carna::Tools::Vector3ui size;


    /** \brief	Computes the position of a voxel within the \ref data "voxel buffer".
      */
    unsigned int getIndex( unsigned int x, unsigned int y, unsigned int z )
    {
        return z * size.x * size.y + y * size.x + x;
    }

}; // ScalarField3ui
