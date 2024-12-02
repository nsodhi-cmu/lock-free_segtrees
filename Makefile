CXX = g++
CXXFLAGS = -std=c++23 -O3 -pedantic -Wall -Wextra -Wunused-variable -Wformat -Wno-unknown-pragmas -fcilkplus

SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
MAIN_SRC = main.cpp

HEADERS = $(wildcard $(SRC_DIR)/*.h)

TARGET = segtree_main

all: $(TARGET)

$(TARGET): $(SRCS) $(MAIN_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $(SRCS) $(MAIN_SRC)

clean:
	rm -f $(TARGET)