#ifndef GRAPH_H
#define GRAPH_H
 
#include <vector>
#include <list>
#include <map>
#include <exception>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <memory>
#include <tuple>

namespace GRAPH
{
 
#define MAX_MODE (ADJACENCY_LIST | ADJACENCY_MATRIX | EDGES_LIST)
#define MIN_MODE EDGES_LIST
 
enum RepresentationMode
{
    EDGES_LIST       = 1L << 0,
    ADJACENCY_MATRIX = 1L << 1,
    ADJACENCY_LIST   = 1L << 2
};

inline constexpr RepresentationMode operator|(RepresentationMode __a,
                                              RepresentationMode __b)
{
    return RepresentationMode(static_cast<long>(__a) | static_cast<long>(__b));
}

inline constexpr bool operator&(RepresentationMode __a, RepresentationMode __b)
{
    return ((static_cast<long>(__a) & static_cast<long>(__b)) != 0);
}

inline constexpr bool operator>(RepresentationMode __a, RepresentationMode __b)
{
    return (static_cast<long>(__a) > static_cast<long>(__b));
}

inline constexpr bool operator<(RepresentationMode __a, RepresentationMode __b)
{
    return (static_cast<long>(__a) < static_cast<long>(__b));
}

inline constexpr bool operator==(RepresentationMode __a, RepresentationMode __b)
{
    return (static_cast<long>(__a) == static_cast<long>(__b));
}

inline constexpr bool operator!=(RepresentationMode __a, RepresentationMode __b)
{
    return (static_cast<long>(__a) != static_cast<long>(__b));
}



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





class NoCostClass               final {};
class UndirectedGraphClass      final {};
class DirectedGraphClass        final {};



std::istream& operator>>(std::istream& ifs, const NoCostClass& unused)
{
    return ifs;
}



template<typename VertexType, typename CostType>
struct CostTraits final
{
    typedef std::vector<CostType*>                       AdjacencyMatrixType;
    typedef std::tuple<VertexType, VertexType, CostType> EdgesListType;

    static void AddEdge(std::list<EdgesListType>& _list,
                        const VertexType& v1,
                        const VertexType& v2,
                        const CostType& ct)
    {
        _list.push_back(EdgesListType(v1, v2, ct));
    }

    static VertexType& GetFirstVertex(EdgesListType& __tuple)
    {
        return std::get<0>(__tuple);
    }

    static const VertexType& GetFirstVertex(const EdgesListType& __tuple)
    {
        return std::get<0>(__tuple);
    }

    static VertexType& GetSecondVertex(EdgesListType& __tuple)
    {
        return std::get<1>(__tuple);
    }

    static const VertexType& GetSecondVertex(const EdgesListType& __tuple)
    {
        return std::get<1>(__tuple);
    }

    static CostType& GetCost(EdgesListType& __tuple)
    {
        return std::get<2>(__tuple);
    }

    static const CostType& GetCost(const EdgesListType& __tuple)
    {
        return std::get<2>(__tuple);
    }

    template<class Compare>
    static void SortEdgesList(std::list<EdgesListType>& __list, Compare _compare)
    {
        __list.sort(_compare);
    }
};

template<typename VertexType>
struct CostTraits<VertexType, NoCostClass> final
{
    typedef std::vector<bool>                 AdjacencyMatrixType;
    typedef std::pair<VertexType, VertexType> EdgesListType;

    static void AddEdge(std::list<EdgesListType>& _list,
                        const VertexType& v1,
                        const VertexType& v2,
                        const NoCostClass& unused)
    {
        _list.push_back(EdgesListType(v1, v2));
    }

    static VertexType& GetFirstVertex(EdgesListType& __pair)
    {
        return __pair.first;
    }

    static const VertexType& GetFirstVertex(const EdgesListType& __pair)
    {
        return __pair.first;
    }

    static VertexType& GetSecondVertex(EdgesListType& __pair)
    {
        return __pair.second;
    }

    static const VertexType& GetSecondVertex(const EdgesListType& __pair)
    {
        return __pair.second;
    }
};





template
<
    typename VertexType,
    typename CostType = NoCostClass,
    typename DirectionType = UndirectedGraphClass
>
class Graph final
{
    static_assert(std::is_same<DirectionType, DirectedGraphClass>::value ||
                  std::is_same<DirectionType, UndirectedGraphClass>::value,
                  "Uses DirectedGraphClass or UndirectedGraphClass!");

    public:
        typedef typename CostTraits
        <
            VertexType,
            CostType
        >::EdgesListType EdgeType;

        typedef CostTraits
        <
            VertexType,
            CostType
        > EdgeTraits;

        typedef std::list
        <
            typename CostTraits
            <
                VertexType,
                CostType
            >::EdgesListType
        > EdgesList;

        typedef std::map
        <
            VertexType,
            std::list
            <
                VertexType
            >
        > AdjacencyList;

        typedef std::vector
        <
            typename CostTraits
            <
                VertexType,
                CostType
            >::AdjacencyMatrixType
        > AdjacencyMatrix;





        /*
         * Default destructor
         */
       ~Graph() = default;

        /*
         * Default constructor
         */
        Graph() = default;

        /*
         * Overloading constructor
         */
        Graph(const std::string& filename,
              const RepresentationMode& mode = EDGES_LIST,
              const bool& binaryFile = false)
              throw(GraphException)
        {
            Read(filename, mode, binaryFile);
        }
 
        /*
         * Read the graph from file.
         */
        void Read(const std::string& filename,
                  const RepresentationMode& mode = EDGES_LIST,
                  const bool& binaryFile = false)
                  throw(GraphException)
        {
            using namespace std;

            if (mode > MAX_MODE || mode < MIN_MODE)
                throw GraphException("Wrong representation mode");

            ifstream file;
            if (binaryFile)
                file.open(filename);
            else
                file.open(filename, ios::in | ios::binary);

            if (!file.is_open())
                throw GraphException("Unable to open file");

            if ((mode & edges_list) && !binaryFile)
                ReadEdgesListFromFile(file);

            file.close();
        }

        void TransformFromTo(const RepresentationMode& from,
                             const RepresentationMode& to)
            throw(GraphException)
        {
            if (from == to)
                throw GraphException("");

            if (from != EDGES_LIST &&
                from != ADJACENCY_LIST &&
                from != ADJACENCY_MATRIX)
                throw GraphException("");

            if (to != EDGES_LIST &&
                to != ADJACENCY_LIST &&
                to != ADJACENCY_MATRIX)
                throw GraphException("");

            if (from == EDGES_LIST)
            {
                if (to == ADJACENCY_LIST)
                {
                    return;
                }
                else
                {
                    return;
                }
            }

            if (from == ADJACENCY_LIST)
            {
                if (to == EDGES_LIST)
                {
                    return;
                }
                else
                {
                    return;
                }
            }

            if (from == ADJACENCY_MATRIX)
            {
                if (to == EDGES_LIST)
                {
                    return;
                }
                else
                {
                    return;
                }
            }
        }

        /*
         * Delete edges list.
         */
        void DeleteEdgesList(void)
        {
            edgesList = nullptr;
        }

        /*
         * Set edges list.
         */
        void SetEdgesList(const EdgesList& __list)
        {
            edgesList.reset(new EdgesList(__list));
        }

        /*
         * Delete adjacency list.
         */
        void DeleteAdjacencyList(void)
        {
            adjacencyList = nullptr;
        }

        /*
         * Set adjacency list.
         */
        void SetAdjacencyList(const AdjacencyList& __map)
        {
            adjacencyList.reset(new AdjacencyList(__map));
        }

        /*
         * Delete adjacency matrix.
         */
        void DeleteAdjacencyMatrix(void)
        {
            adjacencyMatrix = nullptr;
        }

        /*
         * Set adjacency matrix.
         */
        void SetAdjacencyMatrix(const AdjacencyMatrix& __matrix)
        {
            adjacencyMatrix.reset(new AdjacencyMatrix(__matrix));
        }

        template<class Compare>
        void SortEdgesList(Compare __compare)
        {
            EdgeTraits::SortEdgesList(*edgesList, __compare);
        }





        /*
         * Iterator for edges list.
         */
        class EdgesListIterator
        {
            public:
                typedef typename std::list
                <
                    typename CostTraits
                    <
                        VertexType,
                        CostType
                    >::EdgesListType
                >::iterator EdgesListIteratorType;

                EdgesListIterator(const EdgesListIteratorType& __it) :
                    __iterator(__it)
                {

                }

                bool operator!=(const EdgesListIterator& __it) const
                {
                    return (this->__iterator != __it.__iterator);
                }

                EdgesListIterator& operator++()
                {
                    ++__iterator;
                    return *this;
                }

                EdgesListIterator& operator--()
                {
                    ++__iterator;
                    return *this;
                }

                const VertexType& GetFirstVertex(void) const
                {
                    return EdgeTraits::GetFirstVertex(*__iterator);
                }

                const VertexType& GetSecondVertex(void) const
                {
                    return EdgeTraits::GetSecondVertex(*__iterator);
                }

                const CostType& GetCost(void) const
                {
                    return EdgeTraits::GetCost(*__iterator);
                }

                VertexType& GetFirstVertex(void)
                {
                    return EdgeTraits::GetFirstVertex(*__iterator);
                }

                VertexType& GetSecondVertex(void)
                {
                    return EdgeTraits::GetSecondVertex(*__iterator);
                }

                CostType& GetCost(void)
                {
                    return EdgeTraits::GetCost(*__iterator);
                }

                const EdgesListIterator& operator*(void) const
                {
                    return *this;
                }

                EdgesListIterator& operator*(void)
                {
                    return *this;
                }

            protected:
                EdgesListIteratorType __iterator;
        };

        /*
         * Const iterator for edges list.
         */
        class EdgesListConstIterator
        {
            public:
                typedef typename std::list
                <
                    typename CostTraits
                    <
                        VertexType,
                        CostType
                    >::EdgesListType
                >::const_iterator EdgesListConstIteratorType;

                EdgesListConstIterator(const EdgesListConstIteratorType& __it) :
                    __iterator(__it)
                {

                }

                bool operator!=(const EdgesListConstIterator& __it)
                {
                    return (this->__iterator != __it.__iterator);
                }

                EdgesListConstIterator& operator++()
                {
                    ++__iterator;
                    return *this;
                }

                EdgesListConstIterator& operator--()
                {
                    ++__iterator;
                    return *this;
                }

                const VertexType& GetFirstVertex(void) const
                {
                    return EdgeTraits::GetFirstVertex(*__iterator);
                }

                const VertexType& GetSecondVertex(void) const
                {
                    return EdgeTraits::GetSecondVertex(*__iterator);
                }

                const CostType& GetCost(void) const
                {
                    return EdgeTraits::GetCost(*__iterator);
                }

                const EdgesListConstIterator& operator*(void) const
                {
                    return *this;
                }

                EdgesListConstIterator& operator*(void)
                {
                    return *this;
                }

            protected:
                EdgesListConstIteratorType __iterator;
        };

        EdgesListIterator EdgesListBegin(void)
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListIterator(edgesList->begin());
        }

        EdgesListIterator EdgesListEnd(void)
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListIterator(edgesList->end());
        }

        EdgesListConstIterator EdgesListBegin(void) const
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListConstIterator(edgesList->cbegin());
        }

        EdgesListConstIterator EdgesListEnd(void) const
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListConstIterator(edgesList->cend());
        }

        EdgesListConstIterator begin(void) const
            throw(GraphException)
        {
            return EdgesListBegin();
        }

        EdgesListConstIterator end(void) const
            throw(GraphException)
        {
            return EdgesListEnd();
        }

        EdgesListIterator begin(void)
            throw(GraphException)
        {
            return EdgesListBegin();
        }

        EdgesListIterator end(void)
            throw(GraphException)
        {
            return EdgesListEnd();
        }

        /*
         * Are used when you call Read().
         */
        static const RepresentationMode adjacency_list   = ADJACENCY_LIST;
        static const RepresentationMode adjacency_matrix = ADJACENCY_MATRIX;
        static const RepresentationMode edges_list       = EDGES_LIST;

    private:
        /*
         * Shared pointer for adjacency list.
         */
        std::shared_ptr<AdjacencyList>    adjacencyList;

        /*
         * Shared pointer for adjacency matrix.
         */
        std::shared_ptr<AdjacencyMatrix>  adjacencyMatrix;

        /*
         * Shared pointer for edges list.
         */
        std::shared_ptr<EdgesList>        edgesList;


        /*
         * Read edges list from file.
         */
        void ReadEdgesListFromFile(std::ifstream& file)
            throw(GraphException)
        {
            using namespace std;

            edgesList.reset(new EdgesList);

            VertexType v1, v2;
            CostType cost;
            size_t edgesNumber;

            file >> edgesNumber;

            if (is_same<CostType, NoCostClass>::value)
                for (size_t i = 0; i < edgesNumber; i++)
                    if (file >> v1 >> v2)
                        EdgeTraits::AddEdge(*edgesList, v1, v2, cost);
                    else
                        throw GraphException("Wrong input");
            else
                for (size_t i = 0; i < edgesNumber; i++)
                    if (file >> v1 >> v2 >> cost)
                        EdgeTraits::AddEdge(*edgesList, v1, v2, cost);
                    else
                        throw GraphException("Wrong input");
        }

}; // end Graph class

/*
 * Used when you call Read()
 * e.g.:
 *      graph.Read("graphfile.txt", Mode::adjacency_matrix)
 *      graph.Read("graphfile.txt", Mode::adjacency_matrix | Mode::edges_list)
 */
typedef Graph
<
    int,
    NoCostClass,
    UndirectedGraphClass
> Mode;

/*
 * Graph::adjacency_list
 */
template
<
    typename VertexType,
    typename CostType,
    typename DirectionType
>
const RepresentationMode Graph
<
    VertexType,
    CostType,
    DirectionType
>::adjacency_list;

/*
 * Graph::adjacency_matrix
 */
template
<
    typename VertexType,
    typename CostType,
    typename DirectionType
>
const RepresentationMode Graph
<
    VertexType,
    CostType,
    DirectionType
>::adjacency_matrix;

/*
 * Graph::edges_list
 */
template
<
    typename VertexType,
    typename CostType,
    typename DirectionType
>
const RepresentationMode Graph
<
    VertexType,
    CostType,
    DirectionType
>::edges_list;
 
} // end GRAPH namespace
 
#endif

