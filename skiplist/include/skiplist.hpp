#pragma once

#include <climits>  // INT_MIN
#include <cstdlib>  // rand
#include <iomanip>  // std::setw
#include <iostream>
#include <vector>
#include <random>  // std::default_random_engine, std::uniform_real_distribution
#include <chrono>  // chrono

struct SkiplistNode
{
    int _val;
    std::vector<SkiplistNode*> _next;
    SkiplistNode() : _val(INT_MIN), _next(1) {}
    SkiplistNode(int val, int level) : _val(val), _next(level) {}
};

class Skiplist
{
    typedef SkiplistNode Node;

    std::vector<Node*> find(int target, int level_size = 1)
    {
        if (level_size > _head->_next.size())
        {
            _head->_next.resize(level_size);
        }

        Node* curr = _head;
        int level = level_size - 1;

        std::vector<Node*> prev(level_size);
        while (level >= 0)
        {
            Node* next = curr->_next[level];
            if (next == nullptr || next->_val >= target)
            {
                prev[level--] = curr;
            }
            else if (next->_val < target)
            {
                curr = next;
            }
        }

        return prev;
    }

    int randomlevel()
    {
        int level = 1;
        while (rand() < RAND_MAX * _p && level <= _maxLevel)
        {
            ++level;
        }

        return level;
    }

    int RandomLevel()
    {
        static std::default_random_engine generator(
            std::chrono::system_clock::now().time_since_epoch().count());
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        size_t level = 1;
        while (distribution(generator) <= _p && level < _maxLevel)
        {
            ++level;
        }
        return level;
    }

   public:
    Skiplist()
    {
        srand(time(nullptr));
        _head = new Node();
    }

    bool search(int target)
    {
        std::vector<Node*> prevV = find(target);
        Node* next = prevV[0]->_next[0];
        return next == nullptr ? false : next->_val == target;
    }

    void add(int num)
    {
        int level_size = randomlevel();
        std::vector<Node*> prev = find(num, level_size);
        Node* newnode = new Node(num, level_size);

        for (int i = 0; i < level_size; ++i)
        {
            newnode->_next[i] = prev[i]->_next[i];
            prev[i]->_next[i] = newnode;
        }
    }

    bool erase(int num)
    {
        int level_size = _head->_next.size();
        std::vector<Node*> prev = find(num, level_size);
        Node* next = prev[0]->_next[0];
        if (next == nullptr || next->_val != num)
            return false;

        level_size = next->_next.size();
        for (int i = 0; i < level_size; ++i)
        {
            prev[i]->_next[i] = next->_next[i];
        }

        delete next;

        int level = _head->_next.size() - 1;
        while (level >= 0)
        {
            if (_head->_next[level] == nullptr)
                --level;
            else
                break;
        }

        _head->_next.resize(level + 1);

        return true;
    }

    void print()
    {
        // 收集所有节点
        std::vector<SkiplistNode*> nodes;
        SkiplistNode* curr = _head->_next[0];
        while (curr)
        {
            nodes.push_back(curr);
            curr = curr->_next[0];
        }

        // 计算总层数
        int total_levels = _head->_next.size();

        // 打印每一层，从高到低
        for (int level = total_levels - 1; level >= 0; --level)
        {
            std::cout << "Level " << std::setw(2) << level + 1 << ": ";
            curr = _head->_next[level];
            int idx = 0;
            for (size_t i = 0; i < nodes.size(); ++i)
            {
                // 若当前层不存在某个值，填空格
                if (nodes[i]->_next.size() > level && curr == nodes[i])
                {
                    std::cout << std::setw(5) << nodes[i]->_val;
                    curr = curr->_next[level];
                }
                else
                {
                    std::cout << std::setw(5) << " ";
                }
            }
            std::cout << std::endl;
        }
    }

   private:
    Node* _head;
    double _p = 0.25;
    int _maxLevel = 32;
};