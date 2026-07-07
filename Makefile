# Makefile for Graph Theory and Route Optimization Project

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -I.

TARGET = main
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
