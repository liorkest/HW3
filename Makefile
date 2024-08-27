# Makefile for the smash program

# Define the compiler and compiler flags
CXX = g++
CXXFLAGS =  -std=c++11 -Wall -Werror -pedantic-errors -g -DNDEBUG -pthread

# Define the target executable
TARGET = main

# Define the source files and object files
SOURCES = main.cpp customAllocator.h customAllocator.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Rule to build the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

customAllocator.o: customAllocator.cpp customAllocator.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove object files and the target executable
.PHONY: clean
# Cleaning old files before new make
clean:
	rm -f $(OBJECTS) $(TARGET)
