#include "fine.h"

FineSegmentTree::FineSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int)) {
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;
    this->size = size;
    this->tree = new Node[2 * this->size]();
    this->base = base;
    this->func = func;
    this->batch_func = batch_func;
}

FineSegmentTree::~FineSegmentTree() {
    delete[] tree;
}

void FineSegmentTree::build(const std::vector<int> &data) {
    build(data, 1, 0, size - 1);
}

void FineSegmentTree::build(const std::vector<int> &data, int i, int lo, int hi) {
    if (lo == hi) {
        if ((size_t)lo < data.size()) {
            tree[i].value = data[lo];
        } else {
            tree[i].value = base;
        }
        return;
    }
    int mid = SEG_MIDPOINT(lo, hi);
    build(data, L_INDEX(i), lo, mid);
    build(data, R_INDEX(i), mid + 1, hi);
    tree[i].value = func(tree[L_INDEX(i)].value, tree[R_INDEX(i)].value);
}

int FineSegmentTree::range_query(int l, int r) {
    tree[1].mux.lock();
    return range_query(l, r, 1, 0, size - 1);
}

int FineSegmentTree::range_query(int l, int r, int i, int lo, int hi) {
    if (SEG_DISJOINT(l, r, lo, hi)) {
        tree[i].mux.unlock();
        return base;
    }
    lazy_propagate(i, lo, hi);
    if (SEG_CONTAINS(l, r, lo, hi)) {
        int v = tree[i].value;
        tree[i].mux.unlock();
        return v;
    }
    int mid = SEG_MIDPOINT(lo, hi);
    int left = L_INDEX(i);
    int right = R_INDEX(i);
    tree[left].mux.lock();
    tree[right].mux.lock();
    tree[i].mux.unlock();
    int lq = range_query(l, r, L_INDEX(i), lo, mid);
    int rq = range_query(l, r, R_INDEX(i), mid + 1, hi);
    return func(lq, rq);
}

void FineSegmentTree::range_update(int l, int r, int val) {
}

void FineSegmentTree::range_update(int l, int r, int val, int i, int lo, int hi) {
}

void FineSegmentTree::lazy_propagate(int i, int lo, int hi) {
    if (tree[i].update == base) return;
    int size = hi - lo + 1;
    tree[i].value = batch_func(tree[i].value, size, tree[i].update);
    if (size > 1) {
        tree[L_INDEX(i)].update = func(tree[L_INDEX(i)].update, tree[i].update);
        tree[R_INDEX(i)].update = func(tree[R_INDEX(i)].update, tree[i].update);
    }
    tree[i].update = base;
}