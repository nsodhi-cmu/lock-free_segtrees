#ifndef FINE_SEGTREE_H
#define FINE_SEGTREE_H

#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <pthread.h>

#include "segment_tree.h"

class FineSegmentTree : public SegmentTree {
    struct Node {
        pthread_mutex_t mux;
        int value;
        int update;
    };

    struct QueueObj {
        int i;
        int lo;
        int hi;
        QueueObj() : i(-1), lo(-1), hi(-1) {}
        QueueObj(int i_, int lo_, int hi_) : i(i_), lo(lo_), hi(hi_) {}
    };

    int size;
    Node *tree;
    
    public:
        explicit FineSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int));
        ~FineSegmentTree();
        void build(const std::vector<int> &data); // Not thread safe!
        int range_query(int lower, int upper);
        void range_update(int lower, int upper, int value);
        void print(); // Not thread safe!
    
    private:
        void build(const std::vector<int> &data, int i, int lo, int hi);
        void print(int i, const std::string &pref, bool last);
};

#endif