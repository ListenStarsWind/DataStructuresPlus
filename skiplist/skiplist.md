# skiplist  

## 引言

跳表, 是一种用于进行查找的数据结构, 它最早由威廉·普格(William Pugh)于1990年于论文 `Skip Lists: A Probabilistic Alternative to Balanced Trees  `中提出, 论文的下载链接[如下](https://ftp.cs.umd.edu/pub/skipLists/skiplists.pdf)

跳表, 是在有序链表的基础上发展而来的, 我们知道, 对于一个有序的链表来说, 进行数据查找的时间复杂度是$O(N)$.  威廉·普格为了对其进行优化, 最开始提出这样的设想. 

假如我们每相邻两个节点升高一层，增加一个指针，让指针指向下下个节点，如下图a所示。这样所有新增加的指针连成了一个新的链表，但它包含的节点个数只有原来的一半。由于新增加的指针，我们不再需要与链表中每个节点逐个进行比较了，需要比较的节点数大概只有原来的一半。

以此类推，我们可以在第二层新产生的链表上，继续为每相邻的两个节点升高一层，增加一个指针，从而产生第三层链表。如下图b这样搜索效率就进一步提高了.

![image-20250525205607150](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525205607213.png)

skiplist正是受这种多层链表的想法的启发而设计出来的。实际上，按照上面生成链表的方式，上面每一层链表的节点个数，是下面一层的节点个数的一半，这样查找过程就非常类似二分查找，使得查找的时间复杂度可以降低到$O(log_2N)$, 

不过这种机制引入了新的问题, 那就是对新数据的插入删除造成了困扰. 因为这种链表的结构太规整了, 所以如果要删除插入新的数据, 就要对各个层进行重新调整, 否则就会破坏原先的规则.

威廉·普格为了避免这种问题，做了一个大胆的处理，不再严格要求对应比例关系，而是插入一个节点的时候随机出一个层数。这样每次插入和删除都不需要考虑其他节点的层数，这样就好处理多了。细节过程入下图：  

![image-20250525210009992](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525210010034.png)

在上图中, 我们要找17这个关键字, 最开始, 我们从表头出发, 先使用最高层的链表, 发现下一个节点的值为6, 比17小, 所以来到6这个节点, 接下来, 发现下一个节点是空, 空说明这层链表已经没了, 所以我们就要来到下一层, 下一层的下一个节点是25, 比17大, 这说明, 17的位置应该在6和25这两个节点之间, 所以我们再次下降高度, 来到第三层的链表, 此时下一个节点是9, 比17小, 于是来到这里, 接下来, 下一个节点是25, 比17大, 所以来到第四层, 下一个节点是12, 比17小, 所以继续往后来到12, 12往后是19, 19比17大, 而又没有更低的链表了, 所以17应该插入到12这个链表的后面(对于底层链表来说).

然后, 他随机生成一个层数, 是2, 那么17这个节点的高度就是2, 最底层的前驱节点是12, 次底层的前驱节点就是9.

这个随机也不是乱随机的, 他遵循一定的概率的, 伪代码如下

![image-20250525212615307](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525212615343.png)

最开始层数就是1, 然后定义一个概率`p`, 描述, 向上再增加一层的概率, 就是说, 如果现在已经是一层了, 那么又`p`的概率变成两层, 也有`1-p`的概率就停在一层, 还有一个`MaxLevel`, 表示允许的最高高度.

随机一个深度, 输出一层的概率是`1-p`, 也就是在第一层就触发了`(1-p)`的这个不上升的概率
输出二层的概率是`p * (1-p)`, 首先, 你需要从一层上升到二层, 这是`p`的概率, 其次你停在了二层, 所以再乘上`1-p`
输出三层的概率是$p^2 * (1-p)$
输出`h`层的概率是$p^{h-1} * (1-p)$

跳表在一些数据库里面也会用, 不过这是和MySQL已经完全不同的数据库了, 比如MemSQL, 这是一个专门用于进行内存管理的数据库, 还有谷歌的LevelDB.也是一个数据库. 

![image-20250525214344689](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525214344737.png)

跳表的时间复杂度这里不推导了, 较为复杂, 只能说, 和搜索树的级别相同$O(log_2n)$, 具体可以看威廉·普格的论文, 或者是这篇[文章](http://zhangtielei.com/posts/blog-redis-skiplist.html)

## 简单实现

下面我们结合力扣的[1206. 设计跳表](https://leetcode.cn/problems/design-skiplist/)这题, 来简单实现一个跳表.

对于跳表来说, 其唯一一个关键接口就是定位一个节点在跳表中应该所处的位置, 对于`search`来说, 那就是这个位置找到之后, 看看他是不是那个关键字, `add`要先找到这个位置, 找到这个位置之后再去插入, `erase`也是如此, 你要先找到目标节点, 再去删除它

不过在写定位之前, 我们先要把大致框架搭一下.

```cpp
#include<vector>
#include<cstdlib>  // rand
#include<iomanip>  // std::setw
#include<climits>  // INT_MIN
#include<iostream>
#include <random>  // std::default_random_engine, std::uniform_real_distribution
#include <chrono>  // chrono


struct SkiplistNode
{
    int _val;
    std::vector<SkiplistNode*> _next;
    SkiplistNode() : _val(INT_MIN), _next(1){}
    SkiplistNode(int val, int level) : _val(val), _next(level){}
};

class Skiplist {
    typedef SkiplistNode Node;

public:
    Skiplist() {
        _head = new Node();
    }

    bool search(int target) {
        
    }

    void add(int num) {
       
    }

    bool erase(int num) {
        
    }

    void print() {
    
}

    private:
    Node* _head;
    double _p = 0.25;
    int _maxLevel = 32;
};
```

跳表是建立在链表的基础上的, 不管怎么说, 最起码要有一层的链表, 所以对于跳表节点的默认构造来说, 我们最开始把存储每一层下一个节点指针的`_next`数组大小设置为一, 考虑到跳表是有序链表, 对于起哨兵位作用的头结点来说, 其应该是存储数据类型的最小值, 尽管力扣说它的测试用例不会插入负数, 但这里我们还是设置为`INT_MIN`.

接下来我们来写最为关键的定位接口, 就当前的语境来说, 定位接口应该叫做`Locate`, 但之前写的时候, 当时没有多想, 随便给个`find`的名字, 下面, 我们就用`find`来称呼我们的定位函数.

由于跳表是多层的链表结构, 所以为了方便起见, 我们对于`find`的需求是这样的, 指定特定层, 返回节点应该所处位置的前驱节点.

![image-20250525210009992](https://md-wind.oss-cn-nanjing.aliyuncs.com/md/20250525210010034.png)

比如, 我们拿这张图来举例, 如果我要找第一层的`12`节点应该所处的位置, 那么在第一层我们返回`9`节点, 第二层我们返回`9`节点, 第三层返回`6`节点, 第四层返回`6`节点.    如果要找`17`的话, 第一层返回`12`节点, 第二层返回`9`节点, 第三层返回`6`节点, 第四层返回`6`节点, 这种设计的好处是便于我们之后进行插入删除操作时, 对其中涉及到的各层链表进行更新.

因为我们要返回多层的前驱节点, 所以我们的返回值定为`vector<Node*>`, 输入参数有两个, 一个是关键字`target`, 另一个是层数大小`level_size`, 这样的话, `v[0]`对应的就是第一层的前驱节点, `v[1]`对应的就是第二层的前驱节点....

刚开始, 我们先把第一层的前驱节点给找出来, 然后再去同时找多层.这实际上就是一个模拟的过程, 我们每到一个节点, 就查看当前节点当前层的下一个节点, 将下一个节点的值与`target`进行比较. 比如, 假设现在我们已经位于`6`节点, 在第三层, 那此时的下一个节点就是`25`, `25 > 17`, 这意味着, `17`的位置一定在`6, 25`这两个节点之间, 所以我们来到下一层, 即第二层, 在第二层, 我们发现下一个节点是`9`, `9 < 17`, 所以`17`在`9`的后面, 我们应该来到`9`这个节点继续比较.

当然, 我们的这些判断是基于下一个节点有效, 即不为空的情况下进行的. 如果下一层的节点为空, 我们该作何处理, 其实这和上面的那种情况一致, 都是来到下一层,   那什么时候停止查找, 跳出循环呢? 那就是在所有层的链表都已经被查找过的情况下, 即在第一层, 或者说`next[0]`这里, 你发现下面的节点还是比`target`大, 所以继续往下, 来到`next[-1]`, 或者说, 第零层,此时一方面, 所有层都找过了, 另一方面, 再往下找就数组下标越界了, 所以我们跳出循环.

这样, 我们就有如下的代码

```cpp
Node* find(int targer, int level_size = 1)
{
    Node* curr = _head;
    int level = level_size - 1;
    while(level >= 0)
    {
        Node* next = curr->_next[level];
        if(next == nullptr || next->_val > target)
        {
            --level;
        }
        else if(next->_val < target)
        {
            curr = next;
        }
        else
        {
            break;
        }
    }
    
    return curr;
}
```

当节点不存在时, 它将返回第一层的前驱节点, 当节点存在时, 注意, 它将返回高度最高的那个节点, 假设, 我们现在跳表里面有两个`17`, 一个`17`在前面, 是两层的, 另一个`17`在后面, 是四层的, 那么它不会返回前面的那个`17`, 而是返回后面那个`17`最高层的前驱节点.

上面的代码还存在一定的问题, 那就是, 在最开始, 头结点的层数只有一, 那`level`就有越界的可能, 所以我们还要在此之前, 还要判断一下, 如果`level_size`比`_head->_next.size()`都大, 那么就应该将`_next`修正大小.当然最开始你也可以把头结点的层扩大到最多的层数.

下面我们写同时应对多层的方法, 其实也很简单, 当层数下降时, 不就说明当前节点就是目标节点的前驱节点, 所以直接入数组即可.需要注意的是, 当存在多个目标节点时, 由于他们的层数是随机的, 可能并不唯一, 所以此时我们要以第一层的第一个目标节点为基准, 也就是说, 现在如果和下一个节点和`target`相等, 并不退出循环, 而是来到下一层, 确定每层链表的情况, 最终选择的节点都是第一层的第一个目标节点, 然后以这个节点的层数为依据进行相关层链表的更新, 因为第一层的第一个一定是整个跳表的第一个, 所以它的`next`会把后续的相同数值节点给覆盖掉, 从而确保数组里面的前驱节点对应的都是同一个节点.

```cpp
vector<Node*> find(int target, int level_size = 1)
{
    if(level_size > _head->_next.size())
    {
        _head->_next.resize(level_size);
    }

    Node* curr = _head;
    int level = level_size - 1;

    vector<Node*> prev(level_size);
    while(level >= 0)
    {
        Node* next = curr->_next[level];
        if(next == nullptr || next->_val >= target)
        {
            prev[level--] = curr;
        }
        else if(next->_val < target)
        {
            curr = next;
        }
    }

    return prev;
}
```

对于`search`来说, 如果连第一层都找不到, 那就是找不到.

```cpp
bool search(int target) {
    std::vector<Node*> prevV = find(target);
    Node* next = prevV[0]->_next[0];
    return next == nullptr ? false : next->_val == target;
}
```

对于`add`来说, 在确定新节点的层数之后, 我们对其中涉及到的层都进行更新.

```cpp
void add(int num) {
    int level_size = randomlevel();
    std::vector<Node*> prev = find(num, level_size);
    Node* newnode = new Node(num, level_size);

    for(int i = 0; i < level_size; ++i)
    {
        newnode->_next[i] = prev[i]->_next[i];
        prev[i]->_next[i] = newnode;
    }
}
```

对于`erase`来说, 我们一定要以第一层的目标节点为基准, 删除这个节点, 防止多个重复值节点之间因为层数不同而相互干扰, 

```cpp
bool erase(int num) {
    int level_size = _head->_next.size();
    std::vector<Node*> prev = find(num, level_size);
    Node* next = prev[0]->_next[0];
    if(next == nullptr || next->_val != num)
        return false;

    level_size = next->_next.size();
    for(int i = 0; i < level_size; ++i)
    {
        prev[i]->_next[i] = next->_next[i];
    }

    delete next;
    return true;
}
```

在`erase`中, 我们可以进行一个小优化, 当最上层的链表只剩下头结点时, 就删除这一层.

```cpp
bool erase(int num) {
    int level_size = _head->_next.size();
    std::vector<Node*> prev = find(num, level_size);
    Node* next = prev[0]->_next[0];
    if(next == nullptr || next->_val != num)
        return false;

    level_size = next->_next.size();
    for(int i = 0; i < level_size; ++i)
    {
        prev[i]->_next[i] = next->_next[i];
    }

    delete next;

    int level = _head->_next.size() - 1;
    while(level >= 0)
    {
        if(_head->_next[level] == nullptr)
            --level;
        else
            break;
    }

    _head->_next.resize(level+1);

    return true;
}
```

在`add`中, 我们使用了`randomlevel`来基于跳表的相关规则生成一个随机的层数, 用来决定新节点的层数. 对于`randomlevel`来说, 我们会介绍两种方法, 一是C的写法, 而是C++的写法.

在C中, 若要进行概率生成需要借助于`rand`, 随机数生成函数来实现. `rand`会在`0~RAND_MAX`区间, 随机, 或者说均等地返回其中的某一个数, 其中`RAND_MAX`是`rand`所能返回的最大值, 由于机会是均等的, 所以比如说, `rand`只能在`0~99`这个区间里生成随机数, 那么, 它所生成的数小于等于24是多大的概率? 我们可以看到, 小于等于`24`的数一共有`0~24`这`25`个数, 而在`0~99`这`100`个数里面, 每个数的返回概率均等, 那么它返回的值小于等于`24`的概率就是`25/100 = 0.25`, 受此启发.`rand() > RAND_MAX * _p`这个事件发生的概率就是`_P`, 于是便有(注意要在构造函数里面加种子)

```cpp
int randomlevel()
{
    int level = 1;
    while(rand() < RAND_MAX * _p && level <= _maxLevel)
    {
        ++level;
    }

    return level;
}
```

 下面为了便于观察, 我们写一个打印函数, 该函数不是我写的, 因为写起来确实很麻烦, 但又不能没有

```cpp
void print() {
    // 收集所有节点
    std::vector<SkiplistNode*> nodes;
    SkiplistNode* curr = _head->_next[0];
    while (curr) {
        nodes.push_back(curr);
        curr = curr->_next[0];
    }

    // 计算总层数
    int total_levels = _head->_next.size();

    // 打印每一层，从高到低
    for (int level = total_levels - 1; level >= 0; --level) {
        std::cout << "Level " << std::setw(2) << level + 1 << ": ";
        curr = _head->_next[level];
        int idx = 0;
        for (size_t i = 0; i < nodes.size(); ++i) {
            // 若当前层不存在某个值，填空格
            if (nodes[i]->_next.size() > level && curr == nodes[i]) {
                std::cout << std::setw(5) << nodes[i]->_val;
                curr = curr->_next[level];
            } else {
                std::cout << std::setw(5) << " ";
            }
        }
        std::cout << std::endl;
    }
}
```

```cpp
#include"skiplist.hpp"

#include<iostream>

using namespace std;

int main()
{
    cout << "hello skiplist"<<endl;
    int arr[] = { 1, 3, 5, 2, 7, 8, 10, 4, 4, 7, 2};
    Skiplist sl;
    int len = sizeof(arr) / sizeof(int);
    for(int i = 0; i < len; ++i)
    {
        sl.add(arr[i]);
    }

    sl.print();
    return 0;
}
```

```shell
[whisper@starry-sky build]$ ./skiplist 
hello skiplist
Level  2:               2                             7        10
Level  1:     1    2    2    3    4    4    5    7    7    8   10
[whisper@starry-sky build]$ ./skiplist 
hello skiplist
Level  4:                         4                              
Level  3:                         4                              
Level  2:     1                   4                              
Level  1:     1    2    2    3    4    4    5    7    7    8   10
[whisper@starry-sky build]$ ./skiplist 
hello skiplist
Level  3:                    3              5                    
Level  2:                    3              5                  10
Level  1:     1    2    2    3    4    4    5    7    7    8   10
[whisper@starry-sky build]$ 
```

下面我们说一下C++风格的`RandomLevel`, 在C++中, 要自己的接口实现概率, 但由于C++基于面向对象, 所以不建议写C++风格的.

```cpp
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
```

它也需要一个种子, 只不过这个种子被对象化了, 它们只需要初始化一次即可, 所以这里使用静态变量的方法. `uniform_real_distribution`可以在指定的区间里随机均等地生成一个数.

```cpp
int main()
{
    cout << "hello skiplist"<<endl;
    int arr[] = { 1, 3, 5, 2, 7, 8, 10, 4, 4, 7, 2};
    Skiplist sl;
    int len = sizeof(arr) / sizeof(int);
    for(int i = 0; i < len; ++i)
    {
        sl.add(arr[i]);
    }
    sl.print();

    int x;
    cin >> x;
    sl.erase(x);
    sl.print();
    return 0;
}
```

```shell
[whisper@starry-sky build]$ ./skiplist 
hello skiplist
Level  3:                    3                                   
Level  2:               2    3         4                       10
Level  1:     1    2    2    3    4    4    5    7    7    8   10
3
Level  2:               2         4                       10
Level  1:     1    2    2    4    4    5    7    7    8   10
```

## 结尾

skiplist跟平衡搜索树和哈希表的对比  

skiplist相比平衡搜索树(AVL树和红黑树)对比，都可以做到遍历数据有序，时间复杂度也差不多。skiplist的优势是：a、skiplist实现简单，容易控制。平衡树增删查改遍历都更复杂。b、skiplist的额外空间消耗更低。平衡树节点存储每个值有三叉链，平衡因子/颜色等消耗。skiplist中p=1/2时，每个节点所包含的平均指针数目为2；skiplist中p=1/4时，每个节点所包含的平均指针数目为1.33；  

kiplist相比哈希表而言，就没有那么大的优势了。相比而言a、哈希表平均时间复杂度是O(1)，比skiplist快。b、哈希表空间消耗略多一点。skiplist优势如下：a、遍历数据有序b、skiplist空间消耗略小一点，哈希表存在链接指针和表空间消耗。c、哈希表扩容有性能损耗。d、哈希表在极端场景下哈希冲突高，效率下降厉害，需要红黑树补足接力  

# 完

