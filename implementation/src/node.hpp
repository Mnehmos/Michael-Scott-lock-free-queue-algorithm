#pragma once
#include <atomic>
#include <cstdint>

namespace lockfree::msqueue::internal {

template <typename T>
struct Node {
    T data;
    std::atomic<Node<T>*> next;
    uintptr_t aba_counter = 0;
};

template <typename T>
struct pointer_t {
    Node<T>* ptr;
    uintptr_t counter;
    
    bool operator==(const pointer_t& other) const {
        return ptr == other.ptr && counter == other.counter;
    }
    
    bool operator!=(const pointer_t& other) const {
        return !(*this == other);
    }
};

} // namespace lockfree::msqueue::internal
