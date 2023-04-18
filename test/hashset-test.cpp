//
// Created by fengjiaxin on 2023/4/18.
//

#include "../MyTinyStl/hash_set.h"
#include "../MyTinyStl/hash_fun.h"
#include <iostream>

using namespace std;
int main() {
    mystl::hash_set<int> set;
    set.insert(59);
    set.insert(63);
    set.insert(108);
    set.insert(2);
    set.insert(53);
    set.insert(55);

    mystl::hash_set<int>::iterator ite1 = set.begin();
    mystl::hash_set<int>::iterator ite2 = set.end();
    for (; ite1 != ite2; ++ite1)
        cout << *ite1 << ' ';
    cout << endl;
}