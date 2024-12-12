#include <vector>
#include <iostream>
#include <random>

#include "coarse.h"
#include "fine.h"
#include "lock-free.h"

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

    vector<int> data = {3, 4, 1, 0, -1, -2, 3, 9};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, data.size() - 1);
    std::uniform_int_distribution<> vals(-30, 30);

    CoarseSegmentTree c = CoarseSegmentTree(data.size(), base, add_ptr, batch_add_ptr);
    FineSegmentTree f = FineSegmentTree(data.size(), base, add_ptr, batch_add_ptr);
    LFSegmentTree l = LFSegmentTree(data.size(), base, add_ptr, batch_add_ptr, 1);

    c.build(data);
    f.build(data);
    l.build(data);

    for (int i = 0; i < 30; i++) {
        std::cout << "Iteration: " << i << std::endl;

        int n1 = distrib(gen);
        int n2 = distrib(gen);
        int lo = std::min(n1, n2);
        int hi = std::max(n1, n2);
        int v = vals(gen);
        c.range_update(lo, hi, v);
        f.range_update(lo, hi, v);
        l.range_update(lo, hi, v);
        
        std::cout << "Update: (" << lo << ", " << hi << ") -> " << v << std::endl;

        n1 = distrib(gen);
        n2 = distrib(gen);
        lo = std::min(n1, n2);
        hi = std::max(n1, n2);
        int q1 = c.range_query(lo, hi);
        int q2 = f.range_query(lo, hi);
        int q3 = l.range_query(lo, hi);

        std::cout << "Query: (" << lo << ", " << hi << ")" << std::endl;

        std::cout << "C: " << q1 << "    F: " << q2 << "    " << "L: " << q3 << std::endl << std::endl;
    }

    c.print();
    f.print();
    l.print();

    return 0;
}