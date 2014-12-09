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
// Dijkstra
// ----------------------------------------------------------------------------------

template< typename GraphSupport >
class Dijkstra
{

public:

    typedef typename GraphSupport::Graph        Graph;
    typedef typename GraphSupport::Node         Node;
    typedef typename GraphSupport::OrderedEdges OrderedEdges;
    typedef typename GraphSupport::NodeHash     NodeHash;

 // ----------------------------------------------------------------------------------

    Dijkstra( const Graph& graph, const Node& root );

    const Node& rootNode() const;

 // ----------------------------------------------------------------------------------

    bool next();

    bool visitSuccessors( const Node&, const std::function< bool( const Node& ) >& ) const;

    unsigned int countEnqueuedNodes() const;

    unsigned int countExpandedNodes() const;

 // ----------------------------------------------------------------------------------

    template< typename Serializer >
    void saveTo( Serializer& ) const;

    template< typename Deserializer >
    void loadFrom( Deserializer& );

 // ----------------------------------------------------------------------------------

private:

    const Graph& graph;

    Node root;

    struct Expansion
    {

        Node node;
        Node predecessor;
        double distance;

        Expansion( const Node& node, const Node& predecessor, double distance );

    };  // Expansion

    struct closer
    {
        bool operator()( const Expansion& n2, const Expansion& n1 ) const;
    };

    std::priority_queue< Expansion, std::vector< Expansion >, closer > expansionQueue;
    std::map< Node, double, typename GraphSupport::less > queuedExpansionDistances;
    std::set< NodeHash > expandedNodes;
    std::multimap< Node, Node, typename GraphSupport::less > successors;

    Expansion pickNextNode();

    struct ExpansionQueueUnderflow : public std::exception
    {
        virtual const char* what() const throw() override
        {
            return "Remaining queued expansions are obsolete.";
        }
    };

}; // Dijkstra



template< typename GraphSupport >
bool Dijkstra< GraphSupport >::closer::operator()
    ( const Expansion& n2
    , const Expansion& n1 ) const
{
    return n1.distance < n2.distance;
}


template< typename GraphSupport >
Dijkstra< GraphSupport >::Dijkstra
    ( const Graph& graph
    , const Node& root )

    : root( root )
    , graph( graph )
{
    expansionQueue.push( Expansion( root, root, 0. ) );
}


template< typename GraphSupport >
Dijkstra< GraphSupport >::Expansion::Expansion
    ( const Node& node
    , const Node& predecessor
    , double distance )

    : node( node )
    , predecessor( predecessor )
    , distance( distance )
{
}


template< typename GraphSupport >
typename Dijkstra< GraphSupport >::Expansion Dijkstra< GraphSupport >::pickNextNode()
{
    Expansion nodeData( Node(), Node(), 0 );
    NodeHash nodeHash;
    unsigned int evaluatedExpansions = 0;
    do 
    {
        if( expansionQueue.empty() )
        {
            throw ExpansionQueueUnderflow(); // remaining queued expansions are obsolete
        }
        nodeData = expansionQueue.top();
        nodeHash = GraphSupport::hash( graph, nodeData.node );
        expansionQueue.pop();
        ++evaluatedExpansions;
    }
    while( expandedNodes.find( nodeHash ) != expandedNodes.end() );
    expandedNodes.insert( nodeHash );

    if( evaluatedExpansions > 1 )
    {
        qDebug( "%d obsoleted expansions.", evaluatedExpansions - 1 );
    }

    return nodeData;
}


template< typename GraphSupport >
bool Dijkstra< GraphSupport >::next()
{
    if( expansionQueue.empty() )
    {
        return false;
    }

 // pick the next node to expand

    try
    {
        const Expansion current = pickNextNode();

     // enlist current node as successor to its predecessor node if current node is not root

        if( current.predecessor != current.node )
        {
            successors.insert( std::make_pair< Node, Node >( current.predecessor, current.node ) );
        }

     // enqueue succeeding nodes

        OrderedEdges edges;
        graph.expand( current.node, edges,
            [&]( const Node& node )->bool
            {
                typedef GraphSupport ConcreteGraphSupport;
                return expandedNodes.find( ConcreteGraphSupport::hash( graph, node ) ) == expandedNodes.end();
            }
        );
        for( auto edge_it = edges.begin(); edge_it != edges.end(); ++edge_it )
        {
            const double distance = edge_it->first;
            const Node& successor = edge_it->second;
            const double totalDistance = current.distance + distance;

            expansionQueue.push( Expansion( successor, current.node, totalDistance ) );
        }

     // tell that a node has been expanded
    
        return true;
    }
    catch( const ExpansionQueueUnderflow& )
    {
        return false; // remaining queued expansions are obsolete
    }
}


template< typename GraphSupport >
bool Dijkstra< GraphSupport >::visitSuccessors
    ( const Node& node
    , const std::function< bool( const Node& ) >& visit ) const
{
    const auto range = successors.equal_range( node );

    if( range.second == range.first )
    {
        return false;
    }

    for( auto node_it = range.first; node_it != range.second; ++node_it )
    {
        if( !visit( node_it->second ) )
        {
            break;
        }
    }

    return true;
}


template< typename GraphSupport >
unsigned int Dijkstra< GraphSupport >::countEnqueuedNodes() const
{
    return expansionQueue.size();
}


template< typename GraphSupport >
unsigned int Dijkstra< GraphSupport >::countExpandedNodes() const
{
    return expandedNodes.size();
}


template< typename GraphSupport >
const typename GraphSupport::Node& Dijkstra< GraphSupport >::rootNode() const
{
    return root;
}


template< typename GraphSupport >
template< typename Serializer >
void Dijkstra< GraphSupport >::saveTo( Serializer& serializer ) const
{
    serializer.serializeNode( root );

    serializer.createList( expandedNodes.size() );
    for( auto nodeHash_itr = expandedNodes.begin(); nodeHash_itr != expandedNodes.end(); ++nodeHash_itr )
    {
        serializer.serializeNodeHash( *nodeHash_itr );
    }

    serializer.createList( successors.size() );
    for( auto edge_itr = successors.begin(); edge_itr != successors.end(); ++edge_itr )
    {
        serializer.serializeNode( edge_itr->first );
        serializer.serializeNode( edge_itr->second );
    }

    typedef decltype( this->expansionQueue ) PriorityQueue;
    auto expansionQueue = PriorityQueue( this->expansionQueue );
    std::vector< Expansion > expansions;
    while( !expansionQueue.empty() )
    {
        expansions.push_back( expansionQueue.top() );
        expansionQueue.pop();
    }

    serializer.createList( expansions.size() );
    for( auto expansion_itr = expansions.begin(); expansion_itr != expansions.end(); ++expansion_itr )
    {
        serializer.serializeNode( expansion_itr->node );
        serializer.serializeNode( expansion_itr->predecessor );
        serializer.serializeDistance( expansion_itr->distance );
    }
}


template< typename GraphSupport >
template< typename Deserializer >
void Dijkstra< GraphSupport >::loadFrom( Deserializer& deserializer )
{
    deserializer.deserializeNode( root );

    expandedNodes.clear();
    const unsigned int expandedNodesCount = deserializer.readListLength();
    for( unsigned int i = 0; i < expandedNodesCount; ++i )
    {
        expandedNodes.insert( deserializer.deserializeNodeHash() );
    }

    successors.clear();
    const unsigned int successorsCount = deserializer.readListLength();
    for( unsigned int i = 0; i < successorsCount; ++i )
    {
        Node from, to;
        deserializer.deserializeNode( from );
        deserializer.deserializeNode( to );
        successors.insert( std::make_pair< Node, Node >( from, to ) );
    }

    while( !expansionQueue.empty() )
    {
        expansionQueue.pop();
    }
    const unsigned int expansionsCount = deserializer.readListLength();
    for( unsigned int i = 0; i < expansionsCount; ++i )
    {
        Node node, predecessor;
        deserializer.deserializeNode( node );
        deserializer.deserializeNode( predecessor );
        const double distance  = deserializer.deserializeDistance();
        expansionQueue.push( Expansion( node, predecessor, distance ) );
    }
}
