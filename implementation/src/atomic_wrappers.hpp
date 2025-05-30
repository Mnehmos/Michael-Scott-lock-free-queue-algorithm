#pragma once
#include <atomic>
#include "node.hpp"

namespace lockfree::msqueue::internal {

// Overload for atomic types
template <typename T>
bool compare_exchange(std::atomic<T>& obj, T& expected, T desired,
                      std::memory_order success,
                      std::memory_order failure) {
    return obj.compare_exchange_weak(expected, desired, success, failure);
}

// Overload for pointer_t types
template <typename T>
bool compare_exchange(std::atomic<pointer_t<T>>& obj, pointer_t<T>& expected,
                      pointer_t<T> desired, std::memory_order success,
                      std::memory_order failure) {
    return obj.compare_exchange_weak(expected, desired, success, failure);
}

// Overload for atomic types
template <typename T>
T load(const std::atomic<T>& obj, std::memory_order order) {
    return obj.load(order);
}

// Overload for pointer_t types
template <typename T>
pointer_t<T> load(const std::atomic<pointer_t<T>>& obj, std::memory_order order) {
    return obj.load(order);
}

// Overload for atomic types
template <typename T>
void store(std::atomic<T>& obj, T value, std::memory_order order) {
    obj.store(value, order);
}

// Overload for pointer_t types
template <typename T>
void store(std::atomic<pointer_t<T>>& obj, pointer_t<T> value, std::memory_order order) {
    obj.store(value, order);
}

} // namespace lockfree::msqueue::internal