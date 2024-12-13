CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -pthread -pedantic -Wall -Wextra -Wunused-variable -Wformat -Wno-unknown-pragmas

SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
MAIN_SRC = main.cpp

HEADERS = $(wildcard $(SRC_DIR)/*.h)

TARGET = segtree_main

all: $(TARGET)

$(TARGET): $(SRCS) $(MAIN_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(SRCS) $(MAIN_SRC) -latomic

clean:
	rm -f $(TARGET)