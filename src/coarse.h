#ifndef COARSE_SEGTREE_H
#define COARSE_SEGTREE_H

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>

#include "segment_tree.h"

class CoarseSegmentTree : public SegmentTree {
    struct Node {
        int value;
        int update;
    };

    pthread_mutex_t mux;
    int size;
    Node *tree;
    
    public:
        explicit CoarseSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int));
        ~CoarseSegmentTree() override;
        void build(const std::vector<int> &data) override;
        int range_query(int lower, int upper) override;
        void range_update(int lower, int upper, int value) override;
        void print() override;
    
    private:
        void build(const std::vector<int> &data, int i, int lo, int hi);
        int range_query(int l, int r, int i, int lo, int hi);
        void range_update(int l, int r, int val, int i, int lo, int hi);
        void print(int i, const std::string &pref, bool last);
};

#endif