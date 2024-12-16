#include <vector>
#include <iostream>
#include <random>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <pthread.h>
#include <unistd.h>

#include "segment_tree.h"
#include "coarse.h"
#include "fine.h"
#include "lock-free.h"
#include "pointer.h"

using namespace std;

#define MAX_VAL 100

int add(int a, int b) {
    return a + b;
}

int batch_add(int x, int c, int v) {
    return x + c * v;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int batch_max(int x, int c, int v) {
    return x > v ? x : v;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int batch_min(int x, int c, int v) {
    return x < v ? x : v;
}

int mult(int a, int b) {
    return a * b;
}

int batch_mult(int x, int c, int v) {
    int res = 1;
    while (c > 0) {
        if (c % 2 == 1) {
            res *= v;
        }
        v *= v;
        c /= 2;
    }
    return x * res;
}

struct thr_params_t {
    int idx;
    int num_operations;
    int size;
    int seed;
    float prob;
    int min_range;
    int max_range;
    SegmentTree *tree;
};

void* workload(void* arg) {
    thr_params_t params = *reinterpret_cast<thr_params_t*>(arg);
    SegmentTree* tree = params.tree;

    mt19937 gen(params.seed);
    uniform_int_distribution range_dist(params.min_range, params.max_range);
    uniform_int_distribution val_dist(-MAX_VAL, MAX_VAL);
    uniform_real_distribution size_dist(0.0f, 1.0f);
    uniform_real_distribution prob_dist(0.0f, 1.0f);

    const auto start = chrono::steady_clock::now();

    for (int op = 0; op < params.num_operations; op++) {
        int range_size = range_dist(gen);
        int lower = static_cast<int>(size_dist(gen) * (params.size - range_size + 1));
        int upper = lower + range_size - 1;
        
        if (prob_dist(gen) < params.prob) {
            // Range Query
            tree->range_query(lower, upper);
        } else {
            // Range Update
            tree->range_update(lower, upper, val_dist(gen));
        }
    }

    const auto end = chrono::steady_clock::now();

    const double time = chrono::duration_cast<chrono::duration<double>>(end - start).count();
    cout << "Thread #" << params.idx << " Runtime (sec): " << fixed << setprecision(10) << time << endl;
    return nullptr;
}

void decode_function_type(char function_type, char& base, AssociativeFunction& function, BatchAssociativeFunction& batch_function) {
    switch (function_type) {
        case 'a':
            base = 0;
            function = add;
            batch_function = batch_add;
            break;
        case 'l':
            base = INT_MAX;
            function = min;
            batch_function = batch_min;
            break;
        case 'g':
            base = INT_MIN;
            function = max;
            batch_function = batch_max;
            break;
        case 'm':
            base = 1;
            function = mult;
            batch_function = batch_mult;
            break;
        default:
            base = 0;
            function = add;
            batch_function = batch_add;
            break;
    }
}

int main(int argc, char *argv[]) {
    char mode = 0, tree_type = 0, function_type = 0;
    int num_threads = 0, num_operations = 0, size = 0, seed = 42;
    float prob = 0.0f;

    int opt;
    while ((opt = getopt(argc, argv, "m:t:f:p:r:n:i:")) != -1) {
        switch (opt) {
            case 'm':
                mode = *optarg;
                break;
            case 't':
                tree_type = *optarg;
                break;
            case 'f':
                function_type = *optarg;
                break;
            case 'p':
                num_threads = atoi(optarg);
                break;
            case 'r':
                prob = atof(optarg);
                break;
            case 'n':
                size = atoi(optarg);
                break;
            case 'i':
                num_operations = atoi(optarg);
                break;
            default:
                cerr << "Usage: " << argv[0] << " -m mode -t tree_type -f associative_function -p num_threads -r read_prob -n data_size -i operations [-s seed]" << endl;
                return -1;
        }
    }
    if (!mode || !num_threads || !tree_type || !function_type || !size || !num_operations || prob == 0.0f) {
        cerr << "Usage: " << argv[0] << " -m mode -t tree_type -f associative_function -p num_threads -r read_prob -n data_size -i operations [-s seed]" << endl;
        return -1;
    }

    int base;
    AssociativeFunction function;
    BatchAssociativeFunction batch_function;

    switch (function_type) {
        case 'a':
            base = 0;
            function = add;
            batch_function = batch_add;
            break;
        case 'l':
            base = INT_MAX;
            function = min;
            batch_function = batch_min;
            break;
        case 'g':
            base = INT_MIN;
            function = max;
            batch_function = batch_max;
            break;
        case 'm':
            base = 1;
            function = mult;
            batch_function = batch_mult;
            break;
        default:
            base = 0;
            function = add;
            batch_function = batch_add;
            break;
    }

    SegmentTree *tree;
    
    if (tree_type == 'C') {
        tree = new CoarseSegmentTree(size, base, function, batch_function);
    }
    else if (tree_type == 'F') {
        tree = new FineSegmentTree(size, base, function, batch_function);
    }
    else if (tree_type == 'P') {
        tree = new PointerSegmentTree(size, base, function, batch_function);
    }
    else {
        tree = new LFSegmentTree(size, base, function, batch_function, num_threads);
    }

    int num_operations_per_thread = (mode == 'D') ? (num_operations / num_threads) : num_operations;

    printf("Mode: %c\n", mode);
    printf("Tree Type: %c\n", tree_type);
    printf("Associative Function: %c\n", function_type);
    printf("Number of threads: %d\n", num_threads);
    printf("Number of operations: %d Total, %d Per Thread\n", num_operations, num_operations_per_thread);
    printf("Size: %d\n", size);
    printf("Seed: %d\n", seed);
    printf("Read Probability: %f\n", prob);

    vector<thr_params_t> thread_params(num_threads);
    for (int i = 0; i < num_threads; i++) {
        thr_params_t &thr_params = thread_params[i];
        thr_params.idx = i;
        thr_params.num_operations = num_operations_per_thread;
        thr_params.tree = tree;
        thr_params.size = size;
        thr_params.seed = ++seed;
        thr_params.prob = prob;
        thr_params.min_range = 0;
        thr_params.max_range = size;
    }

    vector data(size, 0);
    tree->build(data);

    vector<pthread_t> threads(num_threads);

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&threads[i], nullptr, workload, &thread_params[i]) != 0) {
            return -1;
        }
    }

    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], nullptr) != 0) {
            return -1;
        }
    }
    return 0;
}