#ifndef FJXTINYSTL_HASH_FUN_H
#define FJXTINYSTL_HASH_FUN_H

//
// Created by fengjiaxin on 2023/4/17.
// 定义各种hash函数
#include <cstddef> // size_t

// 对于大部分类型， hash function什么都不做
namespace mystl
{


template<class Key>
struct hash {};


// 对于整型类型，只是返回原值
#define MYSTL_TRIVIAL_HASH_FCN(Type)         \
template <> struct hash<Type>                \
{                                            \
  size_t operator()(Type val) const noexcept \
  { return static_cast<size_t>(val); }       \
};

MYSTL_TRIVIAL_HASH_FCN(bool)

MYSTL_TRIVIAL_HASH_FCN(char)

MYSTL_TRIVIAL_HASH_FCN(signed char)

MYSTL_TRIVIAL_HASH_FCN(unsigned char)

MYSTL_TRIVIAL_HASH_FCN(wchar_t)

MYSTL_TRIVIAL_HASH_FCN(char16_t)

MYSTL_TRIVIAL_HASH_FCN(char32_t)

MYSTL_TRIVIAL_HASH_FCN(short)

MYSTL_TRIVIAL_HASH_FCN(unsigned short)

MYSTL_TRIVIAL_HASH_FCN(int)

MYSTL_TRIVIAL_HASH_FCN(unsigned int)

MYSTL_TRIVIAL_HASH_FCN(long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long)

MYSTL_TRIVIAL_HASH_FCN(long long)

MYSTL_TRIVIAL_HASH_FCN(unsigned long long)

#undef MYSTL_TRIVIAL_HASH_FCN

inline size_t __stl_hash_string(const char *s) {
    unsigned long h = 0;
    while (*s) {
        h = 5 * h + *s;
        ++s;
    }
    return static_cast<size_t>(h);
}

template<>
struct hash<char *> {
    size_t operator()(const char *s) const { return __stl_hash_string(s); }
};

template<>
struct hash<const char *> {
    size_t operator()(const char *s) const { return __stl_hash_string(s); }
};


}

#endif //FJXTINYSTL_HASH_FUN_H
