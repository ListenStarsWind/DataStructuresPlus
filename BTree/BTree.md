# B树

## 引入

在之前我们已经学习了下面这些查找结构

| 查找算法                 | 数据格式要求 | 时间复杂度    |
| ------------------------ | ------------ | ------------- |
| 顺序查找                 | 无要求       | O(N)          |
| 二分查找                 | 有序         | O($\log_2 N$) |
| 二叉搜索树               | 无要求       | O(N)          |
| 二叉平衡树（AVL/红黑树） | 无要求       | O($\log_2 N$) |
| 哈希查找                 | 无要求       | O(1)          |

但它们的使用都建立在这样的一个前提下, 那就是处理的数据量并不是特别大, 能够一次性放进内存中, 也就是所谓的"内查找".  对于信息化时代来说, 充斥着各种各样的数据, 这些数据的量都很大, 全部放进内存中是不现实的, 因此我们就需要一种数据结构, 这种数据结构在内存中存储着数据的某些关键字, 但数据的主体并不在内存中, 而是在其他的存储媒介上, 比如, 磁盘, 为了能找到磁盘上的主体数据, 这种数据结构便被设计为`k-v`格式, `k`表示数据的特征关键字, `v`表示主体数据在磁盘上的索引.  

我们可以先使用手头上现有额数据结构, 比如, 平衡搜索树, 和哈希表, 对于哈希表, 我们就直接跳过了, 为什么呢? 因为当数据量很大时, 哈希冲突的频率将会显著增加, 从而造成搜索效率的低下. 

这样, 就剩下了二叉平衡树, 要严谨一下, 强调一下是二叉, 我们可以让二叉搜索树的`k`充当上文所说的数据关键字, `v`充当主体数据在磁盘上的索引. 于是, 就大概有下图.

![image-20250518204233021](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250518204233104.png)

不过上面这张图所采用的策略和上文稍有不同, 它连关键字都没存到内存里, 每个节点单纯只有磁盘索引, 每次关键字比对, 都是直接进行一次IO. 可能数据量实在太大了, 关键字都存不下. 所以内存里只有地址.

比如我们想看一下`5`在不在这棵树里, 那么我们先看根节点, 读磁盘发现根节点的关键字是`34`, `34`比`5`大, 所以我们去左子树, `0x90`指向`22`, 也比`5`大, 那么就继续往左边走, `0x6A`, 指向`5`, 那么就找到了.

每一层我们都需要读一下选中节点指向的磁盘上的关键字, 所以说我们将进行高度次IO, 此时对于现有的二叉平衡树就有一个问题, 那就是高度还是太高了, 诚然, 对于内存来说, 去找一个30层的节点并不是什么难事, 但对于IO来说, 所耗费的时间就不可忽略了.

对于磁盘来说, 真正耗时的部分是定位过程, 在这个过程中, 磁头会以不断摆动的方式确定自身的位置, 并依据文件系统给出的索引, 逐渐靠近目标位置. 具体内容我们不多说, 在Linux的原始文件系统中有相关的介绍

![image-20250518211853625](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250518211853890.png)

算法导论, 在第18章, B树的开头也做过相关的描述.

![image-20250518212020630](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250518212020909.png)  

既然二叉平衡树太高了, 那么多叉平衡树呢? 子树一多, 高度不就降下来了. 另外, 同样为了减小位置的移动次数, 这棵多叉二叉树的每个节点存的可不是一份数据的关键字和对应索引, 而是多份数据的关键字和对应的索引. 也就是说, B树的一个节点里面有多份数据.

1970年，R.Bayer 和 E.mccreight 提出了一种适合外查找的树，它是一种平衡的多叉树，称为B树(后面有一个B的改进版本B+树，然后有些地方的B树写的的是B-树，注意不要误读成"B减树")。一棵m阶(m>2)的B树，是一棵平衡的M路平衡搜索树，可以是空树或者满足一下性质：

1. 根节点至少有两个孩子  
2. 每个分支节点都包含k-1个关键字和k个孩子，其中 ceil(m/2) ≤ k ≤ m ceil是向上取整函数  
3. 每个叶子节点都包含k-1个关键字，其中 ceil(m/2) ≤ k ≤ m  
4. 所有的叶子节点都在同一层  
5. 每个节点中的关键字从小到大排列，节点当中k-1个元素正好是k个孩子包含的元素的值域划分
6. 每个结点的结构为：（n，A0，K1，A1，K2，A2，… ，Kn，An）其中，Ki(1≤i≤n)为关键字，且Ki<Ki+1(1≤i≤n-1)。Ai(0≤i≤n)为指向子树根结点的指针。且Ai所指子树所有结点中的关键字均小于Ki+1。n为结点中关键字的个数，满足ceil(m/2)-1≤n≤m-1。

先不要看规则, 规则太多了, 一起看看不出什么东西.

首先, 需要明确的是, B树其实像一个迷你的文件系统, 它的主体, 也就是节点, 实际上是就存在磁盘上的. 内存里的节点, 实际上就是只有一个磁盘索引, 系统拿到这个磁盘索引之后, 会将节点中的数据加载到内存中,我们将来为了方便, 就直接看那个从磁盘上被加载进来的节点, 而不是原来的那个只有磁盘索引的节点. 它的实际结构就是这张图

![image-20250519132108526](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519132108597.png)

内存节点里面只有磁盘地址, 其它什么都没有, 节点中的真正数据都在磁盘上, 访问一个节点的动作是根据节点上的索引, 找到那个特定的数据块, 然后把它整个一起读到内存中. 下文我们说的那个节点, 实际上是这个读进来的数据块. 

接下来我们要知道, B树有一个名为"分支因子"的重要参数, 通常记为`m`, 正如它的名字所言, "分支因子"描述了一棵B树到底有"多分叉", 一般而言, `m`的取值是`50-2000`, 具体配置视实际需求而定, 对于服务器来说, 一般配置为`1024`, 是一个程序员很熟悉的数字.

B树中的每个节点, 都被分为两层, 或者更直白的说, 有两个数组, 一个数组存放的是关键字, 及其对应的磁盘索引, 一个数组存放子节点的指针, 在下面写代码的时候, 为了简化模型, 第一个数组, 我们暂且就认为是只放关键字, 而不放磁盘索引,  该数组中的关键字呈现升序摆列, 是将来进行路由选择的判断依据, 第一个数组中的每个有效元素, 都有与之对应的两个子节点, 因此, 第二个数组中的节点个数始终是第一个数组中关键字个数的加一.

![image-20250518214838363](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250518214838413.png)

在这张图中, `data1, data2, data3`就是对应的关键字, 在这里为了简化模型, 我们直接认为关键字就是数据本体, 所以用`data`做前缀, 实际上, 它(每个`data`)当然要有对应的磁盘索引 , 用来指引主体数据在磁盘的何处, `data1`左边是`child1`, 右边是`child2`, `data2`左边是`child2`, 右边是`child3`, `data3`左边是`child3`, 右边是`child4`. 

相当于一个顶点缝合了之前AVL或者红黑树中的多个节点, 当有个关键字从上面过来时, 我们首先通过一些手段(二分查找), 找到和关键字相近的`data`, (也有可能直接就找到这个关键字了那样就没有路由选择, 所以我们这里暂且不说),  比如说, 我想找的数字是`3`, 这三个数字分别是`1, 5, 9`, 那么我们找到最近的(最近的说法似乎不准确, 应该是小于该关键字的最大元素), `159`里面`59`都大于`3`, 所以这里找到的`data`就是`1`, 而`3`又在`1`的右边, 所以下一层就会从`1`的右边, 即`child2`出发.

![image-20250519122942109](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519122942226.png)

为了满足一个关键字搭配两个子节点的机制, 我们要求, 在任何情况下, 都应该满足, 子节点的个数恰好是关键字个数的加一.

之前我们说, `m`控制着多叉平衡树的节点规模, 它是这样控制的. 每个节点中的子节点也就是第二层的元素个数, 最少是`m / 2`向上取整, 最多是`m`. 这样设计是为了之后便于树的生长.

因此, 每个节点的结构就是 `(n，A0，K1，A1，K2，A2，… ，Kn，An）`

下面, 我们以`m = 3`的规格, 插入这样的一组数据`{53, 139, 75, 49, 145, 36, 101}  `

1. 插入53

   首先, 我们必须构建一个新的节点, 因为现在的B树实际上只是一棵空树. 按照我们上文所说的, 理应构建一个关键字数组大小为`2`, 子节点数组大小为`3`的节点, 但实际上, 我们需要构建一个关键字数组大小为`3`, 子节点数组大小为`4`的节点, 这是因为, B树对于数据超额的处理态度是先超额放入一份数据, 然后再进行分裂或者生长, 所以要预留一个位置存放多出的那份关键字和子节点

   ![image-20250519134226749](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519134226811.png)

   然后插入一个`35`

   ![image-20250519134358757](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519134358807.png)

2.  插入139

   直接插入

   ![image-20250519134539201](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519134539248.png)

3. 插入75

   关键字是升序排列的, 所以需要进行调整

   ![image-20250519134900233](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519134900275.png)

4. 满了, 需要进行分裂

   我们首先构建两个节点, 分别是它的父节点和兄弟节点(如果有父节点, 那就不用再创建了)

   ![image-20250519135225270](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519135225348.png)

   然后把中位数(就是中间位置)之后的数据转移给兄弟节点, 我们这里`m = 3`, 看得不是很明显, 如果`m = 1024`, 就会把`512`的关键字给兄弟

   ![image-20250519135409002](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519135409070.png)

   把中位数`75`交给父节点

   ![image-20250519135653709](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519135653774.png)然后让父节点在对应的位置连接自己和兄弟节点![image-20250519140140867](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519140140928.png)

5. 插入49, 注意只有叶节点能插入新的数据

   ![image-20250519140345705](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519140345762.png)

6. 插入145

   ![image-20250519140448985](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519140449043.png)

7. 插入36

   ![image-20250519141228236](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519141228292.png)

8. 满了, 再次分裂, 有父节点, 那就不需要再构建父节点了

   ![image-20250519141705791](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519141705856.png)

   把后半部分的关键字交给兄弟节点

   ![image-20250519141836939](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519141837004.png)

   把中间的关键字交给父节点

   ![image-20250519141954853](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519141954913.png)

   父节点的第二层也需要做相应的调整

   ![image-20250519142108192](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142108248.png)

   把兄弟节点也连接上

   ![image-20250519142151295](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142151352.png)

9. 插入101

   ![image-20250519142345061](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142345132.png)

   满了, 再分裂

   ![image-20250519142522200](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142522254.png)

   ![image-20250519142628313](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142628365.png)

   ![image-20250519142822393](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142822457.png)

   ![image-20250519142926070](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142926134.png)

   ![image-20250519143032376](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519143032439.png)

非叶节点只能接收来自他人转移的关键字和子节点, 叶节点负责容纳新的数据. 因为这样就不用担心新节点把子节点的关系弄乱(叶节点根本没有子节点), 叶节点满了, 分裂出一个兄弟节点, 并向上递交一个中间关键字, 负责维护新的兄弟关系. 当根节点分裂时, 即新增了一层,  B树是同层或者向上长的, 它天然平衡.

当`m`很大时, 仅仅两层的B树, 其数据量就能达到十分恐怖的量级, 正如算法导论中所说明的那样

![image-20250519152116353](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519152116457.png)

根节点经常被用, 所以可以直接长久地放到内存里, 不进行释放, 这样的话, 每次路由我们就能深入一层, 所以这样的话, 我们只需要路由三次, 但实际上只要IO两次,  就可以到达这棵B树的任意一个位置.

如果是刚刚分裂出来的两层B树, 那么根节点有1个关键字, 根节点比较特殊, 可以不为`m / 2`, 引出2个子节点, 每个子节点存储500个关键字, 引出501个子节点, 一共是1002个子节点, 之后是更深层, 更深层有1002个子节点, 每个子节点500关键字, 那么就是501000个关键字, 也是挺大的, 最空和最满都是极端, 一般都看不到这种情况.

## 简单实现

下面我们非常简单地实现一下B树, 主要是借助于代码让我们更深入地理解B树的运作原理, 因为B树的规则特别多, 所以我们会采用边写边调试的方式.

我们先写一下树的节点.

```cpp
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
```

对于`_n, _keys, _subs`相信已经不用说了, 不过还需要一个`_parent`需要说一下, 因为B树是向上生长的, 所以要求子节点能找到自己的父节点, 所以需要这个`_parent`来回指自己的父节点. 之后是一个显示写的默认构造, 我们将其中的成员都进行了清空, 方便等会儿进行调试, 现象更加明显.

接下来是一个二分查找, 它利用数组元素的有序性, 将数组分为两个部分, 为了方便起见, 我们先假设数组中有我们要找的元素, 此时, 目标元素小的就是前面的那个区间, 大的就是后面的那个区间, 所以当落到前面那个区间, 需要`left = mid + 1`, 落到后面的那个区间, 需要`right = mid - 1`,

最后为了分辨到底能不能找到, 我们的返回值是个`pair`, `first`表示有没有找到, 如果能找到的话`second`就是目标元素的下标, 如果找不到, `second`返回的是应该插入的位置,

接下来, 我们来写`insert`, 对于第一个节点的插入, 很简单, 因为此时B树就是空的, 所以直接创建一个, 往里面填就行了.

```cpp
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

}
```

我忽然发现上面的图画错了, 第一个元素是`53`, 但我们当做`35`来插入了, 不过上面的图在逻辑上还是没错的, 接下来, 我们的用例就把第一个元素当做`35`

```cpp
#include"BTree.hpp"

#include<iostream>

using namespace std;

typedef BTree<int, 3> BT;

void TestBTree()
{
    int arr[] = {35, 139, 75, 49, 145, 36, 101};

    BT o;

    for(auto e : arr)
    {
        o.insert(e);
    }
}

int main()
{
    cout <<"hello BTree"<<endl;
    TestBTree();
    return 0;
}
```

![image-20250521110934604](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250521110935071.png)

![image-20250521111003122](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250521111003538.png)

接下来就比较麻烦了,  首先, 我们知道B树都是在叶节点进行插入的, 所以我们先写一下`find`, 用来查找一个关键字, 如果找不到, 就返回`-1`下标, 如果能找到, 就返回该关键字的下标, 于是就有最基本的相关代码

```cpp
ssize_t find(const K& key)
{
 	Node* curr = _root;
    
    while(curr != nullptr)
    {
        auto group = curr->search(key);
        
        if(grout.first == true)
            return group.second;
       
        // 说明没找到, 此时的second恰好就是该路由的下一个节点
        curr = curr->_subs[group.second];
    }
    
    return -1;
}
```

但为了后面能获得对应的叶节点, 我们再添加一个`prev`

```cpp
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
```

如果它返回的是已经找到, 由于我们当前的B树是不支持冗余关键字的, 所以直接返回false, 如果找不到, 那我们就接收相应的叶节点指针, 在这个叶节点指针上插入新的数据

```cpp
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
    
    if(group.second >= 0)
        return false;
    
    Node* parent = group.first;
    
    _insert(key, parent);
}
```

我们这里就把叶节点叫做`parent`, 意为"叶节点的父节点", 在后面, 这个`parent`还会有更多的含义, 在这之后, 我们调用`_insert`, `_insert`的功能是向指定的一个节点中插入一个关键字, 在这之后, 如果叶节点的关键字数目满了, 就需要进行分裂

```cpp
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
    
    if(group.second >= 0)
        return false;
    
    Node* parent = group.first;
    
    _insert(key, parent);
    
    if(parent->_n != M)
        return true;
    
    Node* brother = new Node();
    
    
}
```



































```cpp
#pragma once
#include <stddef.h>     // size_t
#include <algorithm>    // std::fill


template <class K, size_t M>
    struct BTreeNode
    {
        private:
        typedef BTreeNode<K, M> self;

        public:
        size_t _n;          // 记录关键字的个数

        K _keys[M];         // 关键字数组
        self* _subs[M + 1]; // 子节点指针数组

        BTreeNode(){
            // 初始化, 数据清空
            _n = 0;
            std::fill(_keys, _keys + sizeof(_keys), K());
            std::fill(_subs, _subs + sizeof(_subs), nullptr);
        }

        // 二分查找, 如果target在关键字数组中, 返回索引
        // 否则, 返回将要插入的位置
        ssize_t search(const K& target)
        {
            if(_n == 0) return 0;

            ssize_t left = 0, right = _n - 1;
            while(left <= right)
            {
                ssize_t mid = left + (right - left) / 2;
                if(_keys[mid] < target)
                    left = mid + 1;
                else if(_keys[mid] > target)
                    right = mid - 1;
                else
                    return mid;
            }

            return left;
        }
    };

template <class K, size_t M>
    class BTree
    {
        typedef BTreeNode<K, M> Node;

        public:
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

            return false;
        }

        private:
        Node* _root = nullptr;
    };


#include"BTree.hpp"

#include<iostream>

using namespace std;

typedef BTree<int, 3> BT;

void TestBTree()
{
    int arr[] = {53, 139, 75, 49, 145, 36, 101};

    BT o;

    for(auto e : arr)
    {
        o.insert(e);
    }
}

int main()
{
    cout <<"hello BTree"<<endl;
    TestBTree();
    return 0;
}
```

最开始, 很简单, 因为压根没有根节点, 所以直接创建一个节点, 把关键字扔进去就行.

![image-20250519165345466](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519165345624.png)

![image-20250519165411376](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519165411524.png)

接下来就不好写了, 那么首先, 由于新关键字都插入叶节点, 所以先要找到叶节点.

31234
