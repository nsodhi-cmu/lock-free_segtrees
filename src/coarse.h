#ifndef COARSE_SEGTREE_H
#define COARSE_SEGTREE_H

#include <vector>
#include <mutex>

#include "segment_tree.h"

class CoarseSegmentTree : public SegmentTree {
    struct Node {
        int value;
        int update;
    };
    std::mutex mux;
    int size;
    public: Node *tree;
    
    public:
        explicit CoarseSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int));
        ~CoarseSegmentTree();
        void build(const std::vector<int> &data);
        int range_query(int lower, int upper);
        void range_update(int lower, int upper, int value);
    
    private:
        void build(const std::vector<int> &data, int i, int lo, int hi);
        int range_query(int l, int r, int i, int lo, int hi);
        void range_update(int l, int r, int val, int i, int lo, int hi);
};

#endif