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
#include <cmath>
#include <thread>
#include <chrono>
#include <map>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <thread>

// bit depth checking
#if _WIN64
#define _ENV64
#else
#if _WIN32
#define _ENV32
#else
#error UNKNOWN_WORD_SIZE
#endif
#endif

// some type macros
#ifdef _ENV64
typedef int64_t INT_T;
typedef uint64_t UINT_T;
typedef double FLT_T;
#endif

#ifdef _ENV32
typedef int32_t INT_T;
typedef uint32_t UINT_T;
typedef float FLT_T;
#endif

#define _INT (INT_T)
#define _UINT (UINT_T)

// some fixed width casting macros
#define _I32 (int32_t)
#define _U32 (uint32_t)
#define _F32 (float)
#define _I64 (int64_t)
#define _U64 (uint64_t)
#define _F64 (double)

// define some comparison macros

#define ABS(a) ((a < 0) ? -a : a)
#define MAX_2(a, b) ((a > b) ? a : b)
#define MIN_2(a, b) ((b > a) ? a : b);
#define MAX_3(a, b, c) (MAX_2(MAX_2(a, b), c))
#define MIN_3(a, b, c) (MIN_2(MIN_2(a, b), c))
#define UMAX_2(a, b) MAX_2(ABS(a), ABS(b))
#define UMIN_2(a, b) MIN_2(ABS(a), ABS(b))
#define UMAX_3(a, b, c) MAX_3(ABS(a), ABS(b), ABS(c))
#define UMIN_3(a, b, c) MIN_3(ABS(a), ABS(b), ABS(c))

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
//template <typename T>
//bool readFileToString(T filePath, std::string& writeback);
template <typename T>
bool readFileToString(T filePath, std::string& writeback) {
	// open filestream using standard library
	std::ifstream f(filePath);
	// check if file successfully opened
	// therefore check if the file exists
	if (!f.is_open()) {
		std::cout << "Error: could not find file: " << filePath << "\n";
		return false;
	}
	// use stringstream as a go between
	std::stringstream buffer;
	buffer << f.rdbuf();
	// close file
	f.close();
	// convert to string and populate writeback variable
	writeback = buffer.str();

	return true;
}


// check if c++ indexable container type contains value
template <typename T1, typename T2>
bool contains(T1& container, T2 value) {
	return std::find(container.begin(), container.end(), value) != container.end();
}


#endif