#ifndef LF_SEGTREE_H
#define LF_SEGTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <atomic>

#include "segment_tree.h"
#include "concurrent_stack.h"

class LFSegmentTree : public SegmentTree {
    struct Head {
        int index;
        int ticket;
        Head() : index(-1), ticket(-1) {}
        Head(int i, int t) : index(i), ticket(t) {}

        bool operator==(const Head& other) const {
            return index == other.index && ticket == other.ticket;
        }
    };

    struct Node {
        int value;
        int update;
        Node *left;
        Node *right;
        Node* swap_left;
        Node* swap_right;
    };

    int size;
    int num_threads;
    Node **shadows;
    std::atomic<int> *refcount;

    std::atomic<Head> head;
    ConcurrentStack freeList;

    public:
        explicit LFSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int), int num_threads);
        ~LFSegmentTree();
        void build(const std::vector<int> &data); // Not thread safe!
        int range_query(int lower, int upper);
        void range_update(int lower, int upper, int value);
        void print(); // Not thread safe!
    
    private:
        Node *new_node(int len);
        void delete_node(Node* curr);
        void build(const std::vector<int> &data, Node *curr, int idx, int len);
        int range_query(int lower, int upper, Node *curr, int lo, int hi);
        Node *range_update(int l, int r, int val, Node *curr, Node *old, int lo, int hi, std::vector<Node*> &traversal, int &vidx, Head exp);
        Node *swap_pointers(int l, int r, Node *old, int lo, int hi, std::vector<Node*> &traversal, int &vidx);
        void print(Node *curr, const std::string &pref, bool last);
};

#endif