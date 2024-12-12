#include "lock-free.h"

LFSegmentTree::LFSegmentTree(int size, int base, int (*func)(int, int), int (*batch_func)(int, int, int), int num_threads) {
    this->size = NEXT_POW_TWO(size);
    this->base = base;
    this->func = func;
    this->batch_func = batch_func;
    this->num_threads = num_threads;
    shadows = new Node*[num_threads + 1];
    refcount = new std::atomic<int>[num_threads + 1];

    for (int i = 0; i < num_threads + 1; i++) {
        this->nid_ctr = (i + 1) * 100;

        shadows[i] = new_node(this->size);
        refcount[i].store(INT_MIN);
        freeList.push(i);
    }
}

LFSegmentTree::Node *LFSegmentTree::new_node(int len) {
    Node *myself = new Node;
    myself->nid = ++nid_ctr;
    if (len <= 1) {
        myself->left = nullptr;
        myself->right = nullptr;
        myself->swap_left = nullptr;
        myself->swap_right = nullptr;
    } else {
        myself->left = new_node(len / 2);
        myself->right = new_node(len / 2);
        myself->swap_left = myself->left;
        myself->swap_right = myself->right;
    }
    return myself;
}

LFSegmentTree::~LFSegmentTree() {
    for (int i = 0; i < num_threads + 1; i++) {
        delete_node(shadows[i]);
    }
    delete[] shadows;
}

void LFSegmentTree::delete_node(Node* curr) {
    if (curr->left != nullptr) {
        delete_node(curr->swap_left);
    }
    if (curr->right != nullptr) {
        delete_node(curr->swap_right);
    }
    delete curr;
}

void LFSegmentTree::build(const std::vector<int> &data) {
    int headIdx = freeList.pop();
    head = Head(headIdx, 0);
    refcount[headIdx].store(1);
    build(data, shadows[headIdx], 0, size);
}

void LFSegmentTree::build(const std::vector<int> &data, Node *curr, int idx, int len) {
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

int LFSegmentTree::range_query(int l, int r) {
    int prev_refcount = -1;
    int treeIdx = 0;
    while (prev_refcount < 0) {
        treeIdx = head.load().index;
        prev_refcount = refcount[treeIdx].fetch_add(1);
    }
    Node *tree = shadows[treeIdx];
    int result = range_query(l, r, tree, 0, size - 1);
    refcount[treeIdx].fetch_add(-1);
    
    int expected = 0;
    if (refcount[treeIdx].compare_exchange_weak(expected, INT_MIN)) {
        freeList.push(treeIdx);
    }
    return result;
}

int LFSegmentTree::range_query(int l, int r, Node *curr, int lo, int hi) {
    if (SEG_DISJOINT(l, r, lo, hi)) return base;
    int result = base;
    if (curr->update != base) { 
        int intersection = std::min(r, hi) - std::max(l, lo) + 1;
        result = batch_func(result, intersection, curr->update);
    }
    if (SEG_CONTAINS(l, r, lo, hi)) return func(result, curr->value);
    int mid = SEG_MIDPOINT(lo, hi);
    result = func(result, range_query(l, r, curr->left, lo, mid));
    result = func(result, range_query(l, r, curr->right, mid + 1, hi));
    return result;
}

void LFSegmentTree::range_update(int l, int r, int val) {
    std::vector<Node*> traversal;
    int vidx = -1;
    auto prevHead = head.load();
    auto nextTreeIdx = freeList.pop();
    refcount[nextTreeIdx].store(1);
    Node *prevNode, *nextNode;
	while (true) {
        Head nextHead = Head(nextTreeIdx, prevHead.ticket + 1);
        prevNode = shadows[prevHead.index];
        nextNode = shadows[nextTreeIdx];
		range_update(l, r, val, nextNode, prevNode, 0, size - 1, traversal, vidx);
        vidx = 0;
        if (head.compare_exchange_weak(prevHead, nextHead)) {
            break;
        }
    }
    //std::cout << "Vector {";
    //for (const auto& elem : traversal) std::cout << elem << " ";
    //std::cout << "}" << std::endl;

    swap_pointers(l, r, prevNode, 0, size - 1, traversal, vidx);
    refcount[prevHead.index].fetch_add(-1);
    int expected = 0;
    if (refcount[prevHead.index].compare_exchange_weak(expected, INT_MIN)) {
        freeList.push(prevHead.index);
    }
}

LFSegmentTree::Node *LFSegmentTree::range_update(int l, int r, int val, Node *curr, Node *prev, int lo, int hi, std::vector<Node*> &traversal, int &vidx) {
    if (SEG_DISJOINT(l, r, lo, hi)) {
        if (vidx == -1) {
            traversal.push_back(curr);
        } else {
            traversal[vidx++] = curr;
        }
        return prev;
    }
    curr->value = prev->value;
    curr->update = prev->update;
    if (SEG_CONTAINS(l, r, lo, hi)) {
        curr->update = func(curr->update, val);
        curr->swap_left = range_update(l, r, val, curr->swap_left, prev->left, -1, -1, traversal, vidx); // disjoint
        curr->swap_right = range_update(l, r, val, curr->swap_right, prev->right, -1, -1, traversal, vidx); // disjoint
    } else {
        int intersection = std::min(r, hi) - std::max(l, lo) + 1;
        curr->value = batch_func(curr->value, intersection, val);
        int mid = SEG_MIDPOINT(lo, hi);
        curr->swap_left = range_update(l, r, val, curr->swap_left, prev->left, lo, mid, traversal, vidx);
        curr->swap_right = range_update(l, r, val, curr->swap_right, prev->right, mid + 1, hi, traversal, vidx);
    }
    curr->left = curr->swap_left;
    curr->right = curr->swap_right;
    return curr;
}

LFSegmentTree::Node *LFSegmentTree::swap_pointers(int l, int r, Node *prev, int lo, int hi, std::vector<Node*> &traversal, int &vidx) {
    if (SEG_DISJOINT(l, r, lo, hi)) return traversal[vidx++];
    if (SEG_CONTAINS(l, r, lo, hi)) {
        prev->swap_left = swap_pointers(l, r, prev->left, -1, -1, traversal, vidx); // disjoint
        prev->swap_right = swap_pointers(l, r, prev->right, -1, -1, traversal, vidx); // disjoint
    } else {
        int mid = SEG_MIDPOINT(lo, hi);
        prev->swap_left = swap_pointers(l, r, prev->left, lo, mid, traversal, vidx);
        prev->swap_right = swap_pointers(l, r, prev->right, mid + 1, hi, traversal, vidx);
    }
    return prev;
}

void LFSegmentTree::print() {
    print(shadows[head.load().index], "", true);

    /*
    print(shadows[0], "", true);
    std::cout << "printing 0 done" << std::endl;
    print(shadows[1], "", true);
    std::cout << "printing 1 done" << std::endl;
    */
}

void LFSegmentTree::print(Node *curr, const std::string &pref, bool last) {
    std::cout << pref << (last ? "└── " : "├── ") << "(" << curr->value << ", " << curr->update << ")" << std::endl;
    //std::cout << pref << (last ? "└── " : "├── ") << "(nid: " << curr->nid << ")" << std::endl;
    std::string next_pref = pref + (last ? "    " : "│   ");
    if (curr->left != nullptr) {
        print(curr->left, next_pref, false);
    }
    if (curr->right != nullptr) {
        print(curr->right, next_pref, true);
    }
}