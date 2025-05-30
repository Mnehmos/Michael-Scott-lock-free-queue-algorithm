#pragma once
#include <vector>
#include <atomic>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <cstddef>

namespace lockfree::msqueue::internal {

constexpr int K = 2; // Hazard pointers per thread
constexpr int R = 2 * K; // Retire threshold

struct HazardPointer {
    std::atomic<void*> ptr;
    char padding[64 - sizeof(std::atomic<void*>)]; // Cache line padding
};

struct RetiredNode {
    void* ptr;
    void (*deleter)(void*);
};

struct ThreadData {
    HazardPointer hps[K];
    std::vector<RetiredNode> retire_list;
    ThreadData* next = nullptr;
};

// Global registry
inline std::atomic<ThreadData*> global_list{nullptr};

class HazardPointerManager {
public:
    void* acquire(int index, void* ptr) {
        if (index < 0 || index >= K) return nullptr;
        
        ThreadData* local_data = get_local_data();
        local_data->hps[index].ptr.store(ptr, std::memory_order_release);
        return ptr;
    }

    void release(int index) {
        if (index < 0 || index >= K) return;
        
        ThreadData* local_data = get_local_data();
        local_data->hps[index].ptr.store(nullptr, std::memory_order_relaxed);
    }

    template <typename T>
    void retire_node(T* ptr) {
        ThreadData* local_data = get_local_data();
        local_data->retire_list.push_back({
            ptr,
            [](void* p) { delete static_cast<T*>(p); }
        });
        if (local_data->retire_list.size() >= R) {
            scan_and_reclaim();
        }
    }
    
private:
    void scan_and_reclaim() {
        ThreadData* local_data = get_local_data();
        
        std::unordered_set<void*> hazardous;
        
        // Collect all protected pointers
        ThreadData* curr = global_list.load(std::memory_order_acquire);
        while (curr) {
            for (int i = 0; i < K; i++) {
                void* ptr = curr->hps[i].ptr.load(std::memory_order_acquire);
                if (ptr) hazardous.insert(ptr);
            }
            curr = curr->next;
        }
        
        // Reclaim non-hazardous nodes
        auto& rlist = local_data->retire_list;
        auto it = rlist.begin();
        while (it != rlist.end()) {
            if (hazardous.find(it->ptr) == hazardous.end()) {
                it->deleter(it->ptr);
                it = rlist.erase(it);
            } else {
                ++it;
            }
        }
    }

    ThreadData* get_local_data() {
        thread_local ThreadData* local_data = []{
            auto td = new ThreadData();
            ThreadData* old = global_list.load(std::memory_order_relaxed);
            do {
                td->next = old;
            } while (!global_list.compare_exchange_weak(old, td, 
                      std::memory_order_release,
                      std::memory_order_relaxed));
            return td;
        }();
        return local_data;
    }
};
} // namespace lockfree::msqueue::internal