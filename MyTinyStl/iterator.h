#ifndef FJXTINYSTL_ITERATOR_H
#define FJXTINYSTL_ITERATOR_H

//
// Created by fengjiaxin on 2023/4/10.
// iterator 相关类别， 以及iterator_traits 萃取迭代器型别
//

#include <stddef.h>


namespace mystl
{

// 5种迭代器类型
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


// iterator 模板, 必须定义如下5种类型
template<class Category, class T, class Distance = ptrdiff_t,
        class Pointer = T*, class Reference = T&>
struct iterator {
    typedef Category   iterator_category;
    typedef T          value_type;
    typedef Pointer    pointer;
    typedef Reference  reference;
    typedef Distance   difference_type;
};

// iterator_traits 负责萃取迭代器的特性
// 萃取机, 如果iterator是class
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category   iterator_category;
    typedef typename Iterator::value_type          value_type;
    typedef typename Iterator::difference_type     difference_type;
    typedef typename Iterator::pointer             pointer;
    typedef typename Iterator::reference           reference;
};

// 针对原生指针而设计的traits偏特化版本
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag      iterator_category;
    typedef T                               value_type;
    typedef ptrdiff_t                       difference_type;
    typedef T*                              pointer;
    typedef T&                              reference;
};

// 针对原生pointer to const 指针而设计的traits偏特化版本
template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag      iterator_category;
    typedef T                               value_type;
    typedef ptrdiff_t                       difference_type;
    typedef T*                              pointer;
    typedef T&                              reference;
};

// 这个函数可以很方便的获取某个迭代器的类型
template <class Iterator>
inline typename mystl::iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    typedef typename mystl::iterator_traits<Iterator>::iterator_category category;
    return category();
}

// 萃取迭代器的distance_type, 通过指针的方式
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取迭代器的 value_type, 通过指针的方式
template <class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// advance, 指针iterator 前进n步, 函数重载
// 单向指针
template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
    while (n--) ++i;
}

// 双向指针
template <class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
    if (n >= 0)
        while (n--) ++i;
    else
        while (n++) --i;
}

// 随机访问指针
template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
    i += n;
}

template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) {
    mystl::__advance(i, n, iterator_category(i));
}

// distance, 计算两个指针的距离
template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
__distance(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <class RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
    return last - first;
}

template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type
distance(Iterator first, Iterator last) {
    typedef typename iterator_traits<Iterator>::iterator_category category;
    return mystl::__distance(first, last, category());
}


/*****************************************************************************************/
// reverse iterator, 将迭代器的移动行为倒转
/*****************************************************************************************/
template <class Iterator>
class reverse_iterator {
private:
    Iterator current; // 记录对应的正向迭代器
public:
    // 反向迭代器的五种相应型别
    typedef typename mystl::iterator_traits<Iterator>::iterator_category  iterator_category;
    typedef typename mystl::iterator_traits<Iterator>::value_type         value_type;
    typedef typename mystl::iterator_traits<Iterator>::difference_type    difference_type;
    typedef typename mystl::iterator_traits<Iterator>::pointer            pointer;
    typedef typename mystl::iterator_traits<Iterator>::reference          reference;

    typedef Iterator                                                      iterator_type;
    typedef reverse_iterator<Iterator>                                    self;

public:
    // 构造函数
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type i): current(i) {}
    reverse_iterator(const self& rhs) : current(rhs.current) {}

public:
    // 取出对应的正向迭代器
    iterator_type base() const {
        return current;
    }

    // 重载操作符
    reference operator*() const {
        // 实际对应正向迭代器的前一个位置
        iterator_type tmp = current;
        return *--tmp;
    }

    pointer operator->() const {
        return &(operator*());
    }

    // ++ 变 --
    self& operator++() {
        --current;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        --current;
        return tmp;
    }

    // -- 变 ++
    self& operator--() {
        ++current;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self& operator+=(difference_type n)
    {
        current -= n;
        return *this;
    }
    self operator+(difference_type n) const
    {
        return self(current - n);
    }
    self& operator-=(difference_type n)
    {
        current += n;
        return *this;
    }
    self operator-(difference_type n) const
    {
        return self(current + n);
    }

    // 这个需要再听下，没学明白, 遇到第一个*，已经操作符重载，将(*this + n) 传入
    // +运算符也重载
    reference operator[](difference_type n) const
    {
        return *(*this + n);
    }
};




}

#endif //FJXTINYSTL_ITERATOR_H
