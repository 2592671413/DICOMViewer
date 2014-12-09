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



// ----------------------------------------------------------------------------------
// LeafFinder
// ----------------------------------------------------------------------------------

template< typename Tree >
class LeafFinder
{

public:

    typedef typename Tree::Node Node;
    typedef typename Tree::NodeSet NodeSet;
    typedef std::vector< Node > Path;
    typedef std::set< Path* > PathSet;

 // ----------------------------------------------------------------------------------

    LeafFinder( const Tree& );

    const Tree& tree;

 // ----------------------------------------------------------------------------------

    void compute();

    const PathSet& leafPaths();

 // ----------------------------------------------------------------------------------

private:

    void takeLeaf( Path* );

    std::unique_ptr< PathSet > data;

}; // LeafFinder



template< typename Tree >
LeafFinder< Tree >::LeafFinder( const Tree& tree )
    : tree( tree )
{
}


template< typename Tree >
const typename LeafFinder< Tree >::PathSet& LeafFinder< Tree >::leafPaths()
{
    if( data.get() == nullptr )
    {
        compute();
    }

    return *data;
}


template< typename Tree >
void LeafFinder< Tree >::compute()
{
    data.reset( new PathSet() );

    Path* const rootPath = new Path();
    rootPath->push_back( tree.root );

    std::stack< Path* > paths;
    paths.push( rootPath );

    while( !paths.empty() )
    {
        Path* const path = paths.top();
        paths.pop();

        NodeSet children;
        tree.fetchChildren( children, path->back() );

        if( children.empty() )
        {
            takeLeaf( path );
        }
        else
        {
            for( auto child_itr = ++children.begin(); child_itr != children.end(); ++child_itr )
            {
                Path* const branch = new Path( *path );
                CARNA_ASSERT( branch->size() == path->size() );
                branch->push_back( *child_itr );
                paths.push( branch );
            }

            path->push_back( *children.begin() );
            paths.push( path );
        }
    }
}


template< typename Tree >
void LeafFinder< Tree >::takeLeaf( Path* path )
{
    data->insert( path );
}
