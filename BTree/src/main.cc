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