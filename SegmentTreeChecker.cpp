#include "SegmentTree.h"

SegmentTreeChecker::SegmentTreeChecker(int size, SegmentTree &tree, vector<int> &data): tree(tree), data(data) {
    this->size = size;
}

void SegmentTreeChecker::range_update_add(int lo, int hi, int val) const {
    tree.range_update_add(lo, hi, val);
    data_range_update_add(lo, hi, val);
}

void SegmentTreeChecker::verify() const {
    for (int lo = 0; lo < data.size(); ++lo) {
        for (int hi = lo; hi < data.size(); ++hi) {
            int tree_sum = tree.range_query(lo, hi);
            int data_sum = data_range_query(lo, hi);
            if (tree_sum != data_sum) {
                printf("data[%d:%d] = %d vs %d\n", lo, hi, tree_sum, data_sum);
            }
        }
    }
}

void SegmentTreeChecker::print() {
    printf("DATA:\n");
    print_data();
    printf("TREE:\n");
    print_tree(1, 0, size - 1, 0);
}

void SegmentTreeChecker::print_data() const {
    for (int i : data) {
        printf("%d ", i);
    }
    printf("\n");
}

void SegmentTreeChecker::print_tree(int i, int lo, int hi, int level) {
    printf("%*s", 4 * level, "");
    printf("[%d:%d] = %d\n", lo, hi, tree.tree[i].sum);
    if (lo == hi) return;
    int mid = lo + (hi - lo) / 2;
    print_tree(L_INDEX(i), lo, mid, level + 1);
    print_tree(R_INDEX(i), mid + 1, hi, level + 1);
}

void SegmentTreeChecker::data_range_update_add(int lo, int hi, int val) const {
    for (int i = lo; i <= hi; ++i) {
        data[i] += val;
    }
}

int SegmentTreeChecker::data_range_query(int lo, int hi) const {
    int sum = 0;
    for (int i = lo; i <= hi; ++i) {
        sum += data[i];
    }
    return sum;
}

