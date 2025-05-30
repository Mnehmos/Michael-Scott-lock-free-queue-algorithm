# Michael-Scott Lock-Free Queue Implementation

This project provides a C++ implementation of the Michael-Scott lock-free queue algorithm, featuring:
- Hazard pointer-based memory reclamation
- Atomic operations with ABA prevention
- Comprehensive test suite

## Prerequisites
- C++17 compiler
- CMake (version 3.10 or higher)
- MSYS2 (for Windows builds)

## Building
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running Tests
```bash
# Run basic tests
./tests/basic_tests

# Run minimal test
./minimal_test

# Run manual tests
./manual_test
```

## Key Components
- `implementation/include/lockfree/msqueue.hpp`: Main queue implementation
- `implementation/src/atomic_wrappers.hpp`: Atomic operation wrappers
- `implementation/src/hazard_pointer.hpp`: Hazard pointer manager
- `implementation/src/node.hpp`: Queue node structure
- `implementation/tests/`: Test suite

## Demo Programs
- `minimal_test.cpp`: Basic functionality test
- `queue_demo.cpp`: Extended functionality demo
- `manual_test.cpp`: Interactive test program

## Documentation
See the `design/` directory for architectural documentation and pseudocode.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.