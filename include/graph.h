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

    inline static void AddEdge(std::list<EdgesListType>& _list,
                               const VertexType& v1,
                               const VertexType& v2,
                               const CostType& ct)
    {
        _list.push_back(EdgesListType(v1, v2, ct));
    }

    inline static VertexType& GetFirstVertex(EdgesListType& __tuple)
    {
        return std::get<0>(__tuple);
    }

    inline static const VertexType& GetFirstVertex(const EdgesListType& __tuple)
    {
        return std::get<0>(__tuple);
    }

    inline static VertexType& GetSecondVertex(EdgesListType& __tuple)
    {
        return std::get<1>(__tuple);
    }

    inline static const VertexType& GetSecondVertex(const EdgesListType& __tuple)
    {
        return std::get<1>(__tuple);
    }

    inline static CostType& GetCost(EdgesListType& __tuple)
    {
        return std::get<2>(__tuple);
    }

    inline static const CostType& GetCost(const EdgesListType& __tuple)
    {
        return std::get<2>(__tuple);
    }
};

template<typename VertexType>
struct CostTraits<VertexType, NoCostClass> final
{
    typedef std::vector<bool>                 AdjacencyMatrixType;
    typedef std::pair<VertexType, VertexType> EdgesListType;

    inline static void AddEdge(std::list<EdgesListType>& _list,
                               const VertexType& v1,
                               const VertexType& v2,
                               const NoCostClass& unused)
    {
        _list.push_back(EdgesListType(v1, v2));
    }

    inline static VertexType& GetFirstVertex(EdgesListType& __pair)
    {
        return __pair.first;
    }

    inline static const VertexType& GetFirstVertex(const EdgesListType& __pair)
    {
        return __pair.first;
    }

    inline static VertexType& GetSecondVertex(EdgesListType& __pair)
    {
        return __pair.second;
    }

    inline static const VertexType& GetSecondVertex(const EdgesListType& __pair)
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
        typedef CostTraits
        <
            VertexType,
            CostType
        > __CostTraits;

        typedef std::list
        <
            typename CostTraits
            <
                VertexType,
                CostType
            >::EdgesListType
        > __EdgesListType;

        typedef std::map
        <
            VertexType,
            std::list
            <
                VertexType
            >
        > __AdjacencyListType;

        typedef std::vector
        <
            typename CostTraits
            <
                VertexType,
                CostType
            >::AdjacencyMatrixType
        > __AdjacencyMatrixType;

        typedef std::is_same
        <
            CostType,
            NoCostClass
        > __NoCost;





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

        /*
         * Delete edges list.
         */
        inline void DeleteEdgesList(void)
        {
            edgesList = nullptr;
        }

        /*
         * Set edges list.
         */
        inline void SetEdgesList(const __EdgesListType& __list)
        {
            edgesList.reset(new __EdgesListType(__list));
        }

        /*
         * Delete adjacency list.
         */
        inline void DeleteAdjacencyList(void)
        {
            adjacencyList = nullptr;
        }

        /*
         * Set adjacency list.
         */
        inline void SetAdjacencyList(const __AdjacencyListType& __map)
        {
            adjacencyList.reset(new __AdjacencyListType(__map));
        }

        /*
         * Delete adjacency matrix.
         */
        inline void DeleteAdjacencyMatrix(void)
        {
            adjacencyMatrix = nullptr;
        }

        /*
         * Set adjacency matrix.
         */
        inline void SetAdjacencyMatrix(const __AdjacencyMatrixType& __matrix)
        {
            adjacencyMatrix.reset(new __AdjacencyMatrixType(__matrix));
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

                inline bool operator!=(const EdgesListIterator& __it) const
                {
                    return (this->__iterator != __it.__iterator);
                }

                inline EdgesListIterator& operator++()
                {
                    ++__iterator;
                    return *this;
                }

                inline EdgesListIterator& operator--()
                {
                    ++__iterator;
                    return *this;
                }

                inline const VertexType& GetFirstVertex(void) const
                {
                    return __CostTraits::GetFirstVertex(*__iterator);
                }

                inline const VertexType& GetSecondVertex(void) const
                {
                    return __CostTraits::GetSecondVertex(*__iterator);
                }

                inline const CostType& GetCost(void) const
                {
                    return __CostTraits::GetCost(*__iterator);
                }

                inline VertexType& GetFirstVertex(void)
                {
                    return __CostTraits::GetFirstVertex(*__iterator);
                }

                inline VertexType& GetSecondVertex(void)
                {
                    return __CostTraits::GetSecondVertex(*__iterator);
                }

                inline CostType& GetCost(void)
                {
                    return __CostTraits::GetCost(*__iterator);
                }

                inline const EdgesListIterator& operator*(void) const
                {
                    return *this;
                }

                inline EdgesListIterator& operator*(void)
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

                inline bool operator!=(const EdgesListConstIterator& __it)
                {
                    return (this->__iterator != __it.__iterator);
                }

                inline EdgesListConstIterator& operator++()
                {
                    ++__iterator;
                    return *this;
                }

                inline EdgesListConstIterator& operator--()
                {
                    ++__iterator;
                    return *this;
                }

                inline const VertexType& GetFirstVertex(void) const
                {
                    return __CostTraits::GetFirstVertex(*__iterator);
                }

                inline const VertexType& GetSecondVertex(void) const
                {
                    return __CostTraits::GetSecondVertex(*__iterator);
                }

                inline const CostType& GetCost(void) const
                {
                    return __CostTraits::GetCost(*__iterator);
                }

                inline const EdgesListConstIterator& operator*(void) const
                {
                    return *this;
                }

                inline EdgesListConstIterator& operator*(void)
                {
                    return *this;
                }

            protected:
                EdgesListConstIteratorType __iterator;
        };

        inline EdgesListIterator EdgesListBegin(void)
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListIterator(edgesList->begin());
        }

        inline EdgesListIterator EdgesListEnd(void)
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListIterator(edgesList->end());
        }

        inline EdgesListConstIterator EdgesListBegin(void) const
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListConstIterator(edgesList->cbegin());
        }

        inline EdgesListConstIterator EdgesListEnd(void) const
            throw(GraphException)
        {
            if (!edgesList)
                throw GraphException("Edges list is null");

            return EdgesListConstIterator(edgesList->cend());
        }

        inline EdgesListConstIterator begin(void) const
            throw(GraphException)
        {
            return EdgesListBegin();
        }

        inline EdgesListConstIterator end(void) const
            throw(GraphException)
        {
            return EdgesListEnd();
        }

        inline EdgesListIterator begin(void)
            throw(GraphException)
        {
            return EdgesListBegin();
        }

        inline EdgesListIterator end(void)
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
        std::shared_ptr<__AdjacencyListType>    adjacencyList;

        /*
         * Shared pointer for adjacency matrix.
         */
        std::shared_ptr<__AdjacencyMatrixType>  adjacencyMatrix;

        /*
         * Shared pointer for edges list.
         */
        std::shared_ptr<__EdgesListType>        edgesList;


        /*
         * Read edges list from file.
         */
        void ReadEdgesListFromFile(std::ifstream& file)
        {
            using namespace std;

            edgesList.reset(new __EdgesListType);

            VertexType v1, v2;
            CostType cost;
            size_t edgesNumber;

            file >> edgesNumber;

            if (__NoCost::value)
                for (size_t i = 0; i < edgesNumber; i++)
                    if (file >> v1 >> v2)
                        __CostTraits::AddEdge(*edgesList, v1, v2, cost);
                    else
                        throw GraphException("Wrong input");
            else
                for (size_t i = 0; i < edgesNumber; i++)
                    if (file >> v1 >> v2 >> cost)
                        __CostTraits::AddEdge(*edgesList, v1, v2, cost);
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

