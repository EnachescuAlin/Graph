#ifndef GRAPH
#define GRAPH

#include <tuple>
#include <list>

namespace GRAPH
{

class WithoutCost final {};

template
<
    typename VertexType,
    typename CostType
>
class EdgesList
{
    private:
        std::list<std::tuple<VertexType, VertexType, CostType>> edgesList;
};

template
<
    typename VertexType
>
class EdgesList<VertexType, WithoutCost>
{
    private:
        std::list<std::pair<VertexType, VertexType>> edgesList;
};

template
<
    typename VertexType,
    typename CostType,
    typename RepresentationType,
    typename GraphType
>
class Graph
{
    public:
        Graph() = default;
        ~Graph() = default;
};

}

#endif

