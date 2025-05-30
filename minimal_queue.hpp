#pragma once
#include <atomic>
#include <iostream>

namespace lockfree::msqueue {

template <typename T>
class Queue {
public:
    Queue() {
        Node* dummy = new Node();
        head = dummy;
        tail = dummy;
    }

    ~Queue() {
        Node* curr = head;
        while (curr) {
            Node* next = curr->next.load();
            delete curr;
            curr = next;
        }
    }

    void enqueue(T data) {
        Node* node = new Node(data);
        while (true) {
            Node* last = tail.load();
            Node* next = last->next.load();
            if (last != tail) continue;
            if (next == nullptr) {
                if (last->next.compare_exchange_weak(next, node)) {
                    tail.compare_exchange_weak(last, node);
                    return;
                }
            } else {
                tail.compare_exchange_weak(last, next);
            }
        }
    }

    bool dequeue(T& result) {
        while (true) {
            Node* first = head.load();
            Node* last = tail.load();
            Node* next = first->next.load();
            if (first != head) continue;
            if (first == last) {
                if (next == nullptr) return false;
                tail.compare_exchange_weak(last, next);
            } else {
                result = next->data;
                if (head.compare_exchange_weak(first, next)) {
                    delete first;
                    return true;
                }
            }
        }
    }

private:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node(T val = T()) : data(val), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
};

} // namespace lockfree::msqueue