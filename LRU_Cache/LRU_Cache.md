# LRU Cache  

## 目的

在计算机系统中, 缓存是一个很常见的概念, 无论是CPU与内存之间的三级缓存, 还是网络临时文件, 亦或者数据库中的缓存, 等等. 缓存一般来说, 比较小, 这就使得它经常面临一个问题, 那就是缓存本身已经满了, 但现在需要添加新数据, 此时就要把缓存中已有的一些数据给移除出去, 对于这些数据的移除优先级, 我们采用计算机常用的局部性原则, 计算机认为, 经常被用的数据很可能以后也会被经常用, 所以当要把某些数据移除时, 我们就选择最少使用的那些数据, 把他们移除出去, `cache`就是缓存, `LRU `则是`Least Recently Used  `, 即最近最少使用.

由于缓存就是因为数据传输高效才被作为缓存, 所以对于一个`LRU Cache  `, 数据结构来说, 对其的使用都要由$O(1)$时间复杂度实现. 具体来说, 它的增删查改, 都要是常数级别的时间复杂度.

## 实现

力扣上面有一道对应的题目, 而且做了抽象处理, [LRU 缓存](https://leetcode.cn/problems/lru-cache/), 他把数据以`key-value`的形式进行组织, 在这里, 它的`key, value`都是整型, 而在实际当中, `key`可能是地址, `value`则是内存数据.

![image-20250517164729434](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250517164729518.png)

对于`LRU` 缓存, 首先我们要用一种方式判断这份数据最近是不是最少被使用, 复杂的方式包括建立专门的时间管理结构, 但那样太复杂了, 在这里, 我们使用链表, 作为`LRU`的底层容器, 我们依据节点在链表中的位置, 判断它最近是经常被使用还是很少被使用, 具体是这样做的, 对于最新被访问的数据, 无论是本来就有的, 还是从外面插入的, 还是读, 亦或者修改, 都把对应的节点放到链表的最前面, 这样的话, 链表末尾的自然就是最近使用最少的了, 当然, 你也可以把位置倒过来.

另外, 对于数据的查找, 为了能实现常数时间复杂度, 我们再准备一个哈希, 用来存储, 找到对应`k`的索引, 这里我们就用迭代器, 也就是说, 哈希是`key-iterator`的形式, 而那个`iterator`, 就指向了链表中的一个特定节点, 该节点中存储着对应的`val`

```cpp
class LRUCache {
    int size()
    {
        // 链表的size有的STL版本是遍历
        // 得到的所以我们用哈希表的大小
        return _link_table.size();
    }

    public:
    LRUCache(int capacity) :_capacity(capacity){

    }

    int get(int key) {
        // 取出连接表的迭代器
        auto it = _link_table.find(key);

        // 找不到
        if(it == _link_table.end())
            return -1;

        // 获取节点索引
        link node = it->second;

        // 把被修改的节点放到前面, 有两种方法
        // 1. 节点释放, 重新头插
        // 2. 节点保存, 调整在链表中的逻辑位置

        // 1. 要重新更新链接表中的迭代器
        // 2. 物理位置没有改变, 不用更新

        // splice既可以把节点转交给别的链表, 
        // 也能自己调整逻辑位置

        // 把placeholders::_3的节点移到_1前面
        _data.splice(_data.begin(), _data, node); 

        return node->second;
    }

    void put(int key, int value) {
        auto it = _link_table.find(key);

        if(it == _link_table.end())
        {

            // 已经满了
            if(size() == _capacity)
            {
                _link_table.erase(_data.back().first);
                _data.pop_back();
            }

            // 插入
            _data.emplace_front(key, value);
            _link_table[key] = _data.begin();
        }
        else
        {
            // 修改
            link node = it->second;
            node->second = value;

            // 把被修改的节点放到前面, 有两种方法
            // 1. 节点释放, 重新头插
            // 2. 节点保存, 调整在链表中的逻辑位置

            // 1. 要重新更新链接表中的迭代器
            // 2. 物理位置没有改变, 不用更新

            // splice既可以把节点转交给别的链表, 
            // 也能自己调整逻辑位置

            // 把placeholders::_3的节点移到_1前面
            _data.splice(_data.begin(), _data, node);
        }
    }

    private:
    typedef list<pair<int, int>>::iterator link;

    int _capacity;
    list<pair<int, int>> _data;
    unordered_map<int, link> _link_table;
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */
```

