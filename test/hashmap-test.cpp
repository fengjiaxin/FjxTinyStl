//
// Created by fengjiaxin on 2023/4/18.
//

#include <iostream>
#include "../MyTinyStl/hash_map.h"
#include "../MyTinyStl/hash_fun.h"
#include <cstring>
using namespace std;

struct eqstr {
    bool operator()(const char* s1, const char* s2) const {
        return strcmp(s1,s2) == 0;
    }
};

int main() {
    mystl::hash_map<const char*, int, mystl::hash<const char*>, eqstr> days;

    days["a"] = 1;
    days["b"] = 2;
    days["c"] = 3;
    days["d"] = 4;
    days["e"] = 5;
    days["f"] = 6;
    days["g"] = 7;

    cout << "a ->" << days["a"] << endl;
    cout << "b ->" << days["b"] << endl;
    cout << "c ->" << days["c"] << endl;
    cout << "d ->" << days["d"] << endl;

    auto ite1 = days.begin();
    auto ite2 = days.end();
    for (; ite1 != ite2; ++ite1)
        cout << ite1->first << " -> " << ite1->second << endl;



}