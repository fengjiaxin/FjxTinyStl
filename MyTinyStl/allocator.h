//
// Created by fengjiaxin on 2023/4/10.
// allocator, 主要用于管理内存的分配，释放
//

#ifndef FJXTINYSTL_ALLOCATOR_H
#define FJXTINYSTL_ALLOCATOR_H

#include <stddef.h>
#include "construct.h"


namespace mystl
{

template <class T>
class allocator
{
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

public:
    static T*   allocate();
    static T*   allocate(size_type n);

    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type n);

    static void construct(T* ptr);
    static void construct(T* ptr, const T& value);

    static void destroy(T* ptr);

};

template<class T>
T* allocator<T>::allocate() {
    return static_cast<T*>(::operator new(sizeof(T)));
}

template<class T>
T* allocator<T>::allocate(size_type n) {
    if (n == 0)
         return nullptr;
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <class T>
void allocator<T>::deallocate(T *ptr) {
    if (ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T *ptr, size_type /*size*/) {
    if (ptr == nullptr)
        return;
    ::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T *ptr) {
    mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T *ptr, const T& value) {
    mystl::construct(ptr, value);
}


template <class T>
void allocator<T>::destroy(T *ptr) {
    mystl::destroy(ptr);
}


}

#endif //FJXTINYSTL_ALLOCATOR_H
