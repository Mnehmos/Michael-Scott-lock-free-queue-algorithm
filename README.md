# Michael-Scott Lock-Free Queue Implementation

This project implements the Michael-Scott lock-free queue algorithm with hazard pointers for safe memory reclamation. The implementation is written in C++ and includes comprehensive tests and demos.

## Features
- Lock-free enqueue and dequeue operations
- Hazard pointers for safe memory reclamation
- Comprehensive test suite
- Interactive queue demo
- Minimal test for quick verification

## Requirements
- MSYS2 (Windows)
- g++ compiler
- Google Test framework (included in MSYS2)

## Build and Run
```bash
# Install dependencies
.\setup_msys2.ps1

# Build and run all tests
.\build_and_run.bat
```

## Tests
1. **Basic Tests**: Comprehensive tests using Google Test framework
   ```text
   [==========] Running 5 tests from 1 test suite.
   [----------] Global test environment set-up.
   [ RUN      ] MSQueueTest.ConstructorCreatesDummyNode
   [       OK ] MSQueueTest.ConstructorCreatesDummyNode (0 ms)
   [ RUN      ] MSQueueTest.EnqueueSingleItem
   [       OK ] MSQueueTest.EnqueueSingleItem (0 ms)
   [ RUN      ] MSQueueTest.DequeueSingleItem
   [       OK ] MSQueueTest.DequeueSingleItem (0 ms)
   [ RUN      ] MSQueueTest.DequeueFromEmpty
   [       OK ] MSQueueTest.DequeueFromEmpty (0 ms)
   [ RUN      ] MSQueueTest.EnqueueDequeueSequence
   [       OK ] MSQueueTest.EnqueueDequeueSequence (0 ms)
   [----------] 5 tests from MSQueueTest (0 ms total)
   [  PASSED  ] 5 tests.
   ```

2. **Manual Test**: Basic queue operations
   ```text
   Dequeued: 1
   Dequeued: 2
   Dequeued: 3
   Test complete!
   ```

3. **Minimal Test**: Simple queue operations
   ```text
   Dequeued: 1
   Dequeued: 2
   Dequeued: 3
   Minimal test completed successfully!
   ```

4. **Interactive Demo**: Run queue_demo.exe to interact with the queue

## Implementation Details
- Uses hazard pointers for safe memory reclamation
- Header-only implementation (msqueue.hpp)
- Atomic operations for thread safety
- Cache line padding to prevent false sharing

## Files
- `implementation/include/lockfree/msqueue.hpp` - Main queue implementation
- `implementation/src/hazard_pointer.hpp` - Hazard pointer manager
- `implementation/tests/basic_tests.cpp` - Comprehensive tests
- `manual_test.cpp` - Basic manual test
- `queue_demo.cpp` - Interactive demo
- `minimal_queue.hpp` - Minimal queue implementation for comparison
- `minimal_test.cpp` - Minimal queue test

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.