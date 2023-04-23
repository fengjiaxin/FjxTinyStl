#ifndef FJXTINYSTL_CONSTRUCT_H
#define FJXTINYSTL_CONSTRUCT_H

//
// Created by fengjiaxin on 2023/4/10.
// 这个头文件包含两个函数 construct, destroy
// construct : 负责对象的构造
// destroy : 负责对象的析构
//

#include <new>
#include <type_traits>
#include "iterator.h"
#include "util.h"

namespace mystl
{

// construct 构造对象
template <class Ty>
void construct(Ty* ptr) {
    // placement new, 在已经分配内存的地方构造对象
    new (ptr) Ty();
}

template <class Ty1, class Ty2>
void construct(Ty1* ptr, const Ty2& value) {
    new (ptr) Ty1(value);
}

// 析构函数,其实可以分为成员函数是否有指针，有指针一定要调用析构函数释放
// 1.如果类的的析构函数是trivial(不重要的)，不需要调用析构函数
// 2.如果类的析构函数是non-trivial(重要的)， 需要调用析构函数

template <class Ty, class... Args>
void construct(Ty* ptr, Args&&... args)
{
    ::new ((void*)ptr) Ty(mystl::forward<Args>(args)...);
}

template <class Ty>
void destroy(Ty* ptr) {
    if (ptr != nullptr) {
        ptr->~Ty();
    }
}

template <class ForwardIter>
inline void __destroy_aux(ForwardIter first, ForwardIter last, std::false_type) {
    for (;first != last; ++first)
        mystl::destroy(&*first);
}

template <class ForwardIter>
inline void __destroy_aux(ForwardIter, ForwardIter, std::true_type) {}

template <class ForwardIter>
inline void destroy(ForwardIter first, ForwardIter last) {
    typedef typename mystl::iterator_traits<ForwardIter>::value_type  value_type;
    __destroy_aux(first, last, std::is_trivially_destructible<value_type>{});
}

inline void destroy(char*, char*){}
inline void destroy(wchar_t*, wchar_t* ) {}




}

#endif //FJXTINYSTL_CONSTRUCT_H
