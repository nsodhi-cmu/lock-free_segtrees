#include "SegmentTree.h"

SegmentTree::SegmentTree(const int size) {
    this->size = size;
    this->tree = new Node[4 * size]();
}

SegmentTree::~SegmentTree() {
    delete[] tree;
}

void SegmentTree::build(const std::vector<int> &data) {
    build(data, 1, 0, size - 1);
}

void SegmentTree::build(const std::vector<int> &data, int i, int lo, int hi) {
    if (lo == hi) {
        tree[i].sum = data[lo];
        return;
    }
    int mid = lo + (hi - lo) / 2;
    build(data, L_INDEX(i), lo, mid);
    build(data, R_INDEX(i), mid + 1, hi);
    tree[i].sum = tree[L_INDEX(i)].sum + tree[R_INDEX(i)].sum;
}

void SegmentTree::range_update_add(int l, int r, int val, int i, int lo, int hi) {
    propagate_update_add(i, lo, hi);
    if (SEG_DISJOINT(l, r, lo, hi)) return;
    if (SEG_CONTAINS(l, r, lo, hi)) {
        tree[i].lazy_add += val;
        propagate_update_add(i, lo, hi);
        return;
    }
    int mid = lo + (hi - lo) / 2;
    range_update_add(l, r, val, L_INDEX(i), lo, mid);
    range_update_add(l, r, val, R_INDEX(i), mid + 1, hi);
    tree[i].sum = tree[L_INDEX(i)].sum + tree[R_INDEX(i)].sum;
}

void SegmentTree::range_update_add(int l, int r, int val) {
    range_update_add(l, r, val, 1, 0, size - 1);
}

int SegmentTree::range_query(int l, int r) {
    return range_query(l, r, 1, 0, size - 1);
}

int SegmentTree::range_query(int l, int r, int i, int lo, int hi) {
    propagate_update_add(i, lo, hi);
    if (SEG_DISJOINT(l, r, lo, hi)) return 0;
    if (SEG_CONTAINS(l, r, lo, hi)) return tree[i].sum;
    int mid = lo + (hi - lo) / 2;
    return range_query(l, r, L_INDEX(i), lo, mid) + range_query(l, r, R_INDEX(i), mid + 1, hi);
}

void SegmentTree::propagate_update_add(int i, int lo, int hi) {
    if (tree[i].lazy_add == 0) return;
    int size = hi - lo + 1;
    tree[i].sum += size * tree[i].lazy_add;
    tree[L_INDEX(i)].lazy_add += tree[i].lazy_add;
    tree[R_INDEX(i)].lazy_add += tree[i].lazy_add;
    tree[i].lazy_add = 0;
}
