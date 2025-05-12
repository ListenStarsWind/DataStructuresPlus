#include <climits>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// V就是顶点的数据类型
// W则是weight, 权重
// Directed 表示是否是有向的

namespace matrix {

// 默认无向图
template <class V, class W, W MAX_W = INT_MAX, bool Directed = false>
class Graph
{
   public:
    // 对于图的构造, 常见的有三种模式
    // 有些OJ题喜欢直接用标准输入构造
    // 有些喜欢写配置文件, 读文件构造
    // 我们这里一方面为了省事
    // 一方面为了便于修改, 选择
    // 第三种方式, 最开始只加顶点, 边后面加
    Graph(const V* data, int n)
    {
        _data.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            _index_map[data[i]] = i;
            _data.emplace_back(data[i]);
        }

        _matrix.resize(n, std::vector<W>(n, MAX_W));
    }

    int GetVertexIndex(const V& z)
    {
        auto it = _index_map.find(z);

        if (it == _index_map.end())
            throw std::out_of_range("unordered_map::at: key not found");

        return _index_map[z];
    }

    void ModEdge(const V& x, const V& y, W n)
    {
        int src = GetVertexIndex(x);
        int dst = GetVertexIndex(y);

        _matrix[src][dst] = n;

        // 如果是有向图
        if (Directed == false)
            _matrix[dst][src] = n;
    }

    void Print() const
    {
        // Step 1: 计算最大数字宽度
        int max_width = 1;
        for (const auto& row : _matrix)
        {
            for (W w : row)
            {
                if (w != MAX_W)
                {
                    std::ostringstream oss;
                    oss << w;
                    max_width = std::max(max_width, static_cast<int>(oss.str().length()));
                }
            }
        }

        // 非边字符 "#" 也要考虑宽度
        max_width = std::max(max_width, 1);  // 至少为1

        // Step 2: 打印编号对应关系
        std::cout << "Vertex Index:\n";
        for (size_t i = 0; i < _data.size(); ++i) std::cout << "[" << i << "] " << _data[i] << "  ";
        std::cout << "\n\n";

        // Step 3: 打印列标题
        std::cout << std::string(max_width + 1, ' ');
        for (const auto& v : _data) std::cout << std::setw(max_width + 1) << v;
        std::cout << "\n";

        // Step 4: 打印矩阵
        for (size_t i = 0; i < _matrix.size(); ++i)
        {
            std::cout << std::setw(max_width) << _data[i] << " ";
            for (size_t j = 0; j < _matrix[i].size(); ++j)
            {
                if (_matrix[i][j] == MAX_W)
                    std::cout << std::setw(max_width + 1) << "#";
                else
                    std::cout << std::setw(max_width + 1) << _matrix[i][j];
            }
            std::cout << "\n";
        }
    }

   private:
    std::vector<V> _data;
    std::unordered_map<V, int> _index_map;
    std::vector<std::vector<W>> _matrix;
};

void TestGraph()
{
    matrix::Graph<char, int, INT_MAX, true> g("0123", 4);
    g.ModEdge('0', '1', 1);
    g.ModEdge('0', '3', 4);
    g.ModEdge('1', '3', 2);
    g.ModEdge('1', '2', 9);
    g.ModEdge('2', '3', 8);
    g.ModEdge('2', '1', 5);
    g.ModEdge('2', '0', 3);
    g.ModEdge('3', '2', 6);
    g.Print();
}

}  // namespace matrix

namespace table {

template <class W>
struct Edge
{
    typedef Edge<W> self;

    int _dst;
    W _weight;
    std::shared_ptr<self> _next;

    Edge(int dst, const W& weight, const std::shared_ptr<self>& next)
        : _dst(dst), _weight(weight), _next(next)
    {
    }
};

template <class V, class W, bool Directed = false>
class Graph
{
   public:
    typedef Edge<W> Edge_Type;

    Graph(const V* data, int n)
    {
        _data.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            _index_map[data[i]] = i;
            _data.emplace_back(data[i]);
        }

        _table.resize(n);
    }

    int GetVertexIndex(const V& z)
    {
        auto it = _index_map.find(z);

        if (it == _index_map.end())
            throw std::out_of_range("unordered_map::at: key not found");

        return _index_map[z];
    }

    void ModEdge(const V& x, const V& y, const W& w)
    {
        int src = GetVertexIndex(x);
        int dst = GetVertexIndex(y);

        auto node = std::make_shared<Edge_Type>(dst, w, _table[src]);
        _table[src] = node;

        if (Directed == false)
        {
            auto node = std::make_shared<Edge_Type>(src, w, _table[dst]);
            _table[dst] = node;
        }
    }

    void Print()
    {
        for (size_t i = 0; i < _data.size(); ++i)
        {
            std::cout << _data[i] << " -> ";
            auto curr = _table[i];
            while (curr)
            {
                std::cout << "(" << _data[curr->_dst] << ", " << curr->_weight << ") ";
                curr = curr->_next;
            }
            std::cout << '\n';
        }
    }

   private:
    std::vector<V> _data;
    std::unordered_map<V, int> _index_map;
    std::vector<std::shared_ptr<Edge_Type>> _table;
};

void TestGraph()
{
    std::vector<std::string> a{"张三", "李四", "王五", "赵六"};
    Graph<std::string, int> g(a.data(), a.size());
    g.ModEdge("张三", "李四", 100);
    g.ModEdge("张三", "王五", 200);
    g.ModEdge("王五", "赵六", 30);
    g.Print();
}

}  // namespace table