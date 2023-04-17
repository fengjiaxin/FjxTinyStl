//
// Created by fengjiaxin on 2023/4/11.
// vector, 通过数组的方式，可以扩容, vector<bool> 不被允许， bool只占用1个bit, 没有指向单个bit的指针，vector的迭代器是指针
//

#ifndef FJXTINYSTL_VECTOR_H
#define FJXTINYSTL_VECTOR_H

#include <type_traits>
#include "allocator.h"
#include "iterator.h"
#include "algobase.h"
#include "uninitialized.h"
#include <memory>

namespace mystl
{

template<class T>
class vector{
public:
    // vector 的嵌套型别定义
    typedef mystl::allocator<T>                      allocator_type;
    typedef mystl::allocator<T>                      data_allocator;

    typedef typename allocator_type::value_type      value_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    // 迭代器是指针
    typedef value_type*                                   iterator;
    typedef const value_type*                             const_iterator;

private:
    iterator  start ; // 表示目前使用空间的头部
    iterator  finish ; // 目前使用空间的尾部
    iterator  end_of_storage; // 目前可用空间的尾部

private:
    void insert_aux(iterator position, const T& x) {
        if (finish != end_of_storage) {
            mystl::construct(finish, *(finish-1));
            ++finish;
            T x_copy = x ; // 防止x是vector上已有的元素， copy_backward可能会覆盖
            mystl::copy_backward(position, finish - 2, finish - 1);
            *position = x_copy;
        } else {
            const size_type old_size = size();
            const size_type new_size = old_size != 0 ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = mystl::uninitialized_copy(start, position, new_start);
                mystl::construct(new_finish, x);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(position, finish, new_finish);
            } catch(...) {
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw ;
            }
            // 析构并释放原来的vector
            mystl::destroy(begin(), end());
            deallocate();
            // 调整迭代器，指向新vector
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    };

    void deallocate() {
        if (start) data_allocator::deallocate(start, end_of_storage - start);
    }

    // 配置后填充
    iterator allocate_and_fill(size_type n, const T& x) {
        iterator res = data_allocator::allocate(n);
        mystl::uninitialized_fill_n(res, n, x);
        return res;
    }

    void fill_initialize(size_type n, const T& x) {
        start = allocate_and_fill(n, x);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    // 迭代器相关操作
    iterator begin() { return start;}
    iterator end() {return finish; }

    // 容量相关操作
    size_type size() const { return static_cast<size_type>(finish - start);}
    size_type capacity() const { return static_cast<size_type>(end_of_storage - start);}
    bool empty() { return begin() == end() ;}
    void reserve(size_type n); // 分配相关内存操作
    // swap
    void swap(vector& rhs) noexcept {
        if (this != &rhs) {
            mystl::swap(start, rhs.start);
            mystl::swap(finish, rhs.finish);
            mystl::swap(end_of_storage, rhs.end_of_storage);
        }
    }

    // 访问元素相关操作
    reference operator[](size_type n) { return *(begin() + n);}
    reference front() { return *begin(); }
    reference back() { return *(end() - 1);}

public:
    // 构造， 拷贝构造/赋值， 移动构造/赋值， 析构
    vector(): start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
    vector(size_type n, const T& x) { fill_initialize(n, x);}
    explicit vector(size_type n) { fill_initialize(n, T());}

    vector(const vector& v) = delete;
    vector(const vector&& v) = delete;
    vector& operator=(const vector& v) = delete;
    vector& operator=(const vector&& v) = delete;

    ~vector() {
        mystl::destroy(start, finish);
        deallocate();
    }

public:
    // 一些插入删除等操作
    void push_back(const T& x) {
        if (finish != end_of_storage) {
            mystl::construct(finish, x);
            ++finish;
        } else {
            insert_aux(end(), x);
        }
    }

    void pop_back() {
        --finish;
        mystl::destroy(finish);
    }

    iterator erase(iterator pos) {
        if (pos + 1 != end())
            mystl::copy(pos + 1, finish, pos); // 后续元素往前移动
        --finish;
        mystl::destroy(finish);
        return pos;
    }

    iterator erase(iterator first, iterator last) {
        iterator i = mystl::copy(last, finish, first);
        mystl::destroy(i,finish);
        finish = finish - (last - first);
        return first;
    }

    void clear() { erase(begin(), end());}

    void insert(iterator pos, size_type n, const T& x) {
        if (n == 0) return;
        if (size_type(end_of_storage - finish) >= n) { // 剩余空间足够
            T x_copy = x;
            // 计算插入点之后的现有元素个数
            const size_type elems_after = finish - pos;
            iterator old_finish = finish;
            if (elems_after > n) {
                // 插入点之后的现有元素个数 > 新增元素个数
                mystl::uninitialized_copy(finish - n,finish, finish);
                finish += n;
                mystl::copy_backward(pos, old_finish -n, old_finish);
                mystl::fill(pos, pos + n , x_copy);
            } else {
                // 插入点之后的现有元素个数 <= 新增元素个数
                mystl::uninitialized_fill_n(finish, n - elems_after, x_copy);
                finish += n - elems_after;
                mystl::uninitialized_copy(pos, old_finish, finish);
                finish += elems_after;
                mystl::fill(pos, old_finish, x_copy);
            }
        } else { // 剩余空间不够
            const size_type old_size = size();
            size_type new_size = old_size;
            while (new_size <= old_size + n)
                new_size *= 2;
            // 配置新空间
            iterator new_start = data_allocator::allocate(new_size);
            iterator new_finish = new_start;
            try {
                new_finish = mystl::uninitialized_copy(start, pos, new_start);
                new_finish = mystl::uninitialized_fill_n(new_finish, n, x);
                new_finish = mystl::uninitialized_copy(pos, finish, new_finish);
            } catch (...) {
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, new_size);
                throw;
            }
            // 销毁原来的空间
            mystl::destroy(start, finish);
            deallocate();
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + new_size;
        }
    }
};

template <class T>
void vector<T>::reserve(size_type n) {
    if (capacity() < n) {
        const size_type old_size = size();
        iterator tmp = data_allocator::allocate(n);
        mystl::uninitialized_move(start, finish, tmp);
        data_allocator::deallocate(start, end_of_storage - start);
        start = tmp;
        finish = start + old_size;
        end_of_storage = start + n;
    }
}

}



#endif //FJXTINYSTL_VECTOR_H
