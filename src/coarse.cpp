#include "coarse.h"

#include <bit>

CoarseSegmentTree::CoarseSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int)) {
    this->size = std::bit_ceil(size);
    this->tree = new Node[2 * this->size]();
    this->base = base;
    this->func = func;
    this->batch_func = batch_func;
}

CoarseSegmentTree::~CoarseSegmentTree() {
    delete[] tree;
}

void CoarseSegmentTree::build(const std::vector<int> &data) {
    mux.lock();
    build(data, 1, 0, size - 1);
    mux.unlock();
}

void CoarseSegmentTree::build(const std::vector<int> &data, int i, int lo, int hi) {
    if (lo == hi) {
        if (lo < data.size()) {
            tree[i].value = data[lo];
        } else {
            tree[i].value = base;
        }
        return;
    }
    int mid = SEG_MIDPOINT(lo, hi);
    int left = L_INDEX(i);
    int right = R_INDEX(i);
    build(data, left, lo, mid);
    build(data, right, mid + 1, hi);
    tree[i].value = tree[left].value + tree[right].value;
}

int CoarseSegmentTree::range_query(int l, int r) {
    mux.lock();
    int result = range_query(l, r, 1, 0, size - 1);
    mux.unlock();
    return result;
}

int CoarseSegmentTree::range_query(int l, int r, int i, int lo, int hi) {
    lazy_propagate(i, lo, hi);
    if (SEG_DISJOINT(l, r, lo, hi)) return base;
    if (SEG_CONTAINS(l, r, lo, hi)) return tree[i].value;
    int mid = SEG_MIDPOINT(lo, hi);
    return func(range_query(l, r, L_INDEX(i), lo, mid), range_query(l, r, R_INDEX(i), mid + 1, hi));
}

void CoarseSegmentTree::range_update(int l, int r, int val) {
    mux.lock();
    range_update(l, r, val, 1, 0, size - 1);
    mux.unlock();
}

void CoarseSegmentTree::range_update(int l, int r, int val, int i, int lo, int hi) {
    lazy_propagate(i, lo, hi);
    if (SEG_DISJOINT(l, r, lo, hi)) return;
    if (SEG_CONTAINS(l, r, lo, hi)) {
        tree[i].update = func(tree[i].update, val);
        lazy_propagate(i, lo, hi);
        return;
    }
    int mid = SEG_MIDPOINT(lo, hi);
    range_update(l, r, val, L_INDEX(i), lo, mid);
    range_update(l, r, val, R_INDEX(i), mid + 1, hi);
    tree[i].value = tree[L_INDEX(i)].value + tree[R_INDEX(i)].value;
}

void CoarseSegmentTree::lazy_propagate(int i, int lo, int hi) {
    if (tree[i].update == base) return;
    int size = hi - lo + 1;
    tree[i].value = batch_func(tree[i].value, size, tree[i].update);
    if (size > 1) {
        tree[L_INDEX(i)].update = func(tree[L_INDEX(i)].update, tree[i].update);
        tree[R_INDEX(i)].update = func(tree[R_INDEX(i)].update, tree[i].update);
    }
    tree[i].update = base;
}