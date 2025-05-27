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

    int x;
    cin >> x;
    sl.erase(x);
    sl.print();
    return 0;
}