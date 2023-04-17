//
// Created by fengjiaxin on 2023/4/13.
//


#include "../MyTinyStl/vector.h"
#include "../MyTinyStl/algo.h"
#include <iostream>
using namespace std;

int main() {
    mystl::vector<int> ivec;
    cout << boolalpha << "empty: " << ivec.empty() << endl;
    cout << "size = " << ivec.size() << endl;

    ivec.push_back(0);
    ivec.push_back(1);
    ivec.push_back(2);
    ivec.push_back(3);
    ivec.push_back(4);
    cout << "after push_back, size = " << ivec.size()
        << ", capacity = " << ivec.capacity() <<  endl;

    mystl::vector<int>::iterator iter;
    cout << "traverse : " ;
    for (iter = ivec.begin();  iter != ivec.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "back = " << ivec.back() << ", front = " << ivec.front() << endl;

    ivec.pop_back();
    cout << "after pop back, back = "  << ivec.back() << endl;

    cout << "traverse : " ;
    for (iter = ivec.begin();  iter != ivec.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "test insert" << endl;
    iter = mystl::find(ivec.begin(), ivec.end(), 1);
    if (*iter == 1) {
        ivec.insert(iter, 6, 66);
    }
    cout << "traverse : " ;
    for (iter = ivec.begin();  iter != ivec.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "test erase" << endl;
    iter = mystl::find(ivec.begin(), ivec.end(), 1);
    if (*iter == 1)
        ivec.erase(iter);
    cout << "traverse : " ;
    for (iter = ivec.begin();  iter != ivec.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "test reserve" << endl;
    cout << "before reserve ,size = " << ivec.size()
        << ",capacity = " << ivec.capacity() << endl;
    ivec.reserve(32);
    cout << "after reserve ,size = " << ivec.size()
         << ",capacity = " << ivec.capacity() << endl;
    cout << "traverse : " ;
    for (iter = ivec.begin();  iter != ivec.end() ; ++iter) {
        cout << *iter << ' ';
    }
    cout << endl;

    cout << "test clear" << endl;
    ivec.clear();
    cout << "after clear ,size = " << ivec.size()
         << ",capacity = " << ivec.capacity() << endl;

}

