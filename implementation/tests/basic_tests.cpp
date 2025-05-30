#include "../include/lockfree/msqueue.hpp"
#include <gtest/gtest.h>

TEST(MSQueueTest, ConstructorCreatesDummyNode) {
    lockfree::msqueue::Queue<int> queue;
    EXPECT_FALSE(queue.is_empty()); // Dummy node means not technically empty
}

TEST(MSQueueTest, EnqueueSingleItem) {
    lockfree::msqueue::Queue<int> queue;
    EXPECT_TRUE(queue.enqueue(42));
    EXPECT_FALSE(queue.is_empty());
}

TEST(MSQueueTest, DequeueSingleItem) {
    lockfree::msqueue::Queue<int> queue;
    queue.enqueue(42);
    int result;
    EXPECT_TRUE(queue.dequeue(result));
    EXPECT_EQ(42, result);
    EXPECT_TRUE(queue.is_empty());
}

TEST(MSQueueTest, DequeueFromEmpty) {
    lockfree::msqueue::Queue<int> queue;
    int result;
    EXPECT_FALSE(queue.dequeue(result));
}

TEST(MSQueueTest, EnqueueDequeueSequence) {
    lockfree::msqueue::Queue<int> queue;
    for (int i = 0; i < 100; i++) {
        queue.enqueue(i);
    }
    int result;
    for (int i = 0; i < 100; i++) {
        EXPECT_TRUE(queue.dequeue(result));
        EXPECT_EQ(i, result);
    }
    EXPECT_TRUE(queue.is_empty());
}
