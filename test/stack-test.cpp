//
// Created by fengjiaxin on 2023/4/18.
//

#include <iostream>
#include "../MyTinyStl/stack.h"

using namespace std;
int main() {
    mystl::stack<int> istack;
    istack.push(1);
    istack.push(2);
    istack.push(3);
    istack.push(4);

    cout << istack.size() << endl;
    cout << istack.top() << endl;

    istack.pop(); cout << istack.top() << endl;
    istack.pop(); cout << istack.top() << endl;
    istack.pop(); cout << istack.top() << endl;
    cout << istack.size() << endl;
}
