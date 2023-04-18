//
// Created by fengjiaxin on 2023/4/18.
//

#include "../MyTinyStl/queue.h"
#include <iostream>

using namespace std;

int main() {
    mystl::queue<int> iqueue;
    iqueue.push(1);
    iqueue.push(2);
    iqueue.push(3);
    iqueue.push(4);
    iqueue.push(5);

    cout << iqueue.size() << endl;
    cout << iqueue.front() << endl;

    iqueue.pop(); cout << iqueue.front() << endl;
    iqueue.pop(); cout << iqueue.front() << endl;
    iqueue.pop(); cout << iqueue.front() << endl;

    cout << iqueue.size() << endl;


}

