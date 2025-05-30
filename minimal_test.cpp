#include <iostream>
#include "minimal_queue.hpp"

int main() {
    lockfree::msqueue::Queue<int> queue;
    
    // Test enqueue
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    
    // Test dequeue
    int value;
    while (queue.dequeue(value)) {
        std::cout << "Dequeued: " << value << std::endl;
    }
    
    std::cout << "Minimal test completed successfully!" << std::endl;
    return 0;
}