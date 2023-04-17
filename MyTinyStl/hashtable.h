#ifndef FJXTINYSTL_HASHTABLE_H
#define FJXTINYSTL_HASHTABLE_H

//
// Created by fengjiaxin on 2023/4/15.
// hash表， 使用开链法处理冲突
//
#include "vector.h"
#include "util.h"
#include "iterator.h"

namespace mystl
{


// hashtable 的节点定义
template <class Value>
struct __hashtable_node {
    __hashtable_node* next;
    Value value;

    __hashtable_node() = default;
    __hashtable_node(const Value& n):next(nullptr), value(n) {}

    __hashtable_node(const __hashtable_node& node):next(node.next),value(node.value) {}
    __hashtable_node(__hashtable_node&& node):next(node.next),value(node.value) {
        node.next = nullptr;
    }
};

// 前置声明
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class hashtable;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class __hashtable_iterator;

// 迭代器
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class __hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey> hashtable;
    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> iterator;
    typedef __hashtable_node<Value> node;

    // 迭代器5种基本类型
    typedef mystl::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value &reference;
    typedef Value *pointer;

    node *cur;
    hashtable *ht;

    // 构造函数
    __hashtable_iterator() {}

    __hashtable_iterator(node *n, hashtable *h) : cur(n), ht(h) {}

    // 操作符重载
    reference operator*() const { return cur->value; }

    reference operator->() const { return &(operator*()); }

    bool operator==(const iterator &it) const { return cur == it.cur; }

    bool operator!=(const iterator &it) const { return cur != it.cur; }

    // 操作符移动
    iterator &operator++(); // 需要借用ht的函数实现
    iterator operator++(int);
}

// bucket 的长度
// Note: assumes long is at least 32 bits.
static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
        53,         97,           193,         389,       769,
        1543,       3079,         6151,        12289,     24593,
        49157,      98317,        196613,      393241,    786433,
        1572869,    3145739,      6291469,     12582917,  25165843,
        50331653,   100663319,    201326611,   402653189, 805306457,
        1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long __stl_next_prime(unsigned long n)
{
    const unsigned long* first = __stl_prime_list;
    const unsigned long* last = __stl_prime_list + __stl_num_primes;
    const unsigned long* pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class hashtable {
public:
    typedef Key  key_type;
    typedef value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;

    // hashtable 型别定义
    typedef size_t  size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef vonst value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    hasher hash_func() const { return hash; }
    key_equal key_eq() const { return equals; }

    typedef __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> iterator;

private:
    typedef __hashtable_node<Value> node;
    typedef mystl::allocator<Value>   data_allocator;
    typedef mystl::allocator<Value>   allocator_type;
    typedef mystl::allocator<node>    node_allocator;


    hasher  hash;
    key_equal equals;
    ExtractKey get_key;
    mystl::vector<node*> buckets;
    size_type num_elements;

public:
    // 和容量相关的查询
    size_type size() const { return num_elements; }
    bool empty() const { return size() == 0; }
    void swap(hashtable& ht) {
        mystl::swap(hash, ht.hash);
        mystl::swap(equals, ht.equals);
        mystl::swap(get_key, ht.get_key);
        buckets.swap(ht.buckets);
        mystl::swap(num_elements, ht.num_elements);
    }
    // 查询边界
    iterator begin() {
        for (size_type i = 0; i < buckets.size(); ++i) {
            if (buckets[i] != nullptr)
                return iterator(buckets[i], this);
        }
        return end();
    }
    iterator end() { return iterator(nullptr, this); }

public:
    // 和bucket 相关的helper function
    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const { return __stl_prime_list[__stl_num_primes - 1];}
    size_type elems_in_bucket(size_type bucket) const {
        size_type res = 0;
        for (node* cur = buckets[bucket]; cur != nullptr;cur = cur->next)
            res += 1;
        return res;


    }
private:
    size_type next_size(size_type n) const { return mystl::__stl_num_primes(n);}

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, nullptr);
        num_elements = 0;
    }
    // 和 hash相关的helper function
    // 计算应该在哪个桶
    size_type bkt_num_key(const key_type& key, size_t n) const {
        return hasher(key) % n;
    }
    size_type bkt_num_key(const key_type& key) const {
        return bkt_num_key(key, buckets.size());
    }

    size_type bkt_num(const value_type& value) const {
        return bkt_num_key(get_key(value));
    }
    size_type bkt_num(const value_type& value, size_t n) const {
        return bkt_num_key(get_key(value), n);
    }

    // 创建/销毁 节点
    node* new_node(const value_type& value) {
        node* ptr = node_allocator::allocate();
        ptr->next = nullptr;
        try {
            mystl::construct(&ptr->value, value);
            return ptrl
        } catch (...) {
            node_allocator::deallocate(ptr);
            throw;
        }
    }
    void delete_node(node* ptr) {
        mystl::destroy(&ptr->value);
        node_allocator::deallocate(ptr);
    }

    void copy_from(const hashtable& ht);

public:
    // 构造函数
    hashtable()



public:




}





}













#endif //FJXTINYSTL_HASHTABLE_H
