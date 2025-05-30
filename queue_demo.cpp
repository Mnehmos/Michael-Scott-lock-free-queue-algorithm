#include <iostream>
#include <limits> // Required for std::numeric_limits
#include "lockfree/msqueue.hpp"

int main() {
    lockfree::msqueue::Queue<int> queue;
    int choice = 0;
    int value = 0;
    
    std::cout << "=== Lock-Free Queue Interactive Demo ===\n";
    std::cout << "1. Enqueue\n";
    std::cout << "2. Dequeue\n";
    std::cout << "3. Check if empty\n";
    std::cout << "4. Exit\n";
    
    while (choice != 4) {
        std::cout << "\nEnter choice: ";
        std::cin >> choice;
        
        // Clear input buffer after reading choice to prevent issues with subsequent std::cin operations
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch(choice) {
            case 1:
                std::cout << "Enter value to enqueue: ";
                std::cin >> value;
                queue.enqueue(value);
                std::cout << "Enqueued: " << value << "\n";
                break;
                
            case 2:
                if (queue.dequeue(value)) {
                    std::cout << "Dequeued: " << value << "\n";
                } else {
                    std::cout << "Queue is empty!\n";
                }
                break;
                
            case 3:
                if (queue.is_empty()) { // Using the correct method name from msqueue.hpp
                    std::cout << "Queue is empty\n";
                } else {
                    std::cout << "Queue is NOT empty\n";
                }
                break;
                
            case 4:
                std::cout << "Exiting...\n";
                break;
                
            default:
                std::cout << "Invalid choice!\n";
        }
    }
    
    // Pause before exiting to allow user to see output
    std::cout << "Press Enter to exit...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear any remaining input
    return 0;
}