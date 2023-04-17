//
// Created by fengjiaxin on 2023/4/11.
//

#include "../MyTinyStl/list.h"
#include "../MyTinyStl/algo.h"
#include <iostream>
using namespace std;

int main() {
    mystl::list<int> ilist;
    cout << "empty: " << ilist.empty() << endl;
    cout << "size = " << ilist.size() << endl;

    ilist.push_back(0);
    ilist.push_back(1);
    ilist.push_back(2);
    ilist.push_back(3);
    ilist.push_front(-1);
    ilist.push_front(-2);
    cout << "after push, size = " << ilist.size() << endl;

    mystl::list<int>::iterator iter;
    cout << "traverse : " ;
    for (iter = ilist.begin();  iter != ilist.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "back = " << ilist.back() << endl;
    cout << "front = " << ilist.front() << endl;
    cout << endl;

    ilist.pop_front();
    cout << "after pop front, front = "  << ilist.front() << endl;
    ilist.pop_back();
    cout << "after pop back, back = "  << ilist.back() << endl;

    cout << "traverse : " ;
    for (iter = ilist.begin();  iter != ilist.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "test insert" << endl;
    iter = mystl::find(ilist.begin(), ilist.end(), 1);
    if (*iter == 1) {
        ilist.insert(iter, 66);
    }
    cout << "traverse : " ;
    for (iter = ilist.begin();  iter != ilist.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;
}

