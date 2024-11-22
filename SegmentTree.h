#ifndef SEGMENTTREE_H
#define SEGMENTTREE_H

#include <vector>

/**
 * Returns the index of the left child node in the tree
 * @param i Index of the current node in the tree (1-indexed)
 */
#define L_INDEX(i) (2 * (i))
/**
 * Returns the index of the right child node in the tree
 * @param i Index of the current node in the tree (1-indexed)
 */
#define R_INDEX(i) (2 * (i) + 1)
/**
 * Returns whether the segments [l1, r1] and [l2, r2] are disjoint
 * @param l1 Index of the left side of the 1st segment
 * @param r1 Index of the right side of the 1st segment
 * @param l2 Index of the left side of the 2nd segment
 * @param r2 Index of the right side of the 2nd segment
 */
#define SEG_DISJOINT(l1, r1, l2, r2) ((l2) > (r1) || (r2) < (l1))
/**
 * Returns whether the segment [l1, r1] contains the segment [l2, r2]
 * @param l1 Index of the left side of the 1st segment
 * @param r1 Index of the right side of the 1st segment
 * @param l2 Index of the left side of the 2nd segment
 * @param r2 Index of the right side of the 2nd segment
 */
#define SEG_CONTAINS(l1, r1, l2, r2) ((l2) >= (l1) && (r2) <= (r1))
/**
 * Returns the index of the midpoint of the segment
 * @param l Index of the left side of the segment
 * @param r Index of the right side of the segment
 */
#define SEG_MIDPOINT(l, r) ((l) + ((r) - (l)) / 2)

using namespace std;

class SegmentTree {
    struct Node {
        int sum;
        int lazy_add;
    };

    Node *tree;
    int size;

    friend class SegmentTreeChecker;

public:
    explicit SegmentTree(int size);
    ~SegmentTree();

    void build(const vector<int> &data);

    void range_update_add(int l, int r, int val);
    int range_query(int l, int r);

private:
    void build(const vector<int> &data, int i, int lo, int hi);

    void range_update_add(int l, int r, int val, int i, int lo, int hi);
    int range_query(int l, int r, int i, int lo, int hi);

    void propagate_update_add(int i, int lo, int hi);
};

class SegmentTreeChecker {
    SegmentTree &tree;
    vector<int> &data;
    int size;

public:
    SegmentTreeChecker(int size, SegmentTree &tree, vector<int> &data);

    void range_update_add(int lo, int hi, int val) const;
    void verify() const;
    void print();

private:
    void data_range_update_add(int lo, int hi, int val) const;
    int data_range_query(int lo, int hi) const;

    void print_data() const;
    void print_tree(int i, int lo, int hi, int level);
};

#endif //SEGMENTTREE_H
