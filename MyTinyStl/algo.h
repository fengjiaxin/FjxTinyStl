#ifndef FJXTINYSTL_ALGO_H
#define FJXTINYSTL_ALGO_H

//
// Created by fengjiaxin on 2023/4/11.
// 这个头文件包含mystl的一些列算法
//

#include "iterator.h"

namespace mystl
{

// find
// 在[first,last) 区间内找到第一个等于value的元素，返回指向该元素的指针
template <class InputIter, class T>
InputIter find(InputIter first, InputIter last, const T& value) {
    while (first != last && *first != value)
        ++first;
    return first;
}

// lower_bound
// 前向迭代器寻找方式
template <class ForwardIter, class T, class Distance>
ForwardIter __lower_bound(ForwardIter first, ForwardIter last, const T& value, Distance*, mystl::forward_iterator_tag) {
    Distance len = mystl::distance(first, last);
    Distance half;
    ForwardIter mid;
    while (len > 0) {
        half = len >> 1;
        mid = first;
        mystl::advance(mid, half);
        if (*mid < value) {
            first = mid;
            ++first;
            len = len - half - 1;
        } else
            len = half;
    }
    return first;
}

// 随机访问迭代器寻找方式
template <class RandomAccessIter, class T, class Distance>
RandomAccessIter __lower_bound(RandomAccessIter first, RandomAccessIter last, const T& value, Distance*, mystl::random_access_iterator_tag) {
    Distance len = last - first;
    Distance half;
    RandomAccessIter mid;
    while (len > 0) {
        half = len >> 1;
        mid = first + half;
        if (*mid < value) {
            first = mid + 1;
            len = len - half - 1;
        } else
            len = half;
    }
    return first;
}

template <class ForwardIter, T>
ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value) {
    return __lower_bound(first, last, value, mystl::distance_type(first), mystl::iterator_category(first));
}


}


#endif //FJXTINYSTL_ALGO_H
