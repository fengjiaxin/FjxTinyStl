//
// Created by fengjiaxin on 2023/4/10.
// 双向链表, 主要完成下面几个方法
// push_back, push_front, pop_front, pop_back, insert, erase
//

#ifndef FJXTINYSTL_LIST_H
#define FJXTINYSTL_LIST_H

#include "iterator.h"
#include <stddef.h>
#include "allocator.h"

namespace mystl
{

template <class T>
struct __list_node {
    typedef __list_node<T>* pointer;
    pointer prev;
    pointer next;
    T data;
};


// 迭代器类
template <class T>
struct __list_iterator : public mystl::iterator<mystl::bidirectional_iterator_tag, T>{
    typedef __list_iterator<T>           self;

    // 5个基本类型，通过继承 mystl::iterator 的方式获取
    typedef T                                 value_type;
    typedef T*                                pointer;
    typedef T&                                reference;

    typedef __list_node<T>*              link_type;
    typedef size_t                       size_type;

    link_type node; // 迭代器内部有一个普通指针，指向list节点

    __list_iterator() {}
    __list_iterator(link_type x): node(x) {} // iterator接受一个指向node节点的指针进行初始化
    __list_iterator(const self& x) : node(x.node) {}

    bool operator==(const self& x) const { return node == x.node; }
    bool operator!=(const self& x) const { return node != x.node; }

    // 迭代器取值， 取的是节点的数据值
    reference operator*() const { return (*node).data; }

    // 迭代器成员取值->, 重载不消耗->, 还是返回指针类型
    pointer operator->() const { return &(this->operator*()); }

    // 前缀++, ++iterator形式
    self& operator++() {
        node = (link_type)((*node).next);
        return *this;
    }

    // 后缀++, iterator++
    self operator++(int) {
        self tmp = *this;
        ++*this; // 先遇到++, *this作为对象，++操作符重载
        return tmp;
    }

    self& operator--() {
        node = (link_type)((*node).prev);
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
    }
};

// 模板类: list
// 模板参数 T 代表数据类型
template <class T>
class list
{
public:
    // list 的嵌套类别定义
    typedef mystl::allocator<T>                       allocator_type;
    typedef mystl::allocator<T>                       data_allocator;
    typedef mystl::allocator<__list_node<T>>          node_allocator;

    typedef typename allocator_type::value_type       value_type;
    typedef typename allocator_type::pointer          pointer;
    typedef typename allocator_type::const_pointer    const_pointer;
    typedef typename allocator_type::reference        reference;
    typedef typename allocator_type::const_reference  const_reference;
    typedef typename allocator_type::size_type        size_type;
    typedef typename allocator_type::difference_type  difference_type;

    typedef __list_iterator<T>                        iterator;
    typedef __list_node<T>*                           link_type;

private:
    link_type node_ = nullptr; // 指向末尾节点

public:
    // 构造，复制，移动，析构函数
    list() {
        fill_initialize(0, value_type());
    }

    explicit list(size_type n) {
        fill_initialize(n, value_type());
    }

    list(size_type n, const T& value) {
        fill_initialize(n, value);
    }

    // 拷贝构造
    list(const list& ilist) = delete;
    // 移动构造
    list(const list&& ilist) = delete;
    // 拷贝赋值
    list& operator=(const list& ilist) = delete;
    // 移动赋值
    list& operator=(const list&& ilist) = delete;

    // 析构函数
    ~list() {
       if (node_ != nullptr) {
            clear();
            node_allocator::deallocate(node_);
            node_ = nullptr;
       }
    }

public:
    // 迭代器基本的操作方法， insert,erase等基本方法
    // 插入节点
    iterator insert(iterator position, const T& x) {
        link_type tmp = create_node(x);
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        position.node->prev->next = tmp;
        position.node->prev = tmp;
        return tmp;
    }

    void insert(iterator position, size_type n, const T& x) {
        for ( ; n > 0 ; --n) {
            insert(position, x);
        }
    }

    // 消除节点
    iterator erase(iterator position) {
        link_type next_node = position.node->next;
        link_type prev_node = position.node->prev;
        prev_node->next = next_node;
        next_node->prev = prev_node;
        destroy_node(position.node);
        return next_node;
    }

    void clear() {
        link_type cur = (link_type)node_->next;
        while (cur != node_) {
            link_type tmp = cur;
            cur = (link_type)cur->next;
            destroy_node(tmp);
        }
        node_->next = node_;
        node_->prev = node_;
    }

public:
    // 迭代器相关操作

    iterator begin() noexcept {
        return node_->next; // 将指向节点的指针初始化为iterator
    }
    iterator end() noexcept {
        return node_;
    }

    // 容器相关操作
    bool empty() const noexcept {
        return node_->next == node_;
    }
    typename mystl::iterator_traits<iterator>::difference_type
    size() noexcept {
        return mystl::distance<iterator>(begin(), end());
    }

    // 访问元素相关操作
    reference front() {
        return *begin(); // begin是迭代器，已经对*进行重载，指向的是元素的值
    }
    reference back() {
        return *(--end());
    }

    void push_front(const T& value) {
        insert(begin(), value);
    }

    void push_back(const T& value) {
        insert(end(), value);
    }

    void pop_front() {
        erase(begin());
    }

    void pop_back() {
        iterator tmp = end();
        erase(--tmp);
    }

private:
    // 获取/释放 内存  创建/构造对象
    link_type get_node() { // 获取内存指针
        return node_allocator::allocate();
    }
    void release_node(link_type p) { // 释放内存
        node_allocator::deallocate(p);
    }
    // 创建node节点，1.分配内存 2.在指定内存处构造对象
    link_type create_node(const T& value) {
        link_type p = get_node();
        try {
            data_allocator::construct(&(p->data),value);
            p->next = nullptr;
            p->prev = nullptr;
        } catch (...) {
            node_allocator::deallocate(p);
            throw;
        }
        return p;
    }
    // 销毁节点， 1.析构 2.释放内存
    void destroy_node(link_type p) {
        data_allocator::destroy(&(p->data));
        release_node(p);
    }

private:
    // 初始化 相关操作
    void empty_initialize() {
        node_ = get_node();
        node_->next = node_;
        node_->prev = node_;
    }

    void fill_initialize(size_type n, const value_type& value) {
        empty_initialize();
        try {
            insert(begin(), n, value);
        } catch(...) {
            clear();
            destroy_node(node_);
            node_ = nullptr;
            throw;
        }
    }
};

}

#endif //FJXTINYSTL_LIST_H
