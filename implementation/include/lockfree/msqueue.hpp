#pragma once
#include "../../src/node.hpp"
#include "../../src/hazard_pointer.hpp"
#include "../../src/atomic_wrappers.hpp"
#include <memory>
#include <stdexcept>

namespace lockfree::msqueue {

template <typename T>
class Queue {
public:
    Queue() {
        // Create dummy node
        auto dummy = new (std::nothrow) internal::Node<T>();
        if (!dummy) {
            throw std::bad_alloc();
        }
        
        dummy->next.store(nullptr, std::memory_order_relaxed);
        head.store(dummy, std::memory_order_relaxed);
        tail.store(dummy, std::memory_order_relaxed);
    }
    
    ~Queue() {
        // Basic cleanup (will be enhanced with hazard pointers)
        auto curr = head.load(std::memory_order_relaxed);
        while (curr) {
            auto next_ptr = curr->next.load(std::memory_order_relaxed);
            delete curr;
            curr = next_ptr;
        }
    }
    
    bool enqueue(T data) {
        // Create new node
        auto node = new (std::nothrow) internal::Node<T>();
        if (!node) {
            return false; // Allocation failed
        }
        node->data = data;
        // Initialize next as null
        node->next.store(nullptr, std::memory_order_relaxed);

        while (true) {
            auto last = tail.load(std::memory_order_relaxed);
            auto next = last->next.load(std::memory_order_relaxed);
            
            // Verify tail hasn't changed
            if (tail.load(std::memory_order_acquire) != last) {
                continue;
            }
            
            if (next == nullptr) {
                // Try to link new node
                if (last->next.compare_exchange_weak(
                    next,
                    node,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                
                    // Try to update tail (may fail, that's OK)
                    tail.compare_exchange_weak(
                        last,
                        node,
                        std::memory_order_relaxed,
                        std::memory_order_relaxed);
                    return true;
                }
            } else {
                // Help advance tail
                tail.compare_exchange_weak(
                    last,
                    next,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed);
            }
        }
    }
    
    bool dequeue(T& result) {
        while (true) {
            // Protect head pointer
            hp_manager.acquire(0, nullptr);
            auto first = head.load(std::memory_order_acquire);
            hp_manager.acquire(0, first); // Re-protect with actual pointer
            
            auto last = tail.load(std::memory_order_acquire);
            auto next = first->next.load(std::memory_order_acquire);
            
            // Re-check head hasn't changed
            if (head.load(std::memory_order_acquire) != first) {
                continue;
            }
            
            if (first == last) {
                if (next == nullptr) {
                    hp_manager.release(0);
                    return false; // Queue is empty
                }
                // Help advance tail
                tail.compare_exchange_weak(
                    last,
                    next,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed);
            } else {
                // Protect next pointer
                hp_manager.acquire(1, next);
                
                // Re-check head hasn't changed
                if (head.load(std::memory_order_acquire) != first) {
                    hp_manager.release(1);
                    continue;
                }
                
                result = next->data;
                if (head.compare_exchange_weak(
                    first,
                    next,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                
                    hp_manager.retire_node<internal::Node<T>>(first);
                    hp_manager.release(0);
                    hp_manager.release(1);
                    return true;
                }
            }
        }
    }
    
    bool is_empty() const {
        auto h = head.load(std::memory_order_acquire);
        auto t = tail.load(std::memory_order_acquire);
        return (h == t) && (h->next.load(std::memory_order_relaxed) == nullptr);
    }
    
    // Disable copying
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

private:
    std::atomic<internal::Node<T>*> head;
    std::atomic<internal::Node<T>*> tail;
    internal::HazardPointerManager hp_manager;
};

} // namespace lockfree::msqueue