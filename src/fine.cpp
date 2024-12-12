#include "fine.h"

FineSegmentTree::FineSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int)) {
    this->size = NEXT_POW_TWO(size);
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

int FineSegmentTree::range_query(int l, int r) {
    std::queue<std::tuple<int, int, int>> q;
    int result = base;
    tree[1].mux.lock();
    q.push({1, 0, size - 1});
    while (!q.empty()) {
        std::tuple<int, int, int> curr = q.front();
        q.pop();
        int i = std::get<0>(curr);
        int lo = std::get<1>(curr);
        int hi = std::get<2>(curr);
        int size = hi - lo + 1;
        if (SEG_CONTAINS(l, r, lo, hi)) {
            if (size > 1) {
                int v = batch_func(tree[i].value, size, tree[i].update);
                result = func(result, v);
            } else {
                tree[i].value = func(tree[i].value, tree[i].update);
                result = func(result, tree[i].value);
                tree[i].update = base;
            }
        } else if (!(SEG_DISJOINT(l, r, lo, hi))){
            tree[i].value = batch_func(tree[i].value, size, tree[i].update);
            tree[L_INDEX(i)].mux.lock();
            tree[R_INDEX(i)].mux.lock();
            tree[L_INDEX(i)].update = func(tree[L_INDEX(i)].update, tree[i].update);
            tree[R_INDEX(i)].update = func(tree[R_INDEX(i)].update, tree[i].update);
            tree[i].update = base;
            int mid = SEG_MIDPOINT(lo, hi);
            q.push({L_INDEX(i), lo, mid});
            q.push({R_INDEX(i), mid + 1, hi});
        }
        tree[i].mux.unlock();
    }
    return result;
}

void FineSegmentTree::range_update(int l, int r, int val) {
    std::queue<std::tuple<int, int, int>> q;
    tree[1].mux.lock();
    q.push({1, 0, size - 1});
    while (!q.empty()) {
        std::tuple<int, int, int> curr = q.front();
        q.pop();
        int i = std::get<0>(curr);
        int lo = std::get<1>(curr);
        int hi = std::get<2>(curr);
        if (SEG_CONTAINS(l, r, lo, hi)) {
            tree[i].update = func(tree[i].update, val);
        } else if (!(SEG_DISJOINT(l, r, lo, hi))){
            int intersection = std::min(r, hi) - std::max(l, lo) + 1;
            tree[i].value = batch_func(tree[i].value, intersection, val);
            tree[L_INDEX(i)].mux.lock();
            tree[R_INDEX(i)].mux.lock();
            int mid = SEG_MIDPOINT(lo, hi);
            q.push({L_INDEX(i), lo, mid});
            q.push({R_INDEX(i), mid + 1, hi});
        }
        tree[i].mux.unlock();
    }
}

void FineSegmentTree::print() {
    print(1, "", true);
}

void FineSegmentTree::print(int i, const std::string &pref, bool last) {
    Node &curr = tree[i];
    std::cout << pref << (last ? "└── " : "├── ") << "(" << curr.value << ", " << curr.update << ")" << std::endl;
    std::string next_pref = pref + (last ? "    " : "│   ");
    if (i < size) { // Not a child
        print(L_INDEX(i), next_pref, false);
        print(R_INDEX(i), next_pref, true);
    }
}