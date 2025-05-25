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
        auto group = curr->search(key);
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

首先我们需要把后面的部分交给兄弟节点

```cpp
Node* brother = new Node();
size_t mid = parent->_n / 2;

// 将`[mid+1, M-1]`的区域转交给兄弟节点
size_t size = 0;
for(size_t i = mid + 1; i < M; ++i)
{
    // 交换一下调试的效果更好
    swap(parent->_keys[i], brother->_keys[size++]);
    // 由于这里是叶节点, 所以不需要转移子节点? 
}

brother->_n += size;

// 多减一个一是因为要把mid交给父节点
// 在这里是parent的parent
parent->_n -= (size+1);
```

接下来一个很关键的地方来了, 现在我们要把一个关键字`mid`和它右边的子节点`brother`放进父节点, 这相当于指定一个节点, 往里面插入一个关键字和对应的子节点, 所以我们需要修正一下`_insert`的功能, 指定一个节点, 向其中插入关键字和子节点, 对于叶节点来说, 子节点被缺省为空节点., 这样一转化之后, 就相当于要走循环了.

下面我们就来写一下`_insert`

```cpp
void _inser(key, node, sub)
{
    // 找到应该插入的位置
    idx = node->search(key);
    
    // 如果等于n, 说明就是尾插
    // 否则需要挪动数据
    if(idx == node->_n)
    {
        // 尾插被转化成向idx位置插入
    }
    else
    {
        // 这个node可不一定是叶节点, 所以需要关键字
        // 子节点都挪动
       	for(ssize_t i = node->_n; i > idx; --i)
        {
            swap(node->_keys[i], node->_keys[i-1]);
            swap(node->subs[i+1], node->_subs[i]);
        }
    }
    
    node->_keys[idx] = key;
    node->_subs[idx+1] = sub;
    node->_n++;
    
    // 不要忘了让子节点回指父节点
    if(sub)
        sub->_parent = node;
}
```

在下一轮循环中, 就是向父节点插入关键字`[mid]`和子节点`brother`, 需要注意的是, 由于现在`parent`已经不是叶节点了, 所以就像是上面的问号一样, 你也需要把`parent`的子节点进行挪动

```cpp
bool insert(key)
{
    if(_root == nullptr)
    {
        // .......
    }

    // 找叶节点
    group = find(key);

    // 已经存在返回false
    if(group.second >= 0)
        return false;

    // 每轮循环中要向其插入内容父节点
    Node* parent = group.first;

    // 插入的关键字
    K CirKey = key;
    // 插入的子节点
    Node* Sub = nullptr;

    while(/*...*/)
    {
        _insert(key, parent);

        if(parent->_n != M)
            return true;

        Node* brother = new Node();

        size_t mid = parent->_n / 2;

        // 将`[mid+1, M-1]`的区域转交给兄弟节点
        size_t size = 0;
        for(size_t i = mid + 1; i < M; ++i)
        {
            // 交换一下调试的效果更好
            swap(parent->_keys[i], brother->_keys[size]);
            // 由于这里是叶节点, 所以不需要转移子节点? 
            swap(parent->_subs[i], brother->_subs[size++])

                /*! 后续发现这里有问题, 那就是没有更新转移节点的_parent 在中序遍历之后有insert的完整代码 
                后面的调试代码的截图用的也是这段错代码, 不要看   */
        }
        // 每个关键字配两个子节点
        swap(parent->_subs[M], brother->_subs[size])

            brother->_n += size;

        // 多减一个一是因为要把mid交给父节点
        // 在这里是parent的parent
        parent->_n -= (size+1);   

        // 把问题转化成向父节点插入一个关键字和子节点
        CirKey = parent->_keys[mid];
        Sub = brother;
        parent->_keys[mid] = K();
        parent = parent->_parent;
    }
}
```

下面我们看一下循环条件, 当`parent`为空时, 意味着上一轮节点是根节点, 所以此时就要构建新的根节点, 将上一轮的两个节点和`mid`插入进来

```cpp
bool insert(key)
{
    //....

    Node* prev = nullptr;
    while(parent)
    {
        // ...

        // 把问题转化成向父节点插入一个关键字和子节点
        CirKey = parent->_keys[mid];
        Sun = brother;
        parent->_keys[mid] = K();
        prev = parent;
        parent = parent->_parent;
    }

    _root = new Node();
    _root->_keys[0] = CirKey;
    _root->_subs[0] = prev;
    _root->_subs[1] = brother;
    _root->_n = 1;

    prev->_parent = _root;
    Sub->_parent = _root;
    
    return true;
}
```

下面我们就来调试一下

第一个元素, 35

![image-20250522162646083](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522162646302.png)

![image-20250522162719120](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522162719318.png)

第二个元素 139

![image-20250522162810293](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522162810574.png)

直接插入

![image-20250522162909713](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522162909974.png)

接下来是第三个元素75, 满了, 触发分裂行为

![image-20250522163014428](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522163014741.png)

139被移交给了兄弟节点

![image-20250522163127261](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522163127517.png)

出了一点小问题, 不过不是我们的问题, [1]位置现在应该已经变为零了, 但code的图形化界面没有刷新出来

![image-20250522163700585](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522163700833.png)

我们看到现在它已经刷新出来了

![image-20250522163838620](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522163838896.png)

错的又出点小问题, 重新开了一轮

![image-20250522164211552](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164211786.png)

现在就出循环, 创建新的根节点

![image-20250522164300434](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164300695.png)

![image-20250522164332487](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164332733.png)

![image-20250522164409223](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164409455.png)

现在我们这个B树已经变成这样了

![image-20250519140140867](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519140140928.png)

接下来是下一个元素49

![image-20250522164714548](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164714825.png)

直接插入

![image-20250522164803898](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164804096.png)

接下来是145

![image-20250522164849871](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164850143.png)

也是直接插入

![image-20250522164921255](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164921573.png)

下一个是36

![image-20250522164951672](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522164951942.png)

触发了分裂行为

![image-20250522165021400](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165021674.png)

![image-20250522165119186](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165119464.png)

![image-20250522165149635](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165149905.png)

![image-20250522165221227](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165221476.png)

现在就是这种情况

![image-20250519141836939](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519141837004.png)

也是直接插入

![image-20250522165351292](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165351596.png)

接下来是101

![image-20250522165445884](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165446159.png)

满了, 也要分裂

![image-20250522165528285](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165528534.png)

![image-20250522165615777](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522165616030.png)

又出了一些小情况, 再来一轮

![image-20250522170219096](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522170219333.png)

现在就是这种情况, 要把139插入到上面

![image-20250519142522200](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142522254.png)

![image-20250522170415379](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522170415668.png)

再次触发了分裂机制

![image-20250522170455686](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522170455962.png)

![image-20250519142822393](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142822457.png)

![image-20250522170659617](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522170659940.png)

![image-20250522170725635](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522170725914.png)

![image-20250519142926070](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250519142926134.png)

![image-20250522170852193](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522170852425.png)

![image-20250522171009067](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250522171009367.png)

---------

对于 B 树的删除，我们这里就不详细展开了，说实话我自己也还没实现过，精力也不够。不过大致思想是这样的：

删除一个非叶子节点的关键字时，我们不直接删它，而是从它的子节点中“借”一个关键字来替代它。一般是取左子树的最大值（前驱）或右子树的最小值（后继），把这个值提上来，把原本的问题转化成删除子节点中的关键字。

这样一层层往下推进，最终问题都会变成删除一个**叶子节点**中的关键字。如果该叶子节点的关键字够多，那直接删就好。如果删完之后关键字数量不够了，就要“借”关键字 —— 从左右兄弟节点中借一个，通过父节点“中转”来维护平衡。

借不到怎么办？那就只能合并节点了：把当前节点、它的兄弟节点，还有父节点中的一个关键字合并成一个新节点。这个合并操作可能导致父节点关键字数量不足，于是就要递归地往上处理，直到整棵树重新平衡。

-------

最后我们简单写一下B树的中序遍历 大致思想还是先左后右

![image-20250523201317904](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523201318082.png)

```cpp
void _inorder(Node* root)
{
    if(root == nullptr)
        return;

    size_t n = root->_n;
    for(size_t i = 0; i < n; ++i)
    {
        _inorder(root->_subs[i]);
        std::cout<<root->_keys[i]<<" ";
    }
    _inorder(root->_subs[n]);
}

void inorder()
{
    _inorder(_root);
}
```

```cpp
[whisper@starry-sky build]$ ./BTree 
hello BTree
35 36 49 75 101 139 145 
```

刚刚打印的时候发现了一些问题, 在前面已经备注过了, 在此我把`insert`直接贴在这里

```cpp
bool insert(const K& key)
{
    if (_root == nullptr)
    {
        Node* node = new Node();
        node->_keys[0] = key;
        ++node->_n;
        _root = node;
        return true;
    }

    auto group = find(key);

    // 已经存在了, 当前禁止关键字重复
    if (group.second >= 0)
        return false;

    // 不存在, 找到叶节点
    Node* parent = group.first;  // 空节点的父节点

    K CirKey = key;
    Node* Sub = nullptr;

    Node* prev = nullptr;
    while (parent != nullptr)
    {
        _insert(CirKey, parent, Sub);
        if (parent->_n < M)
            return true;  // 不需要分裂，直接插入成功

        // 需要分裂当前节点
        size_t mid = parent->_n / 2;
        K upKey = parent->_keys[mid];  // 上升的中间 key

        Node* brother = new Node();

        // 将右半部分 [mid+1, M-1] 搬到 brother 中
        size_t j = 0;
        for (size_t i = mid + 1; i < M; ++i, ++j)
        {
            brother->_keys[j] = parent->_keys[i];
            parent->_keys[i] = K();  // 清理原节点

            brother->_subs[j] = parent->_subs[i];
            if (brother->_subs[j])
                brother->_subs[j]->_parent = brother;

            parent->_subs[i] = nullptr;
        }

        // 最后一个子节点
        brother->_subs[j] = parent->_subs[M];
        if (brother->_subs[j])
            brother->_subs[j]->_parent = brother;
        parent->_subs[M] = nullptr;

        brother->_n = j;
        parent->_n = mid;  // 原节点只保留前半部分

        // 更新循环变量
        CirKey = upKey;
        Sub = brother;
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

```

```cpp
typedef BTree<int, 3> BT;

// void TestBTree()
// {
//     int arr[] = {35, 139, 75, 49, 145, 36, 101};

//     BT o;

//     for(auto e : arr)
//     {
//         o.insert(e);
//     }
//     o.inorder();
// }

typedef BTree<int, 3> BT;

void TestBTree()
{
    auto RunTest = [](const std::vector<int>& arr, const std::string& name)
    {
        std::cout << "=== " << name << " ===\n";
        BT tree;
        for (auto e : arr)
            tree.insert(e);
        tree.inorder();
        std::cout << "\n\n";
    };

    RunTest({10, 20}, "Test Case 1: Minimal insert");
    RunTest({10, 20, 30, 40}, "Test Case 2: Trigger root split");
    RunTest({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, "Test Case 3: Ordered insert");
    RunTest({100, 90, 80, 70, 60, 50, 40, 30, 20, 10}, "Test Case 4: Reverse insert");
    RunTest({50, 20, 80, 10, 30, 60, 90, 25, 35, 70, 100}, "Test Case 5: Interleaved insert");

    std::vector<int> largeCase;
    for (int i = 1; i <= 50; ++i)
        largeCase.push_back(i);
    RunTest(largeCase, "Test Case 6: Bulk insert (1~50)");
}

int main()
{
    cout <<"hello BTree"<<endl;
    TestBTree();
    return 0;
}
```

```shell
[whisper@starry-sky build]$ ./BTree 
hello BTree
=== Test Case 1: Minimal insert ===
10 20 


=== Test Case 2: Trigger root split ===
10 20 30 40 


=== Test Case 3: Ordered insert ===
1 2 3 4 5 6 7 8 9 10 


=== Test Case 4: Reverse insert ===
10 20 30 40 50 60 70 80 90 100 


=== Test Case 5: Interleaved insert ===
10 20 25 30 35 50 60 70 80 90 100 


=== Test Case 6: Bulk insert (1~50) ===
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 

[whisper@starry-sky build]$ 
```

最后我们稍微算一下二叉树的效率.

首先我们要把元素个数`N`算出来, 我们不严谨的算一下, 严格来说, 对于分支因子为`M`的B树来说, 其最满的时候, 第一层应该是`M-1`个关键字, 但我们这里就看成`M`了, 第二层应该是`M * (M-1)`个关键字, 但我们就当成`M*M`了...

这样的话, 对于一个深度为$h$的B树来说, 就有其关键字个数$N$的表达式
$$
N = M + M^2 + M^3 + ... + M^h;
$$

$$
MN = M^2 + M^3 + ... + M^h + M ^{h+1}
$$

$$
MN-N = M^{h+1}-M
$$

$$
MN -N + M= M^{h+1}
$$

$$
MN = M^{h+1}
$$

$$
log_M{MN} = h+1
$$

$$
log_MM + log_MN = h + 1
$$

$$
1 + log_MN = h + 1
$$

$$
log_MN = h
$$

## B树的变形

很多人觉得B树的规则过于复杂, 实用性不是很高, 所以将其简化, 就有了B+树.

![image-20250523211103497](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523211103886.png)

B树还是搜索树的样子, B+树就已经不太像了, B+取消了原先B树节点中最左边的子节点, 比如在上图中, `[5, 10, 20]`的`p1`指向的数据都是大于等于5, 而又小于10的数据, 这样, B+树的子节点和分支节点中的关键字就是明确的一一对应关系

B+树已经分层了, 叶节点才是真正存储数据的节点, 分支节点仅仅是找到叶节点的路由判断依据, 也就是说, B+树已经出现了路由层和数据层的分化, 分支节点中存储的都是对应子节点的最小值, 这样的话, 由于路由层已经单纯起路由作用, 所以其内部已经不再含有关键字和主体数据的映射关系, 这样大大减少非叶节点的数据量, 从而能将其尽可能多的从磁盘缓存到内存中, 从而便于快速路由, 因此, B+树是目前最常用的具体B树.

另外为了方便地进行相邻叶节点的操作, 叶节点之间使用直接链接到了一起., 也就是上图中的`Q`.

下面, 我们以`{53, 139, 75, 49, 145, 36, 101}`为示例, 简要模拟一下B+树的分裂过程, M依旧选为3.

最开始, B+树就是空树, 没什么好说的

一旦有数据插入, B+树就要形成路由层好数据层两个部分, 非叶节点承担路由功能, 叶节点真正存储数据.

![image-20250523214007413](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214007556.png)

有元素, 还是先放叶节点里, 放不下了再新建一个叶节点

![image-20250523214033316](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214033466.png)

![image-20250523214054207](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214054345.png)

当遇到一个比索引更小的元素, 索引便需要进行更新

![image-20250523214159868](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214200038.png)

现在, 叶节点已经满了, 需要进行分裂, 我们把叶节点的后半部分拿到一个新的叶节点上, 并把其中开头的, 实际上就是最小的关键字放到路由层

![image-20250523214502021](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214502192.png)

当然, 叶节点还有一个指针再进行连接, 不过这里没画.

![image-20250523214727521](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214727680.png)

![image-20250523214751795](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214751967.png)

![image-20250523214820491](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214820670.png)

![image-20250523214903918](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250523214904127.png)

叶节点与叶节点之间是用指针以链表形式进行管理的, 叶节点内部大多采用的是数组, 但也可能用链表

B+树满了提上去的关键字是数组叶子结点的开头元素, 而不是B树的中位数方案.

除了B+树, 还有一个B\*树, B\*树是在B+树的基础上优化而来的, 它主要是从空间利用率的角度来进行优化的, 不过实际上, 平时我们用的最多的还是B+树, 因为数据都存在磁盘上了, 所以不太在乎空间利用率, 人们更喜欢使用结构规则更简单的B+树.

![image-20250524201448750](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524201449241.png)

B\*树一方面, 在非叶节点也引入了链接指针, 另一方面, 它对超额数据的处理策略进行了优化, 对于B\*的节点来说, 其关键字的数量是`[2/3*M, M]`这个区间, 当一个节点满后, 不进行分裂, 而是把值尝试移到兄弟节点, 如果兄弟也满了, 才会和兄弟一起分裂, 它们会共同构造出一个新的兄弟节点, 并把自身`1/3`的数据移给它, 所以, 这个新的节点就会拥有`2/3`的关键字容量.

![image-20250524202213206](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524202213302.png)

![image-20250524202304319](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524202304434.png)

![image-20250524202440133](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524202440252.png)

![image-20250524202956791](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524202957012.png)

通过以上介绍，大致将B树，B+树，B*树总结如下：

B树：有序数组+平衡多叉树；

B+树：有序数组链表+平衡多叉树； 

B*树：一棵更丰满的，空间利用率更高的B+树。

## B树的应用

B树(系列)最常见的应用就是用来做索引。索引通俗的说就是为了方便用户快速找到所寻之物，比如：书籍目录可以让读者快速找到相关信息. 

B树(系列)一般不在内存里面用, 一方面, B树它们占的空间太多了, 另一方面, B树确实大大降低了高度, 但对于内存来说, 高度3和30没什么区别, 毕竟内存肯定是比磁盘快多了, 毕竟缓存的一个作用, 就是用来作为低速的这些外设和CPU这种高速的计算机部件建立一个缓冲区. 

所以B树一般被用在磁盘上, 比如操作系统的文件子系统, 还有数据库的引擎, 我还没系统学过数据库, 我只能感性的理解说, 数据库的引擎就是帮助数据库完成其数据索引的组件, 有引擎, 就能找到数据库中的特定数据或者确认找不到.,

后面我们会学数据库的MySQL. 它是目前市面上主流的数据库.

![image-20250524204915093](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524204915364.png)

数据库是个小系统(其实很大, 很复杂), 里面分了很多层, 一般来说, 有这种进行大量数据存储的需求, 都是公司组织, 反正不是个人, 所以很明显, 将数据从磁盘移到客户端需要借助于网络, 所以数据库都是CS模型, 这些我们在Linux那边已经说过了, 所以数据库肯定是有网络层的, 比如会话层表示层什么的, 

SQL实际上是一种数据库语言, 不同的数据库在细节上有些区别, 但主体上都是相同的.

在上面, 我们可以看到`catch buff`, 这实际上就是像我们之前说B+树的时候那样, 为了能够进行数据的查找, 必然需要非叶节点的路由, 对于B树来说, 由于每个节点既有关键字又有映射的磁盘地址, 所以节点就会比较大, 而B+树的非叶节点只有关键字, 所以就比较小, 因此就可以在缓存中放更多的非叶节点来帮助路由.

我们的B树在这里.

![image-20250524210344670](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524210344929.png)

MyISAM和InnoDB是MySQL的引擎, 它们都是用B+树实现的.

在使用数据库之前, 要做的一件事, 就是建表. 表自然就有行列之分, 

![image-20250524211013959](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524211014129.png)

表的一行就是一份数据的各种信息, 比如在上图中, 每一行都是描述某个人的数据, 第三行就是这个人的名字, 前面两个就是这个人的其它一些描述性数字, 每一行都相当于是一个小文件, 我们可以通过对应的磁盘地址来找到它们.

对于上面的这种图, 我们就可以写一个这样的SQL语句, 用来造表

```sql
CREATE TABLE PersonInfo
{
PersonID int,
name varchar(255),
address varchar(255),
...
};
```

把这份语句发给MySQL Server, 它就需要建立一棵B+树, 它将根据用户的指示, 选择每份数据中的某一项, 也就是表中的某一列, 作为B+树的关键字. 在上图中 我们就把第一列的数据作为了B+树的关键字, 我们把这种向MySQL指定B+树关键字的行为称为"指定主键", 如果没有比较适合作为主键的数据项, 可以选择自增主键, 也就是MySQL自己给一一数据编号. 

此时这个表实际上就变成了B+树的数据层, B+树的非叶节点平时也会被存到磁盘中, 然后常使用的, 或者最近使用的, 就会被缓存到catch里, 便于快速路由, 当然, 如果数据量不大, 能把所有非叶节点都缓存进来, 那么就应该全部缓存到内存中.

在对数据进行查找时, 可以使用主键的数据项进行查找, 也可以使用非主键的, 但推荐使用主键项进行查找, 因为主键是可以靠B+树进行路由的, 但非主键, 比如姓名则要靠穷举全部数据来进行线性搜索, 如果你真的有使用非主键进行查找的需求, 那可以让MySQL对这个非主键也建立B+树, , 那这个非主键B+树, 和主键B+树, 有什么区别呢? 区别就是, 非主键B+树里面存的是主键的数据项, 最后的数据查找仍旧是使用主键B+树进行的, 比如上图的`Rose`, 会先查出来一个`50`, 然后再用`50`这个主键去依据主键B+树查找磁盘上的数据, 

上面的描述不太准确, 把MySQL的两个引擎混着讲了, 下面我们就`MyISAM`, 和`InnoDB`, 来分着说一下, 它们里面的B+树.

首先, 不论是, `MyISAM`还是`InnoDB`, 它们的B+树都分为两种, 一种是主键B+树, 另一种是索引B+树, 主键必须保证唯一性, 索引可以不唯一, 所以我们如果要把一些人的相关数据放进MySQL里, 那么主键就应该使用确保唯一的数据项, 也就是每个人的身份ID, 但身份ID太长了, 不好记, 比如我们可能更多的使用人名来进行查找, 对非主键, 且未建立索引的数据项进行查找, 会采用一种名为"全表扫描"的方式, 顾名思义, 就是把整个表扫描一遍, 找得到那就是找得到, 找不到自然就是找不到, 很明显, 这种查找方式效率太低, 所以为了增加查找效率, 就可以对常用的数据项再单独建立一份索引树. 

`MyISAM`和`InnoDB`无论是主键树还是索引树默认采用的都是B+树(也可以用哈希), 但在某些细节上不同.  

`MyISAM`, 对于主键树, 或者索引树, 所采用的都是非聚集式索引. 在这种索引机制中, 每一份数据都像是一个小文件, 或者说每一行都像是一个小文件, 其B+树的叶节点, 指向的都是这些小文件的文件地址, 我们可以看到, 每个关键字都有与之对应的文件地址

![image-20250524211013959](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250524211014129.png)

`MyISAM  `总体上来说, 已经比较优秀了, 但有一个非常致命的问题, 那就是事务, 所谓事务, 就是说, 有些行为必须成功或者失败, 只允许两种状态, 不允许存在中间态, 比如, 一个人向另一个转账, 那么这个人的账户就要减少一定金额, 另一个人就要多相应的金额, 不能说, 一个人多了, 另一个人没少, 或者其它中间状况. 从系统角度, 对于一个事务来说, 我们必须进行原子化的操作.

`MyISAM  `建立其它索引树的时候, 其叶节点映射的也是每行对应的文件地址, 比如, 在下图中, 使用第二行进行索引建树

![image-20250525195043025](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525195043208.png)

`InnoDB`则是新一代的MySQL引擎, 支持事务, 它所采用的是聚集式索引, 具体来说, 每个叶节点现在并不是各个关键字各映射各的, 而是一起映射到同一份文件上, 或者说, 对于`InnoDB`来说, 每份小文件里面存的是一个叶节点的所有关键字所映射的数据

![image-20250525195456862](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525195456928.png)

比如, 在上图中, 第一个叶节点, 关键字是`15, 18`的那个, 它们的数据都被存到同一个文件里面, 而不是像`MyISAM`, 每个关键字各对应一个文件. 这种方法一方面减少了文件的个数, 这样的话, 对于同一个叶节点的数据, 就不需要再更换文件, 因为它们就在同一个文件里面, 所以就减少了文件定位的次数, 从而优化了效率. 但这种设计思路引发了一个小问题, 那就是, 再建立别的索引树时, 由于临近的主键关键字对应的数据都放在同一个文件里, 所以大家的解耦性就高了, 就是多份关键字被捆绑到同一份文件上, 所以没有`MyISAM`自由灵活. 

比如我现在想用第二列的数据项再建立一个索引树, 这样的话, `5, 22`就可能在一个叶节点里面, 但我们看主键树, `5`在第二个文件, `22`在第三个文件, 但对于以第二列为关键字的索引树, 它们却在同一个叶节点里, 所以, 为了解决这个问题, `InnoDB`的索引树映射的不再是文件地址, 而是主键关键字, 先找到主键关键字, 然后再去主键树里面找对应文件.

针对MySQL的其它内容, 我们这里不再多说, 本课的重点是数据结构, 数据库等到对应章节再细说, 更多的内容可以查看张洋的[MySQL索引背后的数据结构及算法原理](https://cloud.tencent.com/developer/article/1141500?shareByChannel=link), 原文找不到了, 这里贴一篇转载的.

# 完

