#ifndef FJXTINYSTL_DEQUE_H
#define FJXTINYSTL_DEQUE_H

//
// Created by fengjiaxin on 2023/4/14.
// 双向开口
//

#include "iterator.h"
#include "construct.h"
#include "algobase.h"
#include "allocator.h"
#include "uninitialized.h"

namespace mystl
{

// deque map 初始化的大小
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template <class T>
struct deque_buf_size
{
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// deque的迭代器设计
template <class T, class Ref, class Ptr>
struct __deque_iterator : public iterator<mystl::random_access_iterator_tag, T>
{
    typedef __deque_iterator<T, T&, T*>                        iterator;
    typedef __deque_iterator<T, const T&, const T&>            const_iterator;
    typedef __deque_iterator                                   self;

    typedef T           value_type;
    typedef Ptr         pointer;
    typedef Ref         reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          value_pointer;
    typedef T**         map_pointer;

    static const size_type buffer_size = deque_buf_size<T>::value;

    // 迭代器所包含的成员函数 4个
    value_pointer cur; // 指向所在缓冲区的当前元素
    value_pointer first; // 指向所在缓冲区的头部
    value_pointer last; // 指向所在缓冲区的尾部
    map_pointer   node; // 指向缓冲区在控制台的位置

    // 构造函数
    __deque_iterator() noexcept
            :cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    __deque_iterator(value_pointer v, map_pointer n)
            :cur(v), first(*n), last(*n + buffer_size), node(n) {}

    // 拷贝构造
    __deque_iterator(const iterator& rhs)
            :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    __deque_iterator(const const_iterator& rhs)
            :cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    // 拷贝赋值
    self& operator=(const iterator& rhs) {
        if (this != &rhs) {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    // 转到另一个缓冲区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }

    // 重载运算符
    reference operator*() const { return *cur; }
    pointer   operator->() const {return cur;}

    difference_type operator-(const self& x) const {
        return static_cast<difference_type>(buffer_size * (node - x.node - 1)) + (cur - first) + (x.last - x.cur);
    }

    self& operator++() {
        ++cur;
        if (cur == last) { // 到达缓冲区结尾
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if (cur ==  first) { // 在缓冲区的头
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        if (offset >= 0 and offset < static_cast<difference_type>(buffer_size)) {
            cur += n; // 仍在当前缓冲区
        } else { // 跳到其他缓冲区
            difference_type node_offset = offset > 0
                    ? offset / static_cast<difference_type>(buffer_size)
                    : -static_cast<difference_type>((-offset - 1) / buffer_size) -1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) {
        return *this += -n;
    }

    self operator-(difference_type n) const {
        self tmp = *this;
        return *tmp -= n;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }

    // 重载比较操作符
    bool operator==(const self& rhs) const { return cur == rhs.cur; }
    bool operator< (const self& rhs) const
    { return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node); }
    bool operator!=(const self& rhs) const { return !(*this == rhs); }
    bool operator> (const self& rhs) const { return rhs < *this; }
    bool operator<=(const self& rhs) const { return !(rhs < *this); }
    bool operator>=(const self& rhs) const { return !(*this < rhs); }
};

// 模板类 deque
template <class T>
class deque
{
public:
    typedef mystl::allocator<T>       allocator_type;
    typedef mystl::allocator<T>       data_allocator;
    typedef mystl::allocator<T*>      map_allocator;

    typedef typename allocator_type::value_type      value_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;
    typedef pointer*                                 map_pointer;
    typedef const_pointer*                           const_map_pointer;

    typedef __deque_iterator<T, T&, T*>              iterator;
    typedef __deque_iterator<T, const T&, const T*>  const_iterator;

    static const size_type buffer_size = deque_buf_size<T>::value;

private:
    // 4个数据表现一个deque
    iterator  start; // 指向第一个节点
    iterator  finish; // 指向最后一个节点
    map_pointer map_; // 指向map的指针，map中的每个元素是一个指针，指向一个缓冲区
    size_type map_size; // map内指针的个数


public:
    // 基本的查询
    iterator begin() { return start; }
    iterator end() { return finish; }
    reference operator[](size_type n ) { return start[static_cast<difference_type>(n)]; } // 调用迭代器的operator[]，已经重载
    reference front() { return *start;}
    reference back() {
        iterator tmp = finish;
        --tmp;
        return *tmp;
    }
    size_type size() { return finish - start; }// 迭代器的operator-()已经重载
    bool empty() const {  return finish == start; } // = 已经重载

private:
    pointer allocate_node() { return data_allocator::allocate(buffer_size);}
    void deallocate_node(pointer ptr) {
        data_allocator::deallocate(ptr, buffer_size);
    }
    // helper function, construct/destruct
    void create_map_and_nodes(size_type n); // 负责产生并安排好deque结构
    void destroy_map_and_nodes(); // 销毁
    void fill_initialize(size_type n, const value_type& value);

public:
    // 构造，复制， 移动，析构
    deque() : start(), finish(), map_(nullptr), map_size(0) {
        create_map_and_nodes(0);
    }

    deque(size_type n, const value_type& x) : start(), finish(), map_(nullptr), map_size(0){
        fill_initialize(n, x);
    }

    explicit deque(size_type n) : start(), finish(), map_(nullptr), map_size(0) {
        fill_initialize(n, value_type());
    }
    // 拷贝构造
    deque(const deque& x) : start(), finish(), map_(nullptr), map_size(0) {
        create_map_and_nodes(x.size());
        try {
            mystl::uninitialized_copy(x.begin(), x.end(), start);
        } catch (...) {
            destroy_map_and_nodes();
            throw;
        }
    }
    // 移动构造
    deque(deque&& rhs) = delete;
    // 拷贝赋值
    deque& operator=(const deque& rhs) = delete;
    // 移动赋值
    deque& operator=(deque&& rhs) = delete;

    // 析构函数
    ~deque() {
        if (map_ != nullptr) {
            mystl::destroy(start, finish);
            destroy_map_and_nodes();
        }
    }

public:
    // push_*, pop_*
    void push_back(const value_type& t) {
        if (finish.cur != finish.last - 1) { // 最后缓冲区还有至少两个位置
            mystl::construct(finish.cur, t);
            ++finish.cur;
        } else { // 最后缓冲区 只有一个位置
            push_back_aux(t);
        }
    }

    void push_front(const value_type& t) {
        if (start.cur != start.first) { // 第一缓冲区有备用空间
            mystl::construct(start.cur - 1, t);
            --start.cur;
        } else { // 第一缓冲区已无备用空间
            push_front_aux(t);
        }
    }

    void pop_back() {
        if (finish.cur != finish.first) {
            // 最后缓冲区有一个(或更多)元素
            --finish.cur;
            mystl::destroy(finish.cur);
        } else {
            // 最后缓冲区没有任何元素
            pop_back_aux();
        }
    }

    void pop_front() {
        if (start.cur != start.last - 1) {
            // 第一缓冲区有至少两个元素空间
            mystl::destroy(start.cur);
            ++start.cur;
        } else {
            // 第一缓冲区只有一个元素空间
            pop_front_aux();
        }
    }

public: // erase相关
    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        difference_type index = pos - start;
        if (index < (size() >> 1)) {
            mystl::copy_backward(start, pos, next);
            pop_front();
        } else {
            mystl::copy(next, finish, pos);
            pop_back();
        }
        return start + index;
    }

    void erase(iterator first, iterator last);
    void clear();

    iterator insert_aux(iterator pos, const value_type& x);
    iterator insert(iterator pos, const value_type& x) {
        if (pos.cur == start.cur) { // 插入的是前方
            push_front(x);
            return start;
        } else if (pos.cur == finish.cur) { // 插入的是back
            push_back(x);
            iterator tmp = finish;
            --tmp;
            return tmp;
        } else {
            return insert_aux(pos, x);
        }

    }




private:
    // 最后缓冲区只剩一个备用元素空间时会被调用
    void push_back_aux(const value_type& x);
    // 第一个缓冲区没有任何备用元素空间时会被调用
    void push_front_aux(const value_type& x);
    void pop_back_aux();
    void pop_front_aux();
private:
    // helper function and internal push_*, pop_*
    void reallocate_map(size_type nodes_to_add, bool add_at_front);
    // 在后面加一个缓冲区
    void reserve_map_at_back(size_type nodes_to_add = 1) {
        if (nodes_to_add + 1 > map_size - (finish.node - map_))
            reallocate_map(nodes_to_add, false);
    } // 在前面加一个缓冲区
    void reserve_map_at_front(size_type nodes_to_add = 1) {
        if (nodes_to_add > start.node - map_)
            reallocate_map(nodes_to_add, true);
    }



};

template<class T>
void deque<T>::create_map_and_nodes(size_type n) {
    // 需要节点数
    size_type num_nodes = n / buffer_size + 1;
    // map 管理节点数， 最少8个，最多 需要节点数 + 2
    map_size = mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), num_nodes + 2);
    map_ = map_allocator::allocate(map_size);
    // 希望将节点放到中间位置
    map_pointer nstart = map_ + (map_size - num_nodes) / 2;
    map_pointer nfinish = nstart + num_nodes - 1;

    map_pointer cur;
    try {
        for (cur = nstart; cur <= nfinish; ++cur)
            *cur = allocate_node();
    } catch (...) {
        for (map_pointer n = cur; n < cur; ++n)
            deallocate_node(*n);
        map_allocator::deallocate(map_, map_size);
        throw;
    }
    start.set_node(nstart);
    finish.set_node(nfinish);
    start.cur = start.first;
    finish.cur = finish.first + n % buffer_size;
}

template <class T>
void deque<T>::destroy_map_and_nodes() {
    for (map_pointer cur = start.node; cur <= finish.node; ++cur)
        deallocate_node(*cur);
    map_allocator::deallocate(map_, map_size);
}

template <class T>
void deque<T>::fill_initialize(size_type n, const value_type &value) {
    create_map_and_nodes(n);
    map_pointer cur;
    try {
        for (cur = start.node; cur < finish.node; ++cur)
            mystl::uninitialized_fill(*cur, *cur + buffer_size, value);
        mystl::uninitialized_fill(finish.first, finish.cur, value);
    } catch (...) {
        for (map_pointer a = start.node; a < cur; ++a)
            mystl::destroy(*a, *a + buffer_size);
        destroy_map_and_nodes();
        throw;
    }
}

template<class T>
void deque<T>::push_back_aux(const value_type &x) {
    value_type x_copy = x;
    reserve_map_at_back();
    *(finish.node + 1) = allocate_node();
    try {
        mystl::construct(finish.cur,x_copy);
        finish.set_node(finish.node + 1);
        finish.cur = finish.first;
    } catch (...) {
        deallocate_node(*(finish.node + 1));
        throw;
    }
}

template <class T>
void deque<T>::push_front_aux(const value_type &x) {
    value_type x_copy = x;
    reserve_map_at_front();
    *(start.node - 1) = allocate_node();
    try {
        start.set_node(start.node - 1);
        start.cur = start.last - 1;
        mystl::construct(start.cur, x_copy);
    } catch (...) {
        start.set_node(start.node + 1);
        start.cur = start.first;
        deallocate_node(*(start.node - 1));
        throw;
    }
}

template <class T>
void deque<T>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
    size_type old_nodes_num = finish.node - start.node + 1;
    size_type new_nodes_num = old_nodes_num + nodes_to_add;
    map_pointer new_nstart;
    if (map_size > 2 * new_nodes_num) {
        new_nstart = map_ + (map_size - new_nodes_num) / 2 + (add_at_front ? nodes_to_add : 0);
        if (new_nstart < start.node)
            mystl::copy(start.node, finish.node + 1, new_nstart);
        else
            mystl::copy_backward(start.node, finish.node + 1, new_nstart);
    } else {
        size_type new_map_size = map_size + mystl::max(map_size, new_nodes_num) + 2;
        // 配置一块新空间
        map_pointer new_map = map_allocator::allocate(new_map_size);
        new_nstart = new_map + (new_map_size - new_nodes_num) / 2 + (add_at_front ? nodes_to_add : 0);
        // 把原来map内容拷贝过来
        mystl::copy(start.node,finish.node + 1, new_nstart);
        map_allocator::deallocate(map_, map_size);
        map_ = new_map;
        map_size = new_map_size;
    }
    // 重新设定迭代器
    start.set_node(new_nstart);
    finish.set_node(new_nstart + old_nodes_num - 1);
}

template <class T>
void deque<T>::pop_back_aux() {
    // 释放最后一个分区
    deallocate_node(finish.first);
    finish.set_node(finish.node - 1);
    finish.cur = finish.last - 1;
    mystl::destroy(finish.cur);
}

template <class T>
void deque<T>::pop_front_aux() {
    mystl::destroy(start.cur);
    deallocate_node(start.first);
    start.set_node(start.node + 1);
    start.cur = start.first;
}

template <class T>
void deque<T>::clear() {
    // 以下针对头尾以外的每一个缓冲区(一定是饱满的)
    for (map_pointer x = start.node + 1; x < finish.node; ++x) {
        mystl::destroy(*x, *x + buffer_size);
        data_allocator::deallocate(*x, buffer_size);
    }
    if (start.node != finish.node) { // 头 尾 不在一个分区
        mystl::destroy(start.cur, start.last);
        mystl::destroy(finish.first, finish.cur);
        // 以下释放尾部缓冲区，头缓冲区保留
        data_allocator::deallocate(finish.first, buffer_size);
    } else {
        mystl::destroy(start.cur,finish.cur);
        // 注意不释放空间
    }
    finish = start;

}

template <class T>
void deque<T>::erase(iterator first, iterator last) {
    if (first == start && last == finish) {
        // 清除 整个区间
        clear();
        return finish;
    } else {
        difference_type n = last - first; // 清除区间的长度
        difference_type elems_before = first - start; // 清除区间前方的元素个数
        if (elems_before < (size() - n) /2 ) {
            // 前方元素少，向后copy
            mystl::copy_backward(start, first, last);
            iterator new_start = start + n;
            // 释放缓冲区
            for (map_pointer x = start.node; x < new_start.node; ++x)
                data_allocator::deallocate(*x, buffer_size);
            start = new_start;
        } else { // 后方元素少
            mystl::copy(last,first,first);
            iterator new_finish = finish - n;
            mystl::destroy(new_finish, finish);
            for (map_pointer x = new_finish.node + 1; x <= finish.node; ++ x)
                data_allocator::deallocate(*x, buffer_size);
            finish = new_finish;
        }
        return start + elems_before;
    }
}

template <class T>
typename deque<T>::iterator
deque<T>::insert_aux(iterator pos, const value_type &x) {
    difference_type index = pos - start;// 插入点之前的元素个数
    value_type x_copy = x;
    if (index < size() / 2) {
        // 移动前方的元素
        push_front(front());
        iterator front1 = start;
        ++front1;
        iterator front2 = front1;
        ++front2;
        pos = start + index;
        iterator pos1 = pos;
        ++pos1;
        mystl::copy(front2, pos1, front1);
    } else {
        push_back(back());
        iterator back1 = finish;
        --back1;
        iterator back2 = back1;
        --back2;
        pos = start + index;
        mystl::copy_backward(pos, back2, back1);
    }
    *pos = x_copy;
    return pos;
}

}


#endif
