// some general purpose functions that may be useful
// in different parts of the program
// also stores common standard library files for the program

#ifndef UTIL_H
#define UTIL_H

// standard library files that will be used throughout the project

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <thread>
#include <chrono>

// Type for color
typedef struct {
	float r;
	float b;
	float g;
	float a;
} Color_Type;

// Type for color union
typedef union {
	Color_Type bit;

	float reg[sizeof(Color_Type) / sizeof(float)];
} Color;

// Reads file given in const char* path
// to C++ std::string. Uses container type
// because returning dynamically allocated arrays
// is strange at best.
bool readFileToString(const char* filePath, std::string& writeback);


#endif