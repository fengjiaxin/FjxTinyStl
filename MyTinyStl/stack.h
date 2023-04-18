#ifndef FJXTINYSTL_STACK_H
#define FJXTINYSTL_STACK_H

//
// Created by fengjiaxin on 2023/4/18.
//

#include "deque.h"
namespace mystl
{

template <class T, class Sequence = mystl::deque<T>>
class stack {

public:
    typedef typename Sequence::value_type value_type;
    typedef typename Sequence::size_type size_type;
    typedef typename Sequence::reference reference;
    typedef typename Sequence::const_reference const_reference;

private:
    Sequence c;
public:
    bool empty() { return c.emtpy(); }
    size_type size() { return c.size(); }
    reference top() { return c.back(); }
    void push(const value_type& x) { c.push_back(x); }
    void pop() { c.pop_back(); }
};

}






#endif //FJXTINYSTL_STACK_H
