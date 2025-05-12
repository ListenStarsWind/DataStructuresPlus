#pragma once

#include <unistd.h>

#include <unordered_map>
#include <vector>

template <class T>
class UnionFindSet
{
   public:
    // 用户的原始数据不是数字, 该如何将其映射到某个数字上, 并且支持反向映射
    UnionFindSet(const T* data, int n)
    {
        _data.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            _data.emplace_back(data[i]);
            _index_map[_data[i]] = i;
        }
    }

   private:
    std::vector<T> _data;                      // 用编号找原始数据
    std::unordered_map<T, size_t> _index_map;  // 用原始数据找编号
};

// 简单写一下, 不带映射关系了
// 就以模版完全特化的形式来写
template <>
class UnionFindSet<int>
{
   public:
    UnionFindSet(size_t n) : _base(n, -1) {}

    // 合并不同的集合
    void Union(int x, int y)
    {
        if(IsInSameSet(x, y)) return;

        int root1 = FindRoot(x);
        int root2 = FindRoot(y);

        // 让小集合并入大集合(因为 _base[root] 存的是负的，越小表示集合越大)
        if(_base[root1] > _base[root2])
            std::swap(root1, root2);

        _base[root1] += _base[root2];
        _base[root2] = root1;
    }

    // 找根节点
    // 用递归的形式让路径上的每个节点都直接与根相连
    int FindRoot(int z)
    {
        if(_base[z] < 0) return z;

        return _base[z] = FindRoot(_base[z]);
    }

    // // 没有路径压缩
    // int FindRoot(int z)
    // {
    //     int parent = z;
    //     while(_base[parent] >= 0)
    //     {
    //         parent = _base[parent];
    //     }

    //     return parent;
    // }

    // 判断两个元素是否在一个集合
    bool IsInSameSet(int x, int y)
    {
        return FindRoot(x) == FindRoot(y);
    }

    // 获得并查集中的集合数量
    size_t SetSize()
    {
        int ans = 0;
        for(auto e : _base)
        {
            if(e < 0) ++ans;
        }

        return ans;
    }

   private:
    std::vector<int> _base;
};