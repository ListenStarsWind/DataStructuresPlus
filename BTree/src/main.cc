#include"BTree.hpp"

#include<iostream>
#include<vector>

using namespace std;

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