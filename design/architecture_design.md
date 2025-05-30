# Phase 3: ARCHITECTURE

## Task 3.1: System Architecture Design

### Class Hierarchy
```mermaid
classDiagram
    class MSQueue<T> {
        -Atomic<pointer_t> head
        -Atomic<pointer_t> tail
        -HazardPointerManager hp_manager
        +enqueue(T data) bool
        +dequeue(T& result) bool
        +is_empty() bool
    }
    
    class HazardPointerManager {
        -thread_local HazardPointer hps[K]
        -thread_local vector<Node*> retire_list
        -atomic<HazardPointer*> global_list
        +acquire(int index, void* ptr) void*
        +release(int index) void
        +retire_node(void* ptr) void
        +scan_and_reclaim() void
    }
    
    class Node<T> {
        -T data
        -Atomic<Node<T>*> next
        -uintptr_t aba_counter
    }
    
    MSQueue *-- HazardPointerManager
    MSQueue *-- Node
```

### Template Design
- Templated on data type T
- Header-only implementation
- SFINAE for type constraints

### Namespace Organization
```cpp
namespace lockfree {
    namespace msqueue {
        template <typename T>
        class Queue { ... };
        
        namespace internal {
            template <typename T>
            struct Node { ... };
            
            struct HazardPointer { ... };
        }
    }
}
```

### Header File Structure
```
include/
└── lockfree/
    ├── msqueue.hpp      // Main queue interface
    └── hazard_pointer.hpp  // HP implementation
```

### CMake Configuration
```cmake
cmake_minimum_required(VERSION 3.15)
project(lockfree-queue VERSION 1.0)

add_library(lockfree-queue INTERFACE)
target_include_directories(lockfree-queue INTERFACE include)

# Testing configuration
enable_testing()
add_subdirectory(tests)
```

## Task 3.2: Module Design

### Queue Module Interface
```cpp
// msqueue.hpp
#pragma once

namespace lockfree::msqueue {
    template <typename T>
    class Queue {
    public:
        Queue();
        ~Queue();
        
        bool enqueue(T data);
        bool dequeue(T& result);
        bool is_empty() const;
        
        // Disable copying
        Queue(const Queue&) = delete;
        Queue& operator=(const Queue&) = delete;
    };
}
```

### Hazard Pointer Module Interface
```cpp
// hazard_pointer.hpp
#pragma once

namespace lockfree::msqueue::internal {
    class HazardPointerManager {
    public:
        void* acquire(int index, void* ptr);
        void release(int index);
        void retire_node(void* ptr);
        
    private:
        void scan_and_reclaim();
    };
}
```

### Memory Allocator Interface
```cpp
// allocator.hpp
#pragma once

namespace lockfree::msqueue {
    template <typename T>
    class Allocator {
    public:
        virtual T* allocate() = 0;
        virtual void deallocate(T* ptr) = 0;
        virtual ~Allocator() = default;
    };
    
    // Default allocator using new/delete
    template <typename T>
    class DefaultAllocator : public Allocator<T> { ... };
}
```

### Testing Framework Interface
```cpp
// tests/test_framework.hpp
#pragma once

#define THREADS 4
#define ITERATIONS 100000

void run_single_thread_tests();
void run_concurrent_tests();
void run_property_based_tests();
void run_performance_tests();
```

### Benchmark Harness
```cpp
// benchmark/benchmark.hpp
#pragma once

struct BenchmarkResults {
    double enqueue_ops_sec;
    double dequeue_ops_sec;
    size_t memory_usage;
};

BenchmarkResults run_benchmark(int producer_threads, int consumer_threads, int duration_sec);
```

## Task 3.3: Concurrency Architecture

### Thread-Local Storage
- Hazard pointers stored in thread-local arrays
- Retire lists per thread
- Global registry of all hazard pointer arrays

### Cache Line Padding
```cpp
struct HazardPointer {
    std::atomic<void*> ptr;
    char padding[64 - sizeof(std::atomic<void*>)]; // 64-byte cache line
};
```

### NUMA Considerations
- Optional NUMA-aware allocator
- Thread pinning for performance-critical applications
- Localized memory allocation

### Contention Reduction
- Backoff strategy for CAS failures
- Separate hazard pointer arrays per core
- Queue partitioning for very high concurrency

### Scalability Architecture
- Designed for up to 64 threads
- Lock-free at all levels
- No global locks or mutexes

## Task 3.4: Testing Architecture

### Unit Test Framework
- Google Test framework
- Test cases for all public methods
- Edge case testing

### Property-Based Testing
- Hypothesis for C++ (or custom implementation)
- Properties:
  1. FIFO ordering
  2. No lost elements
  3. Size consistency
  4. Memory safety

### Stress Test Harness
- Long-running tests (24+ hours)
- Randomized thread operations
- Memory leak detection
- Sanitizer integration (ASan, TSan)

### Performance Benchmark Suite
- Throughput measurements
- Latency percentiles
- Scalability graphs
- Comparison with std::queue + mutex

### Sanitizer Integration
```cmake
# Enable sanitizers in debug build
option(ENABLE_SANITIZERS "Enable address and thread sanitizers" ON)
if(ENABLE_SANITIZERS)
    target_compile_options(lockfree-queue_tests PRIVATE -fsanitize=address,thread)
    target_link_options(lockfree-queue_tests PRIVATE -fsanitize=address,thread)
endif()