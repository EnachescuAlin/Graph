#ifndef GRAPH
#define GRAPH

#include <list>
#include <exception>
#include <string>

namespace GRAPH
{

class WithoutCost final {};

class GraphException final : public std::exception
{
    public:
        GraphException(const std::string& exc)
        {
            this->exc = exc;
        }

        const char* what() const throw()
        {
            return exc.c_str();
        }

    private:
        std::string exc;
};

template
<
    typename VertexType,
    typename CostType
>
class Edge
{
    public:
        const VertexType& operator[](std::size_t i) const
        {
            if (i == 0)
                return __vertex1;
            if (i == 1)
                return __vertex2;

            throw out_of_range();
        }

        VertexType& operator[](std::size_t i)
        {
            if (i == 0)
                return __vertex1;
            if (i == 1)
                return __vertex2;

            throw out_of_range();
        }

        const CostType& GetCost(void) const
        {
            return __cost;
        }

        CostType& GetCost(void)
        {
            return __cost;
        }

    private:
        VertexType __vertex1, __vertex2;
        CostType __cost;
};

template
<
    typename VertexType
>
class Edge<VertexType, WithoutCost>
{
    public:
        const VertexType& operator[](std::size_t i) const
        {
            if (i == 0)
                return __vertex1;
            if (i == 1)
                return __vertex2;

            throw out_of_range();
        }

        VertexType& operator[](std::size_t i)
        {
            if (i == 0)
                return __vertex1;
            if (i == 1)
                return __vertex2;

            throw out_of_range();
        }

    private:
        VertexType __vertex1, __vertex2;
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
    static_assert(std::is_same<RepresentationType, EdgesList>::value,
                  "RepresentationType must be EdgesList");

    public:
        Graph() = default;
        ~Graph() = default;

    private:
        RepresentationType<VertexType, CostType> collection;
};

}

#endif

