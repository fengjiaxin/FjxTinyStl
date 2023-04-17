#ifndef FJXTINYSTL_UTIL_H
#define FJXTINYSTL_UTIL_H
//
// Created by fengjiaxin on 2023/4/10.
// 这个文件包含一些通用工具， move, swap

#include <type_traits>
namespace mystl
{

// move
// std::remove_reference<T>
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

template <class T>
void swap(T& lhs, T& rhs) {
    T tmp(mystl::move(lhs));
    lhs = mystl::move(rhs);
    rhs = mystl::move(tmp);

}

// forward
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<T&&>(arg);
}

}

#endif //FJXTINYSTL_UTIL_H
