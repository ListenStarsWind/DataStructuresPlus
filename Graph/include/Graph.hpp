#include <algorithm>
#include <climits>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "disjoint_set.hpp"

// V就是顶点的数据类型
// W则是weight, 权重
// Directed 表示是否是有向的

namespace matrix {

// 默认无向图
template <class V, class W, W MAX_W = INT_MAX, bool Directed = false>
class Graph
{
    typedef Graph<V, W, MAX_W, Directed> self;

   public:
    struct Edge
    {
        int _src;
        int _dst;
        W _weight;

        Edge(int src, int dst, const W& weight) : _src(src), _dst(dst), _weight(weight) {}

        bool operator>(const Edge& that) const { return _weight > that._weight; }
    };

    Graph() = default;

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

    void _ModEdge(int src, int dst, const W& w)
    {
        _matrix[src][dst] = w;

        // 如果是有向图
        if (Directed == false)
            _matrix[dst][src] = w;
    }

    void ModEdge(const V& x, const V& y, const W& w)
    {
        int src = GetVertexIndex(x);
        int dst = GetVertexIndex(y);

        _ModEdge(src, dst, w);
    }

    void Print() const
    {
        // Step 1: 计算最大数字宽度
        int max_width = 1;
        for (const auto& row : _matrix)
        {
            for (const W& w : row)
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

        // Step 2: 打印编号与数据对应关系
        std::cout << "Vertex Index:\n";
        for (size_t i = 0; i < _data.size(); ++i) std::cout << "[" << i << "] " << _data[i] << "  ";
        std::cout << "\n\n";

        // Step 3: 打印列标题（编号而不是 _data）
        std::cout << std::string(max_width + 1, ' ');
        for (size_t i = 0; i < _data.size(); ++i) std::cout << std::setw(max_width + 1) << i;
        std::cout << "\n";

        // Step 4: 打印矩阵内容（行首也用编号）
        for (size_t i = 0; i < _matrix.size(); ++i)
        {
            std::cout << std::setw(max_width) << i << " ";
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

    int Kruskal(self& minTree)
    {
        using namespace std;

        self ans(_data.data(), _data.size());

        int size = _data.size();
        priority_queue<Edge, vector<Edge>, greater<Edge>> heap;
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                if (_matrix[i][j] != MAX_W)
                {
                    heap.emplace(i, j, _matrix[i][j]);
                }
            }
        }

        int ret = 0;
        UnionFindSet<int> ufs(size);
        while (!heap.empty())
        {
            Edge top = heap.top();
            heap.pop();

            if (ufs.IsInSameSet(top._src, top._dst))
                continue;

            ++ret;
            ufs.Union(top._src, top._dst);
            ans._ModEdge(top._src, top._dst, top._weight);
        }

        if (ret == size - 1)
            minTree.swap(ans);

        return ret - (size - 1);
    }

    void swap(self& that)
    {
        std::swap(_data, that._data);
        std::swap(_index_map, that._index_map);
        std::swap(_matrix, that._matrix);
    }

    int Prim(self& minTree, const V& start)
    {
        int n = _data.size();
        int src = GetVertexIndex(start);

        std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> heap;

        std::unordered_set<int> S;
        std::unordered_set<int> Q;

        S.emplace(src);
        for (int i = 0; i < n; ++i)
        {
            if (i == src)
                continue;

            Q.emplace(i);

            if (_matrix[src][i] != MAX_W)
            {
                heap.emplace(src, i, _matrix[src][i]);
            }
        }

        int count = 0;
        self ans(_data.data(), n);
        while (!Q.empty() && count < n - 1)
        {
            Edge min = heap.top();
            heap.pop();

            int src = min._src;
            int dst = min._dst;

            if (Q.count(dst) == 0)
                continue;

            ++count;
            ans._ModEdge(src, dst, _matrix[src][dst]);
            Q.erase(dst);
            S.emplace(dst);

            int u = min._dst;

            for (int i = 0; i < n; ++i)
            {
                if (!Q.count(i))
                    continue;
                if (_matrix[u][i] == MAX_W)
                    continue;

                heap.emplace(u, i, _matrix[u][i]);
            }
        }

        if (count == n - 1)
            minTree.swap(ans);

        return count - (n - 1);
    }

    std::vector<V> path(int src, int dst, const std::vector<int> parent)
    {
        std::vector<V> path;
        path.reserve(_data.size());
        path.emplace_back(_data[dst]);

        int idx = dst;
        while (parent[idx] != src)
        {
            idx = parent[idx];
            path.emplace_back(_data[idx]);
        }

        path.emplace_back(_data[src]);

        path.shrink_to_fit();
        std::reverse(path.begin(), path.end());

        return path;
    }

    void _Dijkstra(int src, std::vector<W>& dist, std::vector<int>& parent)
    {
        dist[src] = W();
        parent[src] = src;

        std::vector<bool> S(_data.size(), false);
        std::vector<bool> Q(_data.size(), true);

        for (int i = 0; i < _data.size(); ++i)
        {
            int u = -1;
            W min = MAX_W;
            for (int j = 0; j < Q.size(); ++j)
            {
                if (Q[j] == false)
                    continue;

                if (dist[j] < min)
                {
                    min = dist[j];
                    u = j;
                }
            }

            for (int j = 0; j < _data.size(); ++j)
            {
                if (_matrix[u][j] == MAX_W)
                    continue;
                if (S[j] == true)
                    continue;

                if (dist[u] + _matrix[u][j] < dist[j])
                {
                    parent[j] = u;
                    dist[j] = dist[u] + _matrix[u][j];
                }
            }

            Q[u] = false;
            S[u] = true;
        }
    }

    std::vector<V> Dijkstra(const V& start, const V& end)
    {
        int src = GetVertexIndex(start);
        std::vector<W> dist(_data.size(), MAX_W);   // 记录最短路径
        std::vector<int> parent(_data.size(), -1);  // 寻找上一个前驱顶点

        _Dijkstra(src, dist, parent);

        return path(src, GetVertexIndex(end), parent);
    }

    bool _BellmanFord(int src, std::vector<W>& dist, std::vector<int>& parent)
    {
        dist[src] = W();
        parent[src] = src;

        int n = _index_map.size();

        bool result = true;
        for (int i = 0; i < n; ++i)
        {
            std::cout << "================================" << std::endl;
            std::cout << "第" << i + 1 << "轮松弛尝试" << std::endl;

            bool flag = true;
            for (int j = 0; j < n; ++j)
            {
                for (int k = 0; k < n; ++k)
                {
                    if (_matrix[j][k] == MAX_W)
                        continue;
                    if (dist[j] == MAX_W)
                        continue;

                    std::cout << "对" << j << "到" << k << "尝试更新" << std::endl;
                    if (dist[j] + _matrix[j][k] < dist[k])
                    {
                        flag = false;
                        std::cout << "原值" << dist[k] << " ";
                        dist[k] = dist[j] + _matrix[j][k];
                        std::cout << "现值" << dist[k] << std::endl;
                        parent[k] = j;
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            if (flag)
                break;
            if (i == n - 1)
                result = false;
        }

        return result;
    }

    bool BellmanFord(const V& start, const V& end, std::vector<V>& p)
    {
        int src = GetVertexIndex(start);
        std::vector<W> dist(_data.size(), MAX_W);
        std::vector<int> parent(_data.size(), -1);

        bool ret = _BellmanFord(src, dist, parent);

        // 负权环会让路径生成陷入死循环
        if (ret == true)
            p = path(src, GetVertexIndex(end), parent);

        return ret;
    }

    void _FloydWarshall(std::vector<std::vector<W>>& vvDist,
                        std::vector<std::vector<int>>& vvParent)
    {
        int n = _data.size();

        // 初始化时直接连接
        // 所以parent就是i
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (_matrix[i][j] != MAX_W)
                {
                    vvDist[i][j] = _matrix[i][j];
                    vvParent[i][j] = i;
                }
                else
                {
                    vvDist[i][j] = _matrix[i][j];
                    vvParent[i][j] = -1;
                }

                if (i == j)
                {
                    vvDist[i][j] = W();
                    vvParent[i][j] = -1;
                }
            }
        }

        for (int k = 0; k < n; ++k)
        {
            for (int i = 0; i < n; ++i)
            {
                for (int j = 0; j < n; ++j)
                {
                    if (vvDist[i][k] != MAX_W && vvDist[k][j] != MAX_W)
                    {
                        if (vvDist[i][j] > vvDist[i][k] + vvDist[k][j])
                        {
                            vvDist[i][j] = vvDist[i][k] + vvDist[k][j];

                            // k -> j可能并不直接相连
                            // 所以是[k][j]
                            vvParent[i][j] = vvParent[k][j];
                        }
                    }
                }
            }

            // 打印权值和路径矩阵观察数据
            for (size_t i = 0; i < n; ++i)
            {
                for (size_t j = 0; j < n; ++j)
                {
                    if (vvDist[i][j] == MAX_W)
                    {
                        // cout << "*" << " ";
                        printf("%3c", '*');
                    }
                    else
                    {
                        // cout << vvDist[i][j] << " ";
                        printf("%3d", vvDist[i][j]);
                    }
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
            for (size_t i = 0; i < n; ++i)
            {
                for (size_t j = 0; j < n; ++j)
                {
                    // cout << vvParentPath[i][j] << " ";
                    printf("%3d", vvParent[i][j]);
                }
                std::cout << std::endl;
            }
            std::cout << "=================================" << std::endl;
        }
    }

    std::vector<std::vector<V>> FloydWarshall()
    {
        int n = _data.size();
        std::vector<std::vector<W>> vvDist(n, std::vector<W>(n, MAX_W));
        std::vector<std::vector<int>> vvParent(n, std::vector<int>(n, -1));

        _FloydWarshall(vvDist, vvParent);

        std::vector<std::vector<V>> result;
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (i == j)
                    continue;
                result.emplace_back(path(i, j, vvParent[i]));
            }
        }

        return result;
    }

    void bfs(const V& start)
    {
        int size = _data.size();
        int src = GetVertexIndex(start);
        std::vector<bool> visited(size, false);

        visited[src] = true;
        std::queue<int> q;
        q.push(src);

        while (!q.empty())
        {
            int n = q.size();
            for (int i = 0; i < n; ++i)
            {
                int from = q.front();
                q.pop();

                for (int lead = 0; lead < size; ++lead)
                {
                    if (_matrix[from][lead] == MAX_W)
                        continue;
                    if (visited[lead] == true)
                        continue;

                    q.push(lead);
                    visited[lead] = true;
                }
            }
        }
    }

    void _dfs(int src, std::vector<bool>& visit)
    {
        visit[src] = true;

        for (int i = 0; i < _data.size(); ++i)
        {
            if (visit[i] == true)
                continue;
            if (_matrix[src][i] == MAX_W)
                continue;

            dfs(i, visit);
        }
    }

    void dfs(const V& start)
    {
        std::vector<bool> visit(_data.size(), false);
        _dfs(GetVertexIndex(start), visit);
    }

   private:
    std::vector<V> _data;
    std::unordered_map<V, int> _index_map;
    std::vector<std::vector<W>> _matrix;
};

void TestGraph()
{
    matrix::Graph<char, int, INT_MAX> g("0123", 4);
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

void TestGraphMinTree()
{
    const char* str = "abcdefghi";
    Graph<char, int> g(str, strlen(str));
    g.ModEdge('a', 'b', 4);
    g.ModEdge('a', 'h', 8);
    // g.ModEdge('a', 'h', 9);
    g.ModEdge('b', 'c', 8);
    g.ModEdge('b', 'h', 11);

    g.ModEdge('c', 'i', 2);
    g.ModEdge('c', 'f', 4);
    g.ModEdge('c', 'd', 7);
    g.ModEdge('d', 'f', 14);
    g.ModEdge('d', 'e', 9);
    g.ModEdge('e', 'f', 10);
    g.ModEdge('f', 'g', 2);
    g.ModEdge('g', 'h', 1);
    g.ModEdge('g', 'i', 6);
    g.ModEdge('h', 'i', 7);
    Graph<char, int> kminTree;
    std::cout << "Kruskal:" << g.Kruskal(kminTree) << std::endl;
    kminTree.Print();

    Graph<char, int> PrimTree;
    std::cout << "Prim:" << g.Prim(PrimTree, 'a') << std::endl;
    PrimTree.Print();
}

void TestGraphDijkstra()
{
    const char* str = "syztx";
    Graph<char, int, INT_MAX, true> g(str, strlen(str));
    g.ModEdge('s', 't', 10);
    g.ModEdge('s', 'y', 5);
    g.ModEdge('y', 't', 3);
    g.ModEdge('y', 'x', 9);
    g.ModEdge('y', 'z', 2);
    g.ModEdge('z', 's', 7);
    g.ModEdge('z', 'x', 6);
    g.ModEdge('t', 'y', 2);
    g.ModEdge('t', 'x', 1);
    g.ModEdge('x', 'z', 4);
    auto path = g.Dijkstra('s', 'x');

    for (auto c : path)
    {
        std::cout << "[" << c << "]";
        if (c != path.back())
            std::cout << "->";
    }
    std::cout << std::endl;
}

void TestGraphBellmanFord()
{
    const char* str = "syztx";
    Graph<char, int, INT_MAX, true> g(str, strlen(str));
    g.ModEdge('s', 't', 6);
    g.ModEdge('s', 'y', 7);
    g.ModEdge('y', 'z', 9);
    g.ModEdge('y', 'x', -3);
    g.ModEdge('y', 's', 1);  // 新增
    g.ModEdge('z', 's', 2);
    g.ModEdge('z', 'x', 7);
    g.ModEdge('t', 'x', 5);
    // g.ModEdge('t', 'y', 8);
    g.ModEdge('t', 'y', -8);  // 修改
    g.ModEdge('t', 'z', -4);
    g.ModEdge('x', 't', -2);

    std::vector<char> path;
    auto validity = g.BellmanFord('s', 'x', path);

    std::cout << (validity == true ? "无负权环, 结果可行" : "有负权环, 问题无解") << std::endl;
    for (auto c : path)
    {
        std::cout << "[" << c << "]";
        if (c != path.back())
            std::cout << "->";
    }
    std::cout << std::endl;
}

void TestFloydWarShall()
{
    const char* str = "12345";
    Graph<char, int, INT_MAX, true> g(str, strlen(str));
    g.ModEdge('1', '2', 3);
    g.ModEdge('1', '3', 8);
    g.ModEdge('1', '5', -4);
    g.ModEdge('2', '4', 1);
    g.ModEdge('2', '5', 7);
    g.ModEdge('3', '2', 4);
    g.ModEdge('4', '1', 2);
    g.ModEdge('4', '3', -5);
    g.ModEdge('5', '4', 6);

    auto vv = g.FloydWarshall();
    for (const auto& v : vv)
    {
        for (const auto& e : v)
        {
            std::cout << "[" << e << "]";
            if (e != v.back())
                std::cout << "->";
        }
        std::cout << std::endl;
    }
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