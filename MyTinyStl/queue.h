#ifndef FJXTINYSTL_QUEUE_H
#define FJXTINYSTL_QUEUE_H

//
// Created by fengjiaxin on 2023/4/18.
// queue

#include "stack.h"

namespace mystl
{

template<class T, class Sequence = mystl::deque<T>>
class queue {
public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

private:
    Sequence c;
public:
    bool empty() { return c.empty(); }
    size_type size() { return c.size(); }
    reference front() { return c.front(); }
    reference back() { return c.back(); }
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_front(); }

};



}




#endif //FJXTINYSTL_QUEUE_H
