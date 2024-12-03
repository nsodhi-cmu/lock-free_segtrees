#ifndef FINE_SEGTREE_H
#define FINE_SEGTREE_H

#include <vector>
#include <mutex>

#include "segment_tree.h"

class FineSegmentTree : public SegmentTree {
    struct Node {
        std::mutex mux;
        int value;
        int update;
    };
    int size;
    public: Node *tree;
    
    public:
        explicit FineSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int));
        ~FineSegmentTree();
        void build(const std::vector<int> &data); // Not thread safe!
        int range_query(int lower, int upper);
        void range_update(int lower, int upper, int value);
    
    private:
        void build(const std::vector<int> &data, int i, int lo, int hi);
};

#endif