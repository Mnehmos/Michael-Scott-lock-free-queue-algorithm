#include <iostream>
#include "lockfree/msqueue.hpp"

int main() {
    lockfree::msqueue::Queue<int> queue;
    
    // Simple test
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    
    int value;
    while (queue.dequeue(value)) {
        std::cout << "Dequeued: " << value << std::endl;
    }
    
    std::cout << "Test complete! Press Enter to exit...";
    std::cin.ignore();
    return 0;
}