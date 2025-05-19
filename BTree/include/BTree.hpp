#pragma once
#include <stddef.h>     // size_t
#include <algorithm>    // std::fill
#include <utility>      // std::pair


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
        std::fill(_keys, _keys + sizeof(_keys), K());
        std::fill(_subs, _subs + sizeof(_subs), nullptr);
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
            _insert(key, parent, Sub);
            if(parent->_n != M) return true;

            // 满了, 开始分裂.
            Node* brother = new Node();

            size_t mid = parent->_n / 2;

            // 将[mid+1, M-1]部分转交给兄弟节点
            size_t size = 0;
            for(size_t i = mid + 1; i < M; ++i)
            {
                // 交换一下调试效果更明显
                std::swap(brother->_keys[size++], parent->_keys[i]);
            }
            brother->_n = size;

            // 这是叶节点, 可以不管子节点

            // size转交给brother, 1转交给父节点
            parent->_n -= (size + 1);

            // 把brother与父节点的连接转化成在父节点中插入一个key和节点
            CirKey = parent->_n[mid];
            Sub = brother;
            parent->_n[mid] = K();
            prev = parent;
            parent = parent->_parent;

        }

        // 分裂到了根节点
        Node* root = new Node();
        root->_keys[0] = CirKey ;
        root->_subs[0] = prev;
        root->_subs[1] = Sub;
        root->_n = 1;









        // // 不存在, 找到叶节点
        // Node* curr = group.first;

        // K CirKey = key;
        // Node* Sub = nullptr;

        // while(true)
        // {
        //     _insert(key, curr, Sub);
        //     if(curr->_n != M) return true;

        //     // 满了, 开始分裂.
        //     Node* brother = new Node();

        //     size_t mid = curr->_n / 2;

        //     // 将[mid+1, M-1]部分转交给兄弟节点
        //     size_t size = 0;
        //     for(size_t i = mid + 1; i < M; ++i)
        //     {
        //         // 交换一下调试效果更明显
        //         std::swap(brother->_keys[size++], curr->_keys[i]);
        //     }
        //     brother->_n = size;

        //     // 这是叶节点, 可以不管子节点

        //     // size转交给brother, 1转交给父节点
        //     curr->_n -= (size + 1);

        //     // 把brother与父节点的连接转化成在父节点中插入一个key和节点
        //     CirKey = curr->_n[mid];
        //     Sub = brother;
        //     curr->_n[mid] = K();
        //     curr = curr->_parent;
        // }



        // _insert(key, curr);

        // if(curr->_n == M)
        // {
        //     // 满了, 开始分裂.
        //     Node* brother = new Node();

        //     size_t mid = curr->_n / 2;

        //     // 将[mid+1, M-1]部分转交给兄弟节点
        //     size_t size = 0;
        //     for(size_t i = mid + 1; i < M; ++i)
        //     {
        //         // 交换一下调试效果更明显
        //         std::swap(brother->_keys[size++], curr->_keys[i]);
        //     }
        //     brother->_n = size;

        //     // 这是叶节点, 可以不管子节点

        //     // size转交给brother, 1转交给父节点
        //     curr->_n -= (size + 1);

        //     // 把brother与父节点的连接转化成在父节点中插入一个key和节点
        //     _insert(curr->_n[mid], curr->_parent, brother);
        //     curr->_n[mid] = K();
        // }

        return true;
    }

    private:
    Node* _root = nullptr;
};