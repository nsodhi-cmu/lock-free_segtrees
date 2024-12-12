#include "concurrent_stack.h"

void ConcurrentStack::push(int data) {
    Node* node = new Node(data);
    TaggedNode prevHead;

    do {
        prevHead = head.load(std::memory_order_acquire);
        node->next = prevHead.node;
    } while (!head.compare_exchange_weak(
        prevHead,
        TaggedNode(node, prevHead.tag + 1),
        std::memory_order_release,
        std::memory_order_relaxed)
        );
}

int ConcurrentStack::pop() {
    TaggedNode prevHead;

    do {
        prevHead = head.load(std::memory_order_acquire);
        if (prevHead.node == nullptr) return -1;
    } while (!head.compare_exchange_weak(
        prevHead,
        TaggedNode(prevHead.node->next, prevHead.tag + 1),
        std::memory_order_release,
        std::memory_order_relaxed)
        );

    int data = prevHead.node->data;
    delete prevHead.node;
    return data;
}