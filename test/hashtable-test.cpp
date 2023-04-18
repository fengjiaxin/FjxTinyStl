//
// Created by fengjiaxin on 2023/4/17.
//

#include "../MyTinyStl/hashtable.h"
#include <iostream>
#include "../MyTinyStl/hash_fun.h"
#include "../MyTinyStl/functional.h"

using namespace std;

int main() {
    mystl::hashtable<int,int,mystl::hash<int>, mystl::identity<int>, mystl::equal_to<int>> iht(50,mystl::hash<int>(),mystl::equal_to<int>());
    cout << iht.size() << endl;
    cout << iht.bucket_count() << endl;
    iht.insert_unique(59);
    iht.insert_unique(63);
    iht.insert_unique(108);
    iht.insert_unique(2);
    iht.insert_unique(53);
    iht.insert_unique( 55);
    cout << iht.size() << endl;
}