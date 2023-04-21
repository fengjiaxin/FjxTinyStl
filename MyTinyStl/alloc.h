#ifndef FJXTINYSTL_ALLOC_H
#define FJXTINYSTL_ALLOC_H

//
// Created by fengjiaxin on 2023/4/19.
// 内存池分配， 测试， 参考stl的 alloc
#include <cstddef> // size_t
#include <stdlib.h> // malloc, free
#include <iostream>

namespace mystl {

// 分配器， 分配的size 按照是否 > 128 byte
// 1. 大于 128 byte, 通过malloc分配内存
// 2. 小于 128 byte, 通过 内存池的方式分配内存


// 请求内存 > 128 byte
class malloc_alloc {
private:
    static void* oom_malloc(size_t);
    static void* oom_realloc(void*, size_t);
    static void (*malloc_alloc_oom_handler)(); // malloc_alloc_oom_handler 是在malloc失败后的处理 函数指针

public:
    static void* allocate(size_t n) {
        void* result = malloc(n);
        if (nullptr == result) result = oom_malloc(n);
        return result;
    }
    static void deallocate(void* p, size_t) {
        free(p);
    }
    static void* reallocate(void* p, size_t /*old size*/, size_t new_sz) {
        void* result = realloc(p, new_sz);
        if (nullptr == result) result = oom_realloc(p, new_sz);
        return result;
    }

    typedef void (*H)(); // 定义函数指针
    H set_malloc_handler(H f) {
        H old = malloc_alloc_oom_handler;
        malloc_alloc_oom_handler = f;
        return old;
    }
};

void (*malloc_alloc::malloc_alloc_oom_handler)() = nullptr;

void* malloc_alloc::oom_malloc(size_t n) {
    typedef void (*H)(); // 定义函数指针
    H my_alloc_handler;
    void* res;
    for(;;) {
        my_alloc_handler = malloc_alloc_oom_handler;
        if (nullptr == my_alloc_handler) {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
        my_alloc_handler();
        res = malloc(n);
        if (nullptr != res) return res;

    }
}

void* malloc_alloc::oom_realloc(void* p, size_t n) {
    typedef void (*H)(); // 定义函数指针
    H my_alloc_handler;
    void* res;
    for(;;) {
        my_alloc_handler = malloc_alloc_oom_handler;
        if (nullptr == my_alloc_handler) {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
        my_alloc_handler();
        res = realloc(p,n);
        if (nullptr != res) return res;
    }
}


// 二级分配器， <= 128 byte的内存从这里分配
class default_alloc {

private:
    static const int ALIGN = 8;
    static const int MAX_BYTES = 128;
    static const int N_FREELISTS = MAX_BYTES / ALIGN;

    static size_t ROUND_UP(size_t bytes) {
        return ( bytes + ALIGN - 1) & ~(ALIGN - 1);
    }

    // embedded pointer
    struct obj {
        struct obj* free_list_link;
    };

    static obj* free_list[N_FREELISTS]; // free_list 是个指针, 指向数组的头位置，数组中每个位置存的是obj的地址
    static size_t FREELIST_INDEX(size_t bytes) {
        return ((bytes + ALIGN - 1)/ALIGN - 1);
    }

    static void* refill(size_t n);
    static char* chunk_alloc(size_t size, int& nobjs);

    // chunk allocation state
    static char* start_free;
    static char* end_free;
    static size_t heap_size;

public:
    // n must be > 0
    static void* allocate(size_t n) {
        obj** my_free_list;
        obj* result;
        if (n > static_cast<size_t>(MAX_BYTES)) {
            return malloc_alloc::allocate(n);
        }
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (nullptr == result) {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        *my_free_list = result->free_list_link;
        return reinterpret_cast<void*>(result);
    }

    static void deallocate(void* p, size_t n) {
        obj* q = reinterpret_cast<obj*>(p);
        obj** my_free_list;
        if (n > static_cast<size_t>(MAX_BYTES)) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
    static void* reallocate(void* p, size_t old_sz, size_t new_sz);

};

typedef default_alloc alloc;

// 该方法是从战备池中找地址，如果不够，申请
char* default_alloc::chunk_alloc(size_t size, int &nobjs) {
    char* result;
    size_t total_bytes = size * nobjs;
    size_t bytes_left = end_free - start_free;

    if (bytes_left >= total_bytes) {
        result = start_free;
        start_free += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        nobjs = bytes_left / size;
        total_bytes = size * nobjs;
        result = start_free;
        start_free += total_bytes;
        return result;
    } else { // 战备池不够
        if (bytes_left > 0) { // 将剩余的内存链接到合适的链表
            obj** my_free_list = free_list + FREELIST_INDEX(bytes_left);
            ((obj*)start_free) ->free_list_link = *my_free_list;
            *my_free_list = (obj*)start_free;
        }
        // 申请内存
        size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
        start_free = (char*)malloc(bytes_to_get);
        if (nullptr == start_free) { // 机器内存不够了
            int i;
            obj** my_free_list;
            obj* p;
            for (i = size; i <= MAX_BYTES; i+= ALIGN) {
                my_free_list = free_list + FREELIST_INDEX(i);
                p = *my_free_list;
                if (nullptr != p) {
                    *my_free_list = p->free_list_link;
                    start_free = (char*)p;
                    end_free = start_free + i;
                    return chunk_alloc(size, nobjs);
                }
            }
            end_free = nullptr;
            start_free = (char*)malloc_alloc::allocate(bytes_to_get);
        } else {
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, nobjs);
        }
    }
}

void* default_alloc::refill(size_t n) {
    int nobjs = 20;
    char* chunk = chunk_alloc(n, nobjs);
    obj** my_free_list;
    obj* result;
    obj* current_obj;
    obj* next_obj;
    int i;
    if (1 == nobjs) return chunk; // chunk_alloc 成功只分配一个
    my_free_list = free_list + FREELIST_INDEX(n);
    // build free list in chunk
    result = reinterpret_cast<obj*>(chunk);
    *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);
    for (i = 1;;++i) {
        current_obj = next_obj;
        char* next_addr = reinterpret_cast<char*>(next_obj) + n;
        next_obj = reinterpret_cast<obj*>(next_addr);
        if (nobjs - 1 == i) {
            current_obj->free_list_link = nullptr;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

void* default_alloc::reallocate(void* p, size_t old_sz, size_t new_sz) {
    void* result;
    size_t copy_sz;
    if (old_sz > static_cast<size_t>(MAX_BYTES) && new_sz > static_cast<size_t>(MAX_BYTES) ) {
        return realloc(p, new_sz);
    }
    if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
    result = allocate(new_sz);
    copy_sz = new_sz > old_sz ? old_sz : new_sz;
    memcpy(result, p, copy_sz);
    deallocate(p, old_sz);
    return result;
}

char* default_alloc::start_free = nullptr;
char* default_alloc::end_free = nullptr;
size_t default_alloc::heap_size = 0;
default_alloc::obj* default_alloc::free_list[default_alloc::N_FREELISTS] =
        {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
         nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
}




#endif //FJXTINYSTL_ALLOC_H
