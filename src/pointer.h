#ifndef POINTER_SEGTREE_H
#define POINTER_SEGTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>

#include "segment_tree.h"

class PointerSegmentTree : public SegmentTree {
    struct Node {
        int value;
        int update;
        Node *left;
        Node *right;
    };

    pthread_mutex_t mux;
    int size;
    Node *tree;
    
    public:
        explicit PointerSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int));
        ~PointerSegmentTree() override;
        void build(const std::vector<int> &data) override;
        int range_query(int lower, int upper) override;
        void range_update(int lower, int upper, int value) override;
        void print() override;
    
    private:
        void build(const std::vector<int> &data, Node *curr, int lo, int hi);
        Node *new_node(int len);
        int range_query(int l, int r, Node *curr, int lo, int hi);
        void range_update(int l, int r, int val, Node *curr, int lo, int hi);
        void print(Node *curr, const std::string &pref, bool last);
};

#endif