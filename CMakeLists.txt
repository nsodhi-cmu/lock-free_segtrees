cmake_minimum_required(VERSION 3.30)
project(SEGTREE)

set(CMAKE_CXX_STANDARD 26)

add_executable(SEGTREE main.cpp
        SegmentTree.cpp
        SegmentTree.h
        SegmentTreeChecker.cpp)
