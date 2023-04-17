#ifndef FJXTINYSTL_ALGOBASE_H
#define FJXTINYSTL_ALGOBASE_H

#include "iterator.h"
#include "util.h"
#include <type_traits>
#include <string>
//
// Created by fengjiaxin on 2023/4/11.
// 这个头文件 包含了 mystl 的基本算法(比较简单的)
//

namespace mystl
{

/*****************************************************************************************/
// max
// 取二者中的较大值，语义相等时保证返回第一个参数
/*****************************************************************************************/
template <class T>
const T& max(const T& first, const T& second) {
    return first < second ? second : first;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compate>
const T& max(const T& first, const T& second) {
    return comp(first ,second) ? second : first;
}

/*****************************************************************************************/
// min
// 取二者中的较小值，语义相等时保证返回第一个参数
/*****************************************************************************************/
template <class T>
const T& min(const T& lhs, const T& rhs)
{
    return rhs < lhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare comp)
{
    return comp(rhs, lhs) ? rhs : lhs;
}

/*****************************************************************************************/
// copy
// 把 [first, last)区间内的元素拷贝到 [result, result + (last - first))内
/*****************************************************************************************/

template <class RandomAccessIter, class OutputIter, class Distance>
inline OutputIter
__copy_d(RandomAccessIter first, RandomAccessIter last, OutputIter result, Distance*) {
    // 以n决定循环的执行次数
    for (Distance n = last - first; n > 0; --n,++result,++first)
        *result = *first; // 拷贝赋值
    return result;
}

// InputIterator 版本
template <class InputIter, class OutputIter>
inline OutputIter __copy(InputIter first, InputIter last, OutputIter result, input_iterator_tag) {
    for (; first != last; ++result,++first)
        *result = *first; // 拷贝赋值
    return result;
}

// randomAccessIterator版本
template <class RandomAccessIter, class OutputIter>
inline OutputIter __copy(RandomAccessIter first, RandomAccessIter last, OutputIter result, random_access_iterator_tag) {
    return mystl::__copy_d(first, last, result, mystl::distance_type(first));
}

// 以下版本适用于 "指针所指之对象具备 trivial assignment operator"
template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, std::true_type) {
    std::memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

// 以下版本适用于 "指针所指之对象具备 non-trivial assignment operator"
template <class T>
inline T* __copy_t(const T* first, const T* last, T* result, std::false_type) {
    // 原生指针 是一种 randomAccessIterator
    return mystl::__copy_d(first, last, result, (ptrdiff_t*)0);
}


// __copy_dispatch()函数， 有一个完全泛化版本和两个偏特化版本
template <class InputIter, class OutputIter>
struct __copy_dispatch
{
    OutputIter operator()(InputIter first, InputIter last, OutputIter result) {
        return mystl::__copy(first, last, result, iterator_category(first));
    }
};

// 偏特化版本(1), 两个参数都是T* 指针形式
template <class T>
struct __copy_dispatch<T*, T*>
{
    T* operator()(T* first, T* last, T* result) {
        return mystl::__copy_t(first, last, result, std::is_trivially_copy_assignable<T>{});
    }
};

// 偏特化版本(2), 第一个参数const T* 指针形式， 第二个参数 T* 指针形式
template <class T>
struct __copy_dispatch<const T*, T*>
{
    T* operator()(const T* first, const T* last, T* result) {
        return mystl::__copy_t(first, last, result, std::is_trivially_copy_assignable<T>{});
    }
};


// copy 函数， 完全泛化 特殊版本1，特殊版本2
// 完全泛化版本
template <class InputIter, class OutputIter>
inline OutputIter copy(InputIter first, InputIter last, OutputIter result) {
    return mystl::__copy_dispatch<InputIter,OutputIter>()(first, last, result);
}

// 特殊版本1
inline char* copy(const char* first, const char* last, char* result) {
    std::memmove(result, first, last - first);
    return result + (last - first);
}

// 特殊版本2
inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    std::memmove(result, first, sizeof(wchar_t ) * (last - first));
    return result + (last - first);
}



/*****************************************************************************************/
// move
// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
/*****************************************************************************************/

// 通过指针的方式移动
template <class RandomAccessIter, class OutputIter, class Distance>
inline OutputIter
__move_d(RandomAccessIter first, RandomAccessIter last, OutputIter result, Distance*) {
    // 以n决定循环的执行次数
    for (Distance n = last - first; n > 0; --n,++result,++first)
        *result = mystl::move(*first); // 移动赋值
    return result;
}

// InputIterator 版本
template <class InputIter, class OutputIter>
inline OutputIter __move(InputIter first, InputIter last, OutputIter result, input_iterator_tag) {
    for (; first != last; ++result,++first)
        *result = mystl::move(*first); // 移动赋值
    return result;
}

// randomAccessIterator版本
template <class RandomAccessIter, class OutputIter>
inline OutputIter __move(RandomAccessIter first, RandomAccessIter last, OutputIter result, random_access_iterator_tag) {
    return mystl::__move_d(first, last, result, mystl::distance_type(first));
}

// 以下版本适用于 "指针所指之对象具备 trivial assignment operator"
template <class T>
inline T* __move_t(const T* first, const T* last, T* result, std::true_type) {
    std::memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

// 以下版本适用于 "指针所指之对象具备 non-trivial assignment operator"
template <class T>
inline T* __move_t(const T* first, const T* last, T* result, std::false_type) {
    // 原生指针 是一种 randomAccessIterator
    return mystl::__move_d(first, last, result, (ptrdiff_t*)0);
}


// __move_dispatch()函数， 有一个完全泛化版本和两个偏特化版本
template <class InputIter, class OutputIter>
struct __move_dispatch
{
    OutputIter operator()(InputIter first, InputIter last, OutputIter result) {
        return mystl::__move(first, last, result, iterator_category(first));
    }
};

// 偏特化版本(1), 两个参数都是T* 指针形式
template <class T>
struct __move_dispatch<T*, T*>
{
    T* operator()(T* first, T* last, T* result) {
        return mystl::__move_t(first, last, result, std::is_trivially_move_assignable<T>{});
    }
};

// 偏特化版本(2), 第一个参数const T* 指针形式， 第二个参数 T* 指针形式
template <class T>
struct __move_dispatch<const T*, T*>
{
    T* operator()(const T* first, const T* last, T* result) {
        return mystl::__move_t(first, last, result, std::is_trivially_move_assignable<T>{});
    }
};


// move 函数， 完全泛化 特殊版本1，特殊版本2
// 完全泛化版本
template <class InputIter, class OutputIter>
inline OutputIter move(InputIter first, InputIter last, OutputIter result) {
    return mystl::__move_dispatch<InputIter,OutputIter>()(first, last, result);
}

// 特殊版本1
inline char* move(const char* first, const char* last, char* result) {
    std::memmove(result, first, last - first);
    return result + (last - first);
}

// 特殊版本2
inline wchar_t* move(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    std::memmove(result, first, sizeof(wchar_t ) * (last - first));
    return result + (last - first);
}



/*****************************************************************************************/
// fill
// 把 [first, last)区间内的元素改填新值
/*****************************************************************************************/
template <class ForwardIter, class T>
void fill(ForwardIter first, ForwardIter last, const T& value) {
    for (;first != last; ++first)
        *first = value;
}

/*****************************************************************************************/
// fill_n
// 把 [first, last)内的前n个元素改填新值，返回的迭代器指向被填入的最后一个元素的下一个位置
/*****************************************************************************************/
template <class OutputIter, class Size, class T>
OutputIter fill_n(OutputIter first, Size n, const T& value) {
    for (;n > 0; --n,++first)
        *first = value;
    return first;
}


/*****************************************************************************************/
// copy_backward
// 把 [first, last)区间内的元素拷贝到 [result - (last - first),result), 从后往前copy
/*****************************************************************************************/
template <class BidirectionalIter1, class BidirectionalIter2>
inline BidirectionalIter2 __copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 res) {
    while (first != last)
        *--res = *--last;
    return res;
}

template <class BidirectionalIter1, class BidirectionalIter2>
struct __copy_backward_dispatch
{
    BidirectionalIter2 operator()(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 res) {
        return mystl::__copy_backward(first, last, res);
    }
};

// 指针类型， 判断指针类型指向的对象是pod类型, 可以直接copy
template <class T>
T* __copy_backward_t(const T* first, const T* last, T* res, std::true_type) {
    const ptrdiff_t N = last - first;
    std::memmove(res - N, first, sizeof(T) * N);
    return res - N;
}

// 指针类型， 指向对象是non-pod类型
template <class T>
T* __copy_backward_t(const T* first, const T* last, T* res, std::false_type) {
    return mystl::__copy_backward(first, last, res);
}

template <class T>
struct __copy_backward_dispatch<T*, T*> {
    T* operator()(T* first, T* last, T* res) {
        return __copy_backward_t(first, last, res, std::is_trivially_copy_assignable<T>{});
    }
};

template <class T>
struct __copy_backward_dispatch<const T*, T*> {
    T* operator()(const T* first, const T* last, T* res) {
        return __copy_backward_t(first, last, res, std::is_trivially_copy_assignable<T>{});
    }
};

template <class BidirectionalIter1, class BidirectionalIter2>
inline BidirectionalIter2 copy_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 res) {
    return __copy_backward_dispatch<BidirectionalIter1, BidirectionalIter2>()(first, last, res);
}

}

#endif //FJXTINYSTL_ALGOBASE_H
