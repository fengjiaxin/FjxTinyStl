#ifndef FJXTINYSTL_FUNCTIONAL_H
#define FJXTINYSTL_FUNCTIONAL_H
//
// Created by fengjiaxin on 2023/4/17.
// 函数对象

namespace mystl
{


// 定义一元函数的参数型别和返回值型别
template <class Arg, class Result>
struct unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

// 定义二元函数的参数型别和返回值型别
template <class Arg1, class Arg2, class Result>
struct binary_function
{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

// 函数对象 ： 加法
template <class T>
struct plus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x + y; }
};

// 函数对象 ： 减法
template <class T>
struct minus: public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { return x - y; }
};

// 不会改变元素，返回本身
template <class T>
struct identity : public unary_function<T, bool> {
    const T& operator()(const T& x) const { return x; }
};

template <class T>
struct equal_to : public binary_function<T, T, bool>
{
    bool operator()(const T& x, const T& y) const { return x == y; }
};

// 选择函数， 接受一个pair, 返回第一个元素
template <class Pair>
struct selectfirst : public unary_function<Pair, typename Pair::first_type>
{
    const typename Pair::first_type& operator()(const Pair& x) const {
        return x.first;
    }
};

}




#endif //FJXTINYSTL_FUNCTIONAL_H
