# Manual Testing Procedure

## Test Environment Setup
1. Include the required headers in your project:
   ```cpp
   #include "lockfree/msqueue.hpp"
   ```

## Basic Functionality Tests

### Test 1: Single Thread Enqueue/Dequeue
```cpp
lockfree::msqueue::Queue<int> queue;
int value;

// Test enqueue
queue.enqueue(42);

// Test dequeue
queue.dequeue(value);
assert(value == 42);

// Test empty state
assert(queue.is_empty());
```

### Test 2: Concurrency Test
```cpp
#include <thread>
#include <vector>

lockfree::msqueue::Queue<int> queue;
const int NUM_THREADS = 4;
const int OPS_PER_THREAD = 1000;

void producer() {
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        queue.enqueue(i);
    }
}

void consumer(std::atomic<int>& sum) {
    int value;
    for (int i = 0; i < OPS_PER_THREAD; i++) {
        if (queue.dequeue(value)) {
            sum += value;
        }
    }
}

// Create producer threads
std::vector<std::thread> producers;
for (int i = 0; i < NUM_THREADS; i++) {
    producers.emplace_back(producer);
}

// Create consumer threads
std::atomic<int> total_sum{0};
std::vector<std::thread> consumers;
for (int i = 0; i < NUM_THREADS; i++) {
    consumers.emplace_back(consumer, std::ref(total_sum));
}

// Wait for completion
for (auto& t : producers) t.join();
for (auto& t : consumers) t.join();

// Verify results
int expected = NUM_THREADS * (OPS_PER_THREAD - 1) * OPS_PER_THREAD / 2;
assert(total_sum == expected);
assert(queue.is_empty());
```

### Test 3: Memory Leak Check
```cpp
// Run valgrind or AddressSanitizer
// On Windows, use Visual Studio Memory Profiler
```

## Expected Behavior
1. Enqueue operations should never lose data
2. Dequeue operations should return items in FIFO order
3. Queue should maintain consistency under high concurrency
4. No memory leaks should occur during operation