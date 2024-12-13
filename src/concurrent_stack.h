#ifndef CONCURRENT_STACK_H
#define CONCURRENT_STACK_H

#include <atomic>

class ConcurrentStack {
    private:
        struct Node {
            int data;
            Node* next;
            Node(int value) : data(value), next(nullptr) {}
        };

        struct TaggedNode {
            Node *node;
            int tag;
            TaggedNode() : node(nullptr), tag(0) {}
            TaggedNode(Node* n, uint64_t t) : node(n), tag(t) {}

            bool operator==(const TaggedNode& other) const {
                return node == other.node && tag == other.tag;
            }
        };

        std::atomic<TaggedNode> head;

    public:
        ConcurrentStack() : head(TaggedNode()) {}
        void push(int data);
        int pop();
        ~ConcurrentStack();
};

#endif