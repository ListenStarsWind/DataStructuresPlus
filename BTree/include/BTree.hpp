#pragma once
#include <stddef.h>     // size_t
#include <algorithm>    // std::fill
#include <utility>      // std::pair
#include <iterator>     // std::begin(arr), std::end(arr)


template <class K, size_t M>
struct BTreeNode
{
    private:
    typedef BTreeNode<K, M> self;

    public:
    size_t _n;          // 记录关键字的个数

    K _keys[M];         // 关键字数组
    self* _subs[M + 1]; // 子节点指针数组

    self* _parent;

    BTreeNode(){
        // 初始化, 数据清空
        _n = 0;
        _parent = nullptr;
        std::fill(std::begin(_keys), std::end(_keys), K());
        std::fill(std::begin(_subs), std::end(_subs), nullptr);
    }

    // 二分查找, 如果target在关键字数组中, 返回索引
    // 否则, 返回-1
    std::pair<bool, size_t> search(const K& target)
    {
        if(_n == 0) return {false, 0};

        ssize_t left = 0, right = _n - 1;
        while(left <= right)
        {
            ssize_t mid = left + (right - left) / 2;
            if(_keys[mid] < target)
                left = mid + 1;
            else if(_keys[mid] > target)
                right = mid - 1;
            else
                return {true, mid};
        }

        return {false, left};
    }
};

template <class K, size_t M>
class BTree
{
    typedef BTreeNode<K, M> Node;

    // 向一个节点插入一个关键字
    void _insert(const K& key, Node* node, Node* sub = nullptr)
    {
        auto [temp, idx] = node->search(key);

        if(idx >= node->_n)
        {
            // 说明直接尾插即可
        }
        else
        {
            // 说明需要挪动数据
            for(size_t i = node->_n; i > idx; --i)
            {
                std::swap(node->_keys[i], node->_keys[i-1]);
                std::swap(node->_subs[i+1], node->_subs[i]);
            }
        }

        node->_keys[idx] = key;
        node->_subs[idx+1] = sub;
        node->_n++;

        if(sub != nullptr)
            sub->_parent = node;
    }

    public:

    // 定义find, 如果关键字已经存在,
    // 返回对应的节点及下标索引
    // 不存在, 返回-1和叶节点
    std::pair<Node*, ssize_t> find(const K& key)
    {
        Node* curr = _root;
        Node* prev = nullptr;

        ssize_t idx = 0;
        while(curr != nullptr)
        {
            auto group = _root->search(key);
            if(group.first == true)
                return {curr, group.second};

            idx = group.second;

            prev = curr;
            curr = curr->_subs[idx];
        }

        return {prev, -1};
    }

    bool insert(const K& key)
    {
        if(_root == nullptr)
        {
            Node* node = new Node();
            node->_keys[0] = key;
            ++node->_n;
            _root = node;
            return true;
        }

        auto group = find(key);

        // 已经存在了, 当前禁止关键字重复
        if(group.second >= 0)
            return false;

        // 不存在, 找到叶节点
        Node* parent = group.first; // 空节点的父节点

        K CirKey = key;
        Node* Sub = nullptr;

        Node* prev = nullptr;
        while(parent != nullptr)
        {
            _insert(CirKey, parent, Sub);
            if(parent->_n != M) return true;

            // 满了, 开始分裂.
            Node* brother = new Node();

            size_t mid = parent->_n / 2;

            // 将[mid+1, M-1]部分转交给兄弟节点
            size_t size = 0;
            for(size_t i = mid + 1; i < M; ++i)
            {
                // 交换一下调试效果更明显
                std::swap(brother->_keys[size], parent->_keys[i]);
                std::swap(brother->_subs[size++], parent->_subs[i]);
            }

            std::swap(brother->_subs[size], parent->_subs[M]);

            brother->_n = size;

            // size转交给brother, 1转交给父节点
            parent->_n -= (size + 1);

            // 把brother与父节点的连接转化成在父节点中插入一个key和节点
            CirKey = parent->_keys[mid];
            Sub = brother;
            parent->_keys[mid] = K();
            prev = parent;
            parent = parent->_parent;

        }

        // 分裂到了根节点
        _root = new Node();
        _root->_keys[0] = CirKey;
        _root->_subs[0] = prev;
        _root->_subs[1] = Sub;
        _root->_n = 1;

        // 子节点建立回指指针
        _root->_subs[0]->_parent = _root;
        _root->_subs[1]->_parent = _root;

        return true;
    }

    private:
    Node* _root = nullptr;
};