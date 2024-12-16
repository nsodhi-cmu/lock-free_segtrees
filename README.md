15-418 Fall 2024 Term Project

Lock-Free Segment Trees and Lazy Propagation

How to run:
./segment_tree -m mode -t tree_type -f associative_function -p num_threads -r read_prob -n data_size -i operations [-s seed]

Mode: Either "D" to distribute the workload among the threads or "I" for independent workload for each thread

Tree Type: "C" for coarse-grained array-based, "P" for coarse-grained pointer-based, "F" for fine-grained array-based, "L" for lock-free

Associative Function: "a" for addition, "m" for multiplication, "l" for minimum (least), "g" for maximum (greatest)

Num Threads: The number of threads to parallelize across

Read Probability: Probability of an operation being a read operation

Data Size: The size of the segment tree data

Operations: The size of the workload in number of operations

Seed: The random seed
