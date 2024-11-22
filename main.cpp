#include "SegmentTree.h"
using namespace std;

int main() {
    SegmentTree tree(10);
    vector data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    tree.build(data);

    SegmentTreeChecker checker(10, tree, data);
    checker.verify();
    checker.range_update_add(3, 5, 10);
    checker.range_update_add(0, 4, -5);
    checker.verify();
    checker.print();

    return 0;
}
