#include "pointer.h"


PointerSegmentTree::PointerSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int)) {
    this->size = NEXT_POW_TWO(size);
    this->tree = new_node(this->size);
    this->base = base;
    this->func = func;
    this->batch_func = batch_func;
    pthread_mutex_init(&this->mux, nullptr);
}

PointerSegmentTree::~PointerSegmentTree() {
    pthread_mutex_destroy(&this->mux);
    delete[] tree;
}

PointerSegmentTree::Node *PointerSegmentTree::new_node(int len) {
    Node *myself = new Node;
    if (len <= 1) {
        myself->left = nullptr;
        myself->right = nullptr;
    } else {
        myself->left = new_node(len / 2);
        myself->right = new_node(len / 2);
    }
    return myself;
}

void PointerSegmentTree::build(const std::vector<int> &data) {
    pthread_mutex_lock(&this->mux);
    build(data, this->tree, 0, size - 1);
    pthread_mutex_unlock(&this->mux);
}

void PointerSegmentTree::build(const std::vector<int> &data, Node *curr, int idx, int len) {
    if (len <= 1) {
        if (idx < data.size()) {
            curr->value = data[idx];
        } else {
            curr->value = base;
        }
    } else {
        build(data, curr->left, idx, len / 2);
        build(data, curr->right, idx + len / 2, len / 2);
        curr->value = func(curr->left->value, curr->right->value);
    }
    curr->update = base;
}

int PointerSegmentTree::range_query(int l, int r) {
    pthread_mutex_lock(&this->mux);
    int result = range_query(l, r, tree, 0, size - 1);
    pthread_mutex_unlock(&this->mux);
    return result;
}

int PointerSegmentTree::range_query(int l, int r, Node *curr, int lo, int hi) {
    if (SEG_DISJOINT(l, r, lo, hi)) return base;
    if (curr->update != base) { // lazy propagate
        int size = hi - lo + 1;
        curr->value = batch_func(curr->value, size, curr->update);
        if (size > 1) {
            curr->left->update = func(curr->left->update, curr->update);
            curr->right->update = func(curr->right->update, curr->update);
        }
        curr->update = base;
    }
    if (SEG_CONTAINS(l, r, lo, hi)) return curr->value;
    int mid = SEG_MIDPOINT(lo, hi);
    return func(range_query(l, r, curr->left, lo, mid), range_query(l, r, curr->right, mid + 1, hi));
}

void PointerSegmentTree::range_update(int l, int r, int val) {
    pthread_mutex_lock(&this->mux);
    range_update(l, r, val, tree, 0, size - 1);
    pthread_mutex_unlock(&this->mux);
}

void PointerSegmentTree::range_update(int l, int r, int val, Node *curr, int lo, int hi) {
    if (SEG_DISJOINT(l, r, lo, hi)) return;
    if (SEG_CONTAINS(l, r, lo, hi)) {
        curr->update = func(curr->update, val);
        return;
    }
    int intersection = std::min(r, hi) - std::max(l, lo) + 1;
    curr->value = batch_func(curr->value, intersection, val);
    int mid = SEG_MIDPOINT(lo, hi);
    range_update(l, r, val, curr->left, lo, mid);
    range_update(l, r, val, curr->right, mid + 1, hi);
}

void PointerSegmentTree::print() {
    pthread_mutex_lock(&this->mux);
    print(tree, "", true);
    pthread_mutex_unlock(&this->mux);
}

void PointerSegmentTree::print(Node *curr, const std::string &pref, bool last) {
    std::cout << pref << (last ? "└── " : "├── ") << "(" << curr->value << ", " << curr->update << ")" << std::endl;
    std::string next_pref = pref + (last ? "    " : "│   ");
    if (curr->left != nullptr) {
        print(curr->left, next_pref, false);
    }
    if (curr->right != nullptr) {
        print(curr->right, next_pref, true);
    }
}