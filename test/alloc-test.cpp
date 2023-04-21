//
// Created by fengjiaxin on 2023/4/20.
//
#include <iostream>
#include "../../FjxTinyStl/MyTinyStl/alloc.h"
#include <cstddef>

using namespace std;


void cookie_test(size_t n) {
    void *p1,*p2,*p3;
    p1 = mystl::alloc::allocate(n);
    p2 = mystl::alloc::allocate(n);
    p3 = mystl::alloc::allocate(n);
    cout << "p1= " << p1 << '\t' << "p2= " << p2 << '\t' << "p3= " << p3 << '\n';
    mystl::alloc::deallocate(p1, sizeof(n));
    mystl::alloc::deallocate(p2, sizeof(n));
    mystl::alloc::deallocate(p3, sizeof(n));
}

int main() {
    cout << sizeof(mystl::alloc) << endl;
    cookie_test(1);

    cout << "----------------------" << endl;

    return 0;



}



