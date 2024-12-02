#include <vector>
#include <iostream>

#include "coarse.h"

using namespace std;

int add(int a, int b) {
    return a + b;
}

int batch_add(int x, int c, int v) {
    return x + c * v;
}

int main() {
    int base = 0;
    int (*add_ptr)(int, int) = add;
    int (*batch_add_ptr)(int, int, int) = batch_add;

    vector<int> data = {3, 4, 1, 0};

    CoarseSegmentTree t = CoarseSegmentTree(data.size(), base, add_ptr, batch_add_ptr);

    t.build(data);

    for (int i = 1; i < 8; ++i) {
        std::cout << "Node " << i << "\t Value: " << t.tree[i].value << "\t Update: " << t.tree[i].update << std::endl;
    }

    t.range_update(1, 3, 1);

    for (int i = 1; i < 8; ++i) {
        std::cout << "Node " << i << "\t Value: " << t.tree[i].value << "\t Update: " << t.tree[i].update << std::endl;
    }

    int r = t.range_query(1, 3);

    cout << r << endl;

    for (int i = 1; i < 8; ++i) {
        std::cout << "Node " << i << "\t Value: " << t.tree[i].value << "\t Update: " << t.tree[i].update << std::endl;
    }

    r = t.range_query(2, 3);

    cout << r << endl;

    for (int i = 1; i < 8; ++i) {
        std::cout << "Node " << i << "\t Value: " << t.tree[i].value << "\t Update: " << t.tree[i].update << std::endl;
    }

    return 0;
}