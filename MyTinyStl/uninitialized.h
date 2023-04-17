#ifndef FJXTINYSTL_UNINITIALIZED_H
#define FJXTINYSTL_UNINITIALIZED_H

//
// Created by fengjiaxin on 2023/4/11.
// 定义全局函数， 作用于未初始化空间上
// 1. uninitialized_copy()
// 2. uninitialized_fill()
// 3. uninitialized_fill_n()
// 分别对应于高层次函数 copy(), fill(), fill_n()


#include "iterator.h"
#include "algobase.h"
#include "construct.h"

namespace mystl {

/*****************************************************************************************/
// uninitialized_fill_n
// 如果[first, first + n)范围内的每一个迭代器都指向未初始化的内存， 会在每个迭代器的位置调用construct(), 产生x的复制品
/*****************************************************************************************/

// pod, plain old data,标量型对象 或 传统的C struct类型， POP 型别必然拥有 trivial ctr/dtr/copy/assignment 函数
// 可以对pod 型别采用最有效率的初值填写方法， 而对non-pod 型别采取最保险的做法

// 针对pod型别
    template<class ForwardIter, class Size, class T>
    inline ForwardIter
    __uninitialized_fill_n_aux(ForwardIter first, Size n, const T &value, std::true_type) {
        return mystl::fill_n(first, n, value);
    }

// 针对non-pod型别， 需要调用construct
    template<class ForwardIter, class Size, class T>
    ForwardIter
    __uninitialized_fill_n_aux(ForwardIter first, Size n, const T &value, std::false_type) {
        ForwardIter cur = first;
        try {
            for (; n > 0; --n, ++cur)
                mystl::construct(&*cur, value);
        } catch (...) {
            mystl::destroy(first, cur);
        }
        return cur;
    }


    template<class ForwardIter, class Size, class T>
    inline ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T &x) {
        typedef typename mystl::iterator_traits<ForwardIter>::value_type value_type;
        return mystl::__uninitialized_fill_n_aux(first, n, x, std::is_trivially_copy_assignable<value_type>{});
    }

/*****************************************************************************************/
// uninitialized_copy
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/

// 针对pod 型别
    template<class InputIter, class ForwardIter>
    inline ForwardIter
    __uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, std::true_type) {
        return mystl::copy(first, last, result);
    }

// 针对non-pod型别
    template<class InputIter, class ForwardIter>
    ForwardIter __uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter result, std::false_type) {
        ForwardIter cur = result;
        try {
            for (; first != last; ++first, ++cur)
                mystl::construct(&*cur, *first);
        } catch (...) {
            mystl::destroy(result, cur);
        }
    }

    template<class InputIter, class ForwardIter>
    inline ForwardIter
    __uninitialized_copy(InputIter first, InputIter last, ForwardIter res) {
        typedef typename mystl::iterator_traits<ForwardIter>::value_type value_type;
        return mystl::__uninitialized_copy_aux(first, last, res, std::is_trivially_copy_assignable<value_type>{});
    }

    template<class InputIter, class ForwardIter>
    inline ForwardIter
    uninitialized_copy(InputIter first, InputIter last, ForwardIter res) {
        return mystl::__uninitialized_copy(first, last, res);
    }

// 针对char*, wchar_t* 两种型别， 特化版本
    inline char *uninitialized_copy(const char *first, const char *last, char *res) {
        std::memmove(res, first, last - first);
        return res + (last - first);
    }

    inline wchar_t *uninitialized_copy(const wchar_t *first, const wchar_t *last, wchar_t *res) {
        std::memmove(res, first, sizeof(wchar_t) * (last - first));
        return res + (last - first);
    }


/*****************************************************************************************/
// uninitialized_move
// 把 [first, last) 上的内容移动到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/

// 针对pod 型别
    template<class InputIter, class ForwardIter>
    inline ForwardIter
    __uninitialized_move_aux(InputIter first, InputIter last, ForwardIter result, std::true_type) {
        return mystl::move(first, last, result);
    }

// 针对non-pod型别
    template<class InputIter, class ForwardIter>
    ForwardIter __uninitialized_move_aux(InputIter first, InputIter last, ForwardIter result, std::false_type) {
        ForwardIter cur = result;
        try {
            for (; first != last; ++first, ++cur)
                mystl::construct(&*cur, mystl::move(*first));
        } catch (...) {
            mystl::destroy(result, cur);
        }
    }

    template<class InputIter, class ForwardIter>
    inline ForwardIter
    __uninitialized_move(InputIter first, InputIter last, ForwardIter res) {
        typedef typename mystl::iterator_traits<ForwardIter>::value_type value_type;
        return mystl::__uninitialized_copy_aux(first, last, res, std::is_trivially_move_assignable<value_type>{});
    }

    template<class InputIter, class ForwardIter>
    inline ForwardIter
    uninitialized_move(InputIter first, InputIter last, ForwardIter res) {
        return mystl::__uninitialized_move(first, last, res);
    }

// 针对char*, wchar_t* 两种型别， 特化版本
    inline char *uninitialized_move(const char *first, const char *last, char *res) {
        std::memmove(res, first, last - first);
        return res + (last - first);
    }

    inline wchar_t *uninitialized_move(const wchar_t *first, const wchar_t *last, wchar_t *res) {
        std::memmove(res, first, sizeof(wchar_t) * (last - first));
        return res + (last - first);
    }


/*****************************************************************************************/
// uninitialized_fill
// 把 [first, last) 上的内容复制到以 result 为起始处的空间，返回复制结束的位置
/*****************************************************************************************/

// pod型别
    template<class ForwardIter, class T>
    inline void __uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T &x, std::true_type) {
        mystl::fill(first, last, x);
    }

// non-pod型别
    template<class ForwardIter, class T>
    void __uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T &x, std::false_type) {
        ForwardIter cur = first;
        try {
            for (; cur != last; ++cur)
                mystl::construct(&*cur, x);
        } catch (...) {
            mystl::destroy(first, cur);
        }
    }

// 加个步骤， 判断iter指向的对象的型别
    template<class ForwardIter, class T>
    void uninitialized_fill(ForwardIter first, ForwardIter last, const T &x) {
        typedef typename mystl::iterator_traits<ForwardIter>::value_type value_type;
        __uninitialized_fill_n_aux(first, last, std::is_trivially_copy_assignable<value_type>{});
    }
}


#endif //FJXTINYSTL_UNINITIALIZED_H
