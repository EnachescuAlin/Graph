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
class Edge
{
        const VertexType& GetFirstVertex(void) const
        {
            return std::get<0>(edge);
        }

        const VertexType& GetSecondVertex(void) const
        {
            return std::get<1>(edge);
        }

        const CostType& GetCost(void) const
        {
            return std::get<2>(edge);
        }

        VertexType& GetFirstVertex(void)
        {
            return std::get<0>(edge);
        }

        VertexType& GetSecondVertex(void)
        {
            return std::get<1>(edge);
        }

        CostType& GetCost(void)
        {
            return std::get<2>(edge);
        }

    private:
        std::tuple<VertexType, VertexType, CostType> edge;
};

template
<
    typename VertexType
>
class Edge<VertexType, WithoutCost>
{
    public:
        const VertexType& GetFirstVertex(void) const
        {
            return edge.first;
        }

        const VertexType& GetSecondVertex(void) const
        {
            return edge.second;
        }

        VertexType& GetFirstVertex(void)
        {
            return edge.first;
        }

        VertexType& GetSecondVertex(void)
        {
            return edge.second;
        }

    private:
        std::pair<VertexType, VertexType> edge;
};

template
<
    typename VertexType,
    typename CostType
>
class EdgesList
{
    private:
        std::list<Edge<VertexType, CostType>> edgesList;
}

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

