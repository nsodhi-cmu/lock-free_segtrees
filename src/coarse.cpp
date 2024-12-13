#include "coarse.h"


CoarseSegmentTree::CoarseSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int)) {
    this->size = NEXT_POW_TWO(size);
    this->tree = new Node[2 * this->size]();
    this->base = base;
    this->func = func;
    this->batch_func = batch_func;
    pthread_mutex_init(&this->mux, nullptr);
}

CoarseSegmentTree::~CoarseSegmentTree() {
    pthread_mutex_destroy(&this->mux);
    delete[] tree;
}

void CoarseSegmentTree::build(const std::vector<int> &data) {
    pthread_mutex_lock(&this->mux);
    build(data, 1, 0, size - 1);
    pthread_mutex_unlock(&this->mux);
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
    build(data, L_INDEX(i), lo, mid);
    build(data, R_INDEX(i), mid + 1, hi);
    tree[i].value = func(tree[L_INDEX(i)].value, tree[R_INDEX(i)].value);
}

int CoarseSegmentTree::range_query(int l, int r) {
    pthread_mutex_lock(&this->mux);
    int result = range_query(l, r, 1, 0, size - 1);
    pthread_mutex_unlock(&this->mux);
    return result;
}

int CoarseSegmentTree::range_query(int l, int r, int i, int lo, int hi) {
    if (SEG_DISJOINT(l, r, lo, hi)) return base;
    if (tree[i].update != base) { // lazy propagate
        int size = hi - lo + 1;
        tree[i].value = batch_func(tree[i].value, size, tree[i].update);
        if (size > 1) {
            tree[L_INDEX(i)].update = func(tree[L_INDEX(i)].update, tree[i].update);
            tree[R_INDEX(i)].update = func(tree[R_INDEX(i)].update, tree[i].update);
        }
        tree[i].update = base;
    }
    if (SEG_CONTAINS(l, r, lo, hi)) return tree[i].value;
    int mid = SEG_MIDPOINT(lo, hi);
    return func(range_query(l, r, L_INDEX(i), lo, mid), range_query(l, r, R_INDEX(i), mid + 1, hi));
}

void CoarseSegmentTree::range_update(int l, int r, int val) {
    pthread_mutex_lock(&this->mux);
    range_update(l, r, val, 1, 0, size - 1);
    pthread_mutex_unlock(&this->mux);
}

void CoarseSegmentTree::range_update(int l, int r, int val, int i, int lo, int hi) {
    if (SEG_DISJOINT(l, r, lo, hi)) return;
    if (SEG_CONTAINS(l, r, lo, hi)) {
        tree[i].update = func(tree[i].update, val);
        return;
    }
    int intersection = std::min(r, hi) - std::max(l, lo) + 1;
    tree[i].value = batch_func(tree[i].value, intersection, val);
    int mid = SEG_MIDPOINT(lo, hi);
    range_update(l, r, val, L_INDEX(i), lo, mid);
    range_update(l, r, val, R_INDEX(i), mid + 1, hi);
}

void CoarseSegmentTree::print() {
    pthread_mutex_lock(&this->mux);
    print(1, "", true);
    pthread_mutex_unlock(&this->mux);
}

void CoarseSegmentTree::print(int i, const std::string &pref, bool last) {
    Node &curr = tree[i];
    std::cout << pref << (last ? "└── " : "├── ") << "(" << curr.value << ", " << curr.update << ")" << std::endl;
    std::string next_pref = pref + (last ? "    " : "│   ");
    if (i < size) { // Not a child
        print(L_INDEX(i), next_pref, false);
        print(R_INDEX(i), next_pref, true);
    }
}