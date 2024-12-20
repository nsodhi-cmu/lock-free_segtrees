#ifndef SEGTREE_H
#define SEGTREE_H

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
/**
 * Rounds up to the next power of two
 * @param size Value to round up
 */
#define NEXT_POW_TWO(size) \
    (--(size), \
    (size) |= (size) >> 1, \
    (size) |= (size) >> 2, \
    (size) |= (size) >> 4, \
    (size) |= (size) >> 8, \
    (size) |= (size) >> 16, \
    ++(size))

typedef int (*AssociativeFunction)(int, int);
typedef int (*BatchAssociativeFunction)(int, int, int);

// Abstract Class
class SegmentTree {
    protected:
        int base;
        AssociativeFunction func;
        BatchAssociativeFunction batch_func;
    public:
        virtual ~SegmentTree() = default;
        virtual void build(const std::vector<int> &data) = 0;
        virtual int range_query(int lower, int upper) = 0;
        virtual void range_update(int lower, int upper, int value) = 0;
        virtual void print() = 0;
};

#endif