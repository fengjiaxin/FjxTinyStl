#ifndef FJXTINYSTL_HASH_SET_H
#define FJXTINYSTL_HASH_SET_H

//
// Created by fengjiaxin on 2023/4/17.
// hash set
#include "hashtable.h"
#include "functional.h"
#include "hash_fun.h"

namespace mystl {

template <class Value, class HashFcn = mystl::hash<Value>, class EqualKey = mystl::equal_to<Value>>
class hash_set
{
private:
    typedef mystl::hashtable<Value, Value, HashFcn, mystl::identity<Value>, EqualKey> ht;
    ht rep; // 底层机制 以 hash table完成

public: // 型别定义
    typedef typename ht::key_type  key_type;
    typedef typename ht::value_type  value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;

    typedef typename ht::size_type  size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::const_pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::const_reference reference;
    typedef typename ht::const_reference const_reference;

    typedef typename ht::iterator iterator;
//    typedef typename ht::const_iterator const_iterator;

    hasher hash_funct() const { return rep.hash_func(); }
    key_equal key_eq() const { return rep.key_eq(); }

public: // 构造函数
    hash_set():rep(100, hasher(), key_equal()) {}
    explicit hash_set(size_type n) : rep(n, hasher(), key_equal()) {}
    hash_set(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    hash_set(size_type n, const hasher& hf, const key_equal& eql)
            : rep(n, hf, eql) {}

public: // 基础属性api
    size_type size() { return rep.size(); }
    bool empty() { return rep.empty(); }
    void swap(hash_set& hs) { rep.swap(hs.rep);}
    friend bool operator==(const hash_set& , const hash_set&);

    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }

public: // 涉及到改变数据的操作api
    std::pair<iterator, bool> insert(const value_type& obj) {
        std::pair<typename ht::iterator, bool> p = rep.insert_unique(obj);
        return std::pair<iterator,bool>(p.first, p.second);
    }

    std::pair<iterator, bool> insert_noresize(const value_type& obj) {
        std::pair<typename ht::iterator, bool> p = rep.insert_unique_noresize(obj);
        return std::pair<iterator, bool>(p.first, p.second);
    }

    iterator find(const key_type& key) const { return rep.find(key);}
    size_type count(const key_type& key) const { return rep.count(key); }

    size_type erase(const key_type& key) { return rep.erase(key);}
    void erase(iterator it) { rep.erase(it); }
    void clear() { rep.clear(); }

public: // 一些辅助函数
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }
};

template <class Value, class HashFcn, class EqualKey>
inline bool operator==(const hash_set<Value, HashFcn, EqualKey> left, const hash_set<Value, HashFcn, EqualKey> right) {
    return left.rep == right.rep;
}

}



#endif //FJXTINYSTL_HASH_SET_H
