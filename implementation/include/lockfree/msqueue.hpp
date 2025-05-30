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
        head = {dummy, 0};
        tail = {dummy, 0};
    }
    
    ~Queue() {
        // Basic cleanup (will be enhanced with hazard pointers)
        auto curr = head.load(std::memory_order_relaxed).ptr;
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
        // Initialize next as null pointer_t
        internal::pointer_t<T> null_next{nullptr, 0};
        node->next.store(null_next, std::memory_order_relaxed);

        while (true) {
            auto last = internal::load(tail, std::memory_order_relaxed);
            auto next = internal::load(last.ptr->next, std::memory_order_relaxed);
            
            // Verify tail hasn't changed
            if (internal::load(tail, std::memory_order_acquire) != last) {
                continue;
            }
            
            if (next.ptr == nullptr) {
                // Try to link new node
                internal::pointer_t<T> new_next{node, next.counter + 1};
                if (internal::compare_exchange(
                    last.ptr->next,
                    next,
                    new_next,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                    
                    // Try to update tail (may fail, that's OK)
                    internal::pointer_t<T> new_tail{node, last.counter + 1};
                    internal::compare_exchange(
                        tail,
                        last,
                        new_tail,
                        std::memory_order_relaxed,
                        std::memory_order_relaxed);
                    return true;
                }
            } else {
                // Help advance tail
                internal::pointer_t<T> new_tail{next.ptr, last.counter + 1};
                internal::compare_exchange(
                    tail,
                    last,
                    new_tail,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed);
            }
        }
    }
    
    bool dequeue(T& result) {
        while (true) {
            // Protect head pointer
            hp_manager.acquire(0, nullptr);
            internal::pointer_t<T> first = internal::load(head, std::memory_order_acquire);
            hp_manager.acquire(0, first.ptr); // Re-protect with actual pointer
            
            internal::pointer_t<T> last = internal::load(tail, std::memory_order_acquire);
            internal::pointer_t<T> next = internal::load(first.ptr->next, std::memory_order_acquire);
            
            // Re-check head hasn't changed
            if (internal::load(head, std::memory_order_acquire) != first) {
                continue;
            }
            
            if (first.ptr == last.ptr) {
                if (next.ptr == nullptr) {
                    hp_manager.release(0);
                    return false; // Queue is empty
                }
                // Help advance tail
                internal::pointer_t<T> new_tail{next.ptr, last.counter + 1};
                internal::compare_exchange(
                    tail,
                    last,
                    new_tail,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed);
            } else {
                // Protect next pointer
                hp_manager.acquire(1, next.ptr);
                
                // Re-check head hasn't changed
                if (internal::load(head, std::memory_order_acquire) != first) {
                    hp_manager.release(1);
                    continue;
                }
                
                result = next.ptr->data;
                internal::pointer_t<T> new_head{next.ptr, first.counter + 1};
                if (internal::compare_exchange(
                    head,
                    first,
                    new_head,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                
                    hp_manager.retire_node(first.ptr);
                    hp_manager.release(0);
                    hp_manager.release(1);
                    return true;
                }
            }   // Added missing closing brace for else block
        }
    }
    
    bool is_empty() const {
        auto h = internal::load(head, std::memory_order_acquire);
        auto t = internal::load(tail, std::memory_order_acquire);
        return (h.ptr == t.ptr) &&
               (internal::load(h.ptr->next, std::memory_order_relaxed).ptr == nullptr);
    }
    
    // Disable copying
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

private:
    std::atomic<internal::pointer_t<T>> head;
    std::atomic<internal::pointer_t<T>> tail;
    internal::HazardPointerManager hp_manager;
};  // Added missing semicolon after class definition

} // namespace lockfree::msqueue