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

struct thr_params_t {
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
    return nullptr;
}

int main(int argc, char *argv[]) {
    string input_filename;
    char tree_type;
    int num_threads;

    int opt;
    while ((opt = getopt(argc, argv, "f:t:n:")) != -1) {
        switch (opt) {
            case 'f':
                input_filename = optarg;
                break;
            case 't':
                tree_type = *optarg;
                break;
            case 'n':
                num_threads = atoi(optarg);
                break;
            default:
                cerr << "Usage: " << argv[0] << " -f input_filename -t tree_type [-n num_threads]" << endl;
                return -1;
        }
    }
    if (empty(input_filename)) {
        cerr << "Usage: " << argv[0] << " -f input_filename" << endl;
        return -1;
    }

    ifstream input_file(input_filename);
    if (!input_file) {
        cerr << "Unable to open file: " << input_filename << endl;
        return -1;
    }

    int _num_threads, num_operations, size, seed;
    char function_type;

    input_file >> _num_threads >> num_operations >> size >> seed;
    input_file >> function_type;

    int base;
    AssociativeFunction function;
    BatchAssociativeFunction batch_function;

    switch (function_type) {
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
    else {
        tree = new LFSegmentTree(size, base, function, batch_function, num_threads);
    }

    vector<thr_params_t> thread_params(num_threads);
    for (auto& thr_params : thread_params) {
        thr_params.num_operations = num_operations;
        thr_params.tree = tree;
        thr_params.size = size;
        thr_params.seed = ++seed;
        input_file >> thr_params.prob >> thr_params.min_range >> thr_params.max_range;
    }
   
    mt19937 gen(seed);
    uniform_int_distribution<int> dist(0, 2 * MAX_VAL);

    vector<int> data;
    data.reserve(size);
    for (int i = 0; i < size; i++) {
        data.push_back(dist(gen));
    }

    tree->build(data);

    vector<pthread_t> threads(num_threads);

    const auto start = chrono::steady_clock::now();

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

    const auto end = chrono::steady_clock::now();
    
    const double time = chrono::duration_cast<chrono::duration<double>>(end - start).count();
    cout << "Runtime (sec): " << fixed << setprecision(10) << time << endl;

    // delete tree;

    return 0;
}