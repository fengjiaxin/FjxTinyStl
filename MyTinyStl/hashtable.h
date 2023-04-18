#ifndef FJXTINYSTL_HASHTABLE_H
#define FJXTINYSTL_HASHTABLE_H

//
// Created by fengjiaxin on 2023/4/15.
// hash表， 使用开链法处理冲突
//
#include "vector.h"
#include "util.h"
#include "iterator.h"
#include <utility>
#include "algo.h"

namespace mystl
{


// hashtable 的节点定义
template <class Value>
struct hashtable_node
{
    hashtable_node* next;
    Value value;

    hashtable_node() = default;
    hashtable_node(const Value& n):next(nullptr), value(n) {}

    hashtable_node(const hashtable_node& node):next(node.next),value(node.value) {}
    hashtable_node(hashtable_node&& node):next(node.next),value(node.value) {
        node.next = nullptr;
    }
};

// 前置声明
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class hashtable;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
struct hashtable_iterator;

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
struct hashtable_const_iterator;

// 迭代器
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
struct hashtable_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey> hashtable;
    typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> const_iterator;
    typedef hashtable_node<Value> node;

    // 迭代器5种基本类型
    typedef mystl::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef Value &reference;
    typedef Value *pointer;

    node* cur;
    hashtable* ht;

    // 构造函数
    hashtable_iterator() {}

    hashtable_iterator(node* n, hashtable* h) : cur(n), ht(h) {}

    // 操作符重载
    reference operator*() const { return cur->value; }

    pointer operator->() const { return &(operator*()); }

    bool operator==(const iterator &it) const { return cur == it.cur; }

    bool operator!=(const iterator &it) const { return cur != it.cur; }

    // 操作符移动
    iterator &operator++(); // 需要借用ht的函数实现
    iterator operator++(int);
};

// 迭代器
template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
struct hashtable_const_iterator {
    typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey> hashtable;
    typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> const_iterator;
    typedef hashtable_node<Value> node;

    // 迭代器5种基本类型
    typedef mystl::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;
    typedef const Value& reference;
    typedef const Value* pointer;

    node* cur;
    hashtable* ht;

    // 构造函数
    hashtable_const_iterator() {}

    hashtable_const_iterator(const node* n, const hashtable* h) : cur(n), ht(h) {}

    hashtable_const_iterator(const iterator& it) : cur(it.cur), ht(it.ht) {}

    // 操作符重载
    reference operator*() const { return cur->value; }

    reference operator->() const { return &(operator*()); }

    bool operator==(const const_iterator &it) const { return cur == it.cur; }

    bool operator!=(const const_iterator &it) const { return cur != it.cur; }

    // 操作符移动
    const_iterator &operator++(); // 需要借用ht的函数实现
    const_iterator operator++(int);
};



// bucket 的长度
// Note: assumes long is at least 32 bits.
static const int stl_num_primes = 28;
static const unsigned long stl_prime_list[stl_num_primes] =
{
        53,         97,           193,         389,       769,
        1543,       3079,         6151,        12289,     24593,
        49157,      98317,        196613,      393241,    786433,
        1572869,    3145739,      6291469,     12582917,  25165843,
        50331653,   100663319,    201326611,   402653189, 805306457,
        1610612741, 3221225473ul, 4294967291ul
};

inline unsigned long stl_next_prime(unsigned long n)
{
    const unsigned long* first = stl_prime_list;
    const unsigned long* last = stl_prime_list + stl_num_primes;
    const unsigned long* pos = lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
class hashtable {
public:
    typedef Key  key_type;
    typedef Value value_type;
    typedef HashFcn hasher;
    typedef EqualKey key_equal;

    // hashtable 型别定义
    typedef size_t  size_type;
    typedef ptrdiff_t difference_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    hasher hash_func() const { return hash; }
    key_equal key_eq() const { return equals; }

    typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> iterator;
    typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey> const_iterator;
    friend struct hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
    friend struct hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;

private:
    typedef hashtable_node<Value> node;
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

    const_iterator begin() const {
        for (size_type i = 0; i < buckets.size(); ++i) {
            if (buckets[i] != nullptr)
                return const_iterator(buckets[i], this);
        }
        return end();
    }

    iterator end() { return iterator(nullptr, this); }

    const_iterator end() const { return const_iterator(nullptr, this); }

public:
    // 和bucket 相关的helper function
    size_type bucket_count() const { return buckets.size(); }
    size_type max_bucket_count() const { return stl_prime_list[stl_num_primes - 1];}
    size_type elems_in_bucket(size_type bucket) const {
        size_type res = 0;
        for (node* cur = buckets[bucket]; cur != nullptr;cur = cur->next)
            res += 1;
        return res;
    }

    // 插入元素，不允许重复
    std::pair<iterator, bool> insert_unique(const value_type& obj) {
        resize(num_elements + 1);
        return insert_unique_noresize(obj);
    }

    // 插入元素， 允许重复
    std::pair<iterator, bool> insert_equal(const value_type& obj) {
        resize(num_elements + 1);
        return insert_equal_noresize(obj);
    }

    size_type erase(const key_type& key);
    void erase(const iterator& it);

    void clear(); // 并未释放掉vector空间，仍保持原来大小

    reference find_or_insert(const value_type& obj);
    iterator find(const key_type& key) {
        size_type n = bkt_num_key(key);
        node* first = buckets[n];
        while (first && !equals(get_key(first->value), key)) {
            first = first->next;
        }
        return iterator(first, this);
    }

    const_iterator find(const key_type& key) const {
        size_type n = bkt_num_key(key);
        node* first = buckets[n];
        while (first && !equals(get_key(first->value), key)) {
            first = first->next;
        }
        return const_iterator(first, this);
    }

    size_type count(const key_type& key) const {
        const size_type n = bkt_num_key(key);
        size_type res = 0;
        const node* cur = buckets[n];
        while (cur) {
            if (equals(get_key(cur->value), key))
                ++res;
            cur = cur->next;
        }
        return res;
    }



private:
    size_type next_size(size_type n) const { return mystl::stl_next_prime(n);}

    // 创建/销毁 节点
    node* new_node(const value_type& value) {
        node* ptr = node_allocator::allocate();
        ptr->next = nullptr;
        try {
            mystl::construct(&ptr->value, value);
            return ptr;
        } catch (...) {
            node_allocator::deallocate(ptr);
            throw;
        }
    }
    void delete_node(node* ptr) {
        mystl::destroy(&ptr->value);
        node_allocator::deallocate(ptr);
    }

    void initialize_buckets(size_type n) {
        const size_type n_buckets = next_size(n);
        buckets.reserve(n_buckets);
        buckets.insert(buckets.end(), n_buckets, nullptr);
        num_elements = 0;
    }

    // 和 hash相关的helper function
    // 计算应该在哪个桶
    size_type bkt_num_key(const key_type& key, size_t n) const {
        return hash(key) % n;
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

    // 是否需要重建表格
    void resize(size_type num_elements_hint);
    // 在不需要重建表格的情况下插入新节点，键值不允许重复
    std::pair<iterator, bool> insert_unique_noresize(const value_type& obj);
    // 在不需要重建表格的情况下插入新节点，键值允许重复
    iterator insert_equal_noresize(const value_type& obj);


    void copy_from(const hashtable& ht);

public:
    // 构造函数
    hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
        : hash(hf), equals(eql), get_key(ExtractKey()), num_elements(0) {
        initialize_buckets(n);
    }

};

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::resize(size_type num_elements_hint) {
    // 表格是否重建的判断原则， num_elements_hint > buckets.size 就重建
    const size_type old_n = buckets.size();
    if (num_elements_hint > old_n) {
        // 找出下个质数
        const size_type n = next_size(num_elements_hint);
        if (n > old_n) {
            mystl::vector<node*> tmp(n, nullptr);
            try {
                for(size_type bucket = 0; bucket < old_n; ++bucket) {
                    node* first = buckets[bucket];
                    while (first) {
                        size_type new_bucket = bkt_num(first->value, n);
                        // 以下4个操作
                        // 1.旧的bucket指向下一个节点
                        buckets[bucket] = first->next;
                        // 2.3 将当前节点插入到新的bucket中
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        // 4. 回到旧bucket,准备处理下一个节点
                        first = buckets[bucket];
                    }
                }
                buckets.swap(tmp);
            } catch (...) {
                for (size_type bucket = 0; bucket < tmp.size(); ++bucket) {
                    while (tmp[bucket]) {
                        node* next = tmp[bucket]->next;
                        delete_node(tmp[bucket]);
                        tmp[bucket] = next;
                    }
                }
                throw;
            }
        }
    }
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_equal_noresize(const value_type &obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];
    for (node* cur = first; cur; cur = cur->next) {
        if (equals(get_key(cur->value), get_key(obj))) {
            node* tmp = new_node(obj);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator(tmp, this);
        }
    }
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++ num_elements;
    return iterator(tmp, this);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
std::pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::insert_unique_noresize(const value_type &obj) {
    const size_type n = bkt_num(obj);
    node* first = buckets[n];
    for (node* cur = first; cur; cur = cur->next) {
        if (equals(get_key(cur->value), get_key(obj)))
            return std::pair<iterator, bool>(iterator(cur, this), false);
    }
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++ num_elements;
    return std::pair<iterator, bool>(iterator(tmp, this), true);
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::clear() {
    for(size_type i = 0; i < buckets.size(); ++i) {
        node* cur = buckets[i];
        while (cur) {
            node* next = cur->next;
            delete_node(cur);
            cur = next;
        }
        buckets[i] = nullptr;
    }
    num_elements = 0;
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::copy_from(const hashtable& ht) {
    // 先清除己方的buckets vector
    buckets.clear();
    buckets.reserve(ht.buckets.size());
    buckets.insert(buckets.end(), ht.buckets.size(), nullptr);
    try {
        for (size_type i = 0; i < ht.buckets.size(); ++i) {
            if (const node* cur = ht.buckets[i]) {
                 node* copy = new_node(cur->value);
                 buckets[i] = copy;
                 node* next = cur->next;
                 while (next) {
                     copy->next = new_node(next->value);
                     copy = copy->next;
                     cur = next;
                     next = cur->next;
                 }
            }
        }
        num_elements = ht.num_elements;
    } catch (...) {
        clear();
        throw;
    }
}

template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::erase(const key_type& key) {
    const size_type n = bkt_num_key(key);
    node* first = buckets[n];
    size_type res = 0;
    if (first) {
        node* cur = first;
        node* next = cur->next; // 先从第二个处理
        while (next) {
            if (equals(get_key(next->value), key)) {
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                ++res;
                --num_elements;
            } else {
                cur = next;
                next = cur->next;
            }
        }
        if (equals(get_key(first->value), key)) {
            buckets[n] = first->next;
            delete_node(first);
            ++res;
            --num_elements;
        }
    }
    return res;
}


template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::erase(const iterator& it) {
    if (node* const p = it.cur) {
        const size_type n = bkt_num(p->value);
        node* cur = buckets[n];

        if (cur == p) {
            buckets[n] = cur->next;
            delete_node(cur);
            --num_elements;
        } else {
            node* next = cur->next;
            while (next) {
                if (next == p) {
                    cur->next = next->next;
                    delete_node(next);
                    --num_elements;
                    break;
                } else {
                    cur = next;
                    next = cur->next;
                }
            }
        }
    }
}


template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::reference
hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::find_or_insert(const value_type &obj) {
    resize(num_elements + 1);
    size_type n = bkt_num(obj);
    node* first = buckets[n];

    for(node* cur = first; cur; cur = cur->next)
        if (equals(get_key(cur->value), get_key(obj)))
            return cur->value;
    node* tmp = new_node(obj);
    tmp->next = first;
    buckets[n] = tmp;
    ++num_elements;
    return tmp->value;
}


template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>&
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++() {
    const node* old = cur;
    cur = cur->next;
    if (cur == nullptr) {
        size_type bucket = ht->bkt_num(old->value);
        while (cur == nullptr && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>
hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}


template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>&
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++() {
    const node* old = cur;
    cur = cur->next;
    if (cur == nullptr) {
        size_type bucket = ht->bkt_num(old->value);
        while (cur == nullptr && ++bucket < ht->buckets.size())
            cur = ht->buckets[bucket];
    }
    return *this;
}

template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>
hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

}

#endif //FJXTINYSTL_HASHTABLE_H
