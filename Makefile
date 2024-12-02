CXX = g++
CXXFLAGS = -std=c++26 -O3 -pedantic -Wall -Wextra -Wshadow -Wunused-variable -Wconversion -Wformat -Wno-unknown-pragmas -fopenmp -pthread

SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)

COARSE_SRC = $(SRC_DIR)/coarse.cpp
FINE_SRC = $(SRC_DIR)/fine.cpp
LOCKFREE_SRC = $(SRC_DIR)/lock-free.cpp
MAIN_SRC = main.cpp

HEADERS = $(SRC_DIR)/segment_tree.h
TARGETS = coarse fine lock-free

coarse: $(COARSE_SRC) $(MAIN_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(COARSE_SRC) $(MAIN_SRC)

fine: $(FINE_SRC) $(MAIN_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(FINE_SRC) $(MAIN_SRC)

lock-free: $(LOCKFREE_SRC) $(MAIN_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(LOCKFREE_SRC) $(MAIN_SRC)

clean:
	rm -f $(TARGETS)