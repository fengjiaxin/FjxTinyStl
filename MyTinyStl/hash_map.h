

#ifndef FJXTINYSTL_HASH_MAP_H
#define FJXTINYSTL_HASH_MAP_H

//
// Created by fengjiaxin on 2023/4/18.
// hashmap
#include "hashtable.h"
#include "functional.h"
#include "hash_fun.h"
#

namespace mystl
{


template <class Key, class T, class HashFcn = mystl::hash<Key>, class EqualKey = mystl::equal_to<Key>>
class hash_map {

private:
    typedef mystl::hashtable<std::pair<const Key, T>, Key, HashFcn, mystl::selectfirst<std::pair<const Key, T>>, EqualKey> ht;
    ht rep; // 底层机制 以 hash table完成
public:
    typedef typename ht::key_type key_type;
    typedef T data_type;
    typedef T mapped_type;
    typedef typename ht::value_type  value_type;
    typedef typename ht::hasher hasher;
    typedef typename ht::key_equal key_equal;

    typedef typename ht::size_type size_type;
    typedef typename ht::difference_type difference_type;
    typedef typename ht::pointer pointer;
    typedef typename ht::const_pointer const_pointer;
    typedef typename ht::reference reference;
    typedef typename ht::const_reference const_reference;
    typedef typename ht::iterator iterator;
    typedef typename ht::const_iterator const_iterator;

    hasher hash_funct() const { return rep.hash_func(); }
    key_equal key_eq() const { return rep.key_eq(); }

public:
    hash_map(): rep(100, hasher(), key_equal()) {}
    explicit hash_map(size_type n) : rep(n, hasher(), key_equal()) {}
    hash_map(size_type n, const hasher& hf) : rep(n, hf, key_equal()) {}
    hash_map(size_type n, const hasher& hf, const key_equal& eql) : rep(n, hf, eql) {}

public:
    // 一些基础属性
    size_type size() const { return rep.size(); }
    bool empty() const { return rep.empty(); }
    void swap(hash_map& hs) { rep.swap(hs.rep); }
    friend bool operator==(const hash_map&, const hash_map&);

    iterator begin() { return rep.begin(); }
    iterator end() { return rep.end(); }
    const_iterator begin() const { return rep.begin(); }
    const_iterator end() const { return rep.end(); }

public:
    // 一些插入，删除，查询api
    std::pair<iterator, bool> insert(const value_type& obj) {
        return rep.insert_unique(obj);
    }

    std::pair<iterator, bool> insert_noresize(const value_type& obj) {
        return rep.insert_unique_noresize(obj);
    }

    iterator find(const key_type& key) { return rep.find(key);}
    const_iterator find(const key_type& key) const { return rep.find(key); }

    T& operator[](const key_type& key) {
        return rep.find_or_insert(value_type(key, T())).second;
    }

    size_type count(const key_type& key) const { return rep.count(key); }
    size_type erase(const key_type& key) { return rep.erase(key); }
    void erase(iterator it) { rep.erase(it); }
    void clear() {rep.clear(); }

public:
    void resize(size_type hint) { rep.resize(hint); }
    size_type bucket_count() const { return rep.bucket_count(); }
    size_type max_bucket_count() const { return rep.max_bucket_count(); }
    size_type elems_in_bucket(size_type n) const { return rep.elems_in_bucket(n); }

};



template<class Key, class T, class HashFcn, class EqualKey>
inline bool operator==(const hash_map<Key, T, HashFcn, EqualKey> left,const hash_map<Key, T, HashFcn, EqualKey> right) {
    return left.rep == right.rep;
}



}
#endif //FJXTINYSTL_HASH_MAP_H
