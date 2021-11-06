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
//#include <thread>

// SDL is used for window management and all
// that nasty platform specific stuff
#include <SDL.h>
#undef main // must undefine main defined in SDL.h
// to avoid linker errors
#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif

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
#define INT_T_MAX INT64_MAX
#define UINT_T_MAX UINT64_MAX
#define INT_T_MIN INT64_MIN
#define UINT_T_MIN UINT64_MIN
#endif

#ifdef _ENV32
typedef int32_t INT_T;
typedef uint32_t UINT_T;
typedef float FLT_T;
#define INT_T_MAX INT32_MAX
#define UINT_T_MAX UINT32_MAX
#define INT_T_MIN INT32_MIN
#define UINT_T_MIN UINT32_MIN
#endif

#define _INT(a) static_cast<INT_T>(a)
#define _UINT(a) static_cast<UINT_T>(a)

// some fixed width casting macros
#define _I32(a) static_cast<int32_t>(a)
#define _U32(a) static_cast<uint32_t>(a)
#define _F32(a) static_cast<float>(a)
#define _I64(a) static_cast<int64_t>(a)
#define _U64(a) static_cast<uint64_t>(a)
#define _F64(a) static_cast<double>(a)

// define some comparison macros

#define ABS(a) ((a < 0) ? -(a) : a)
#define MAX_2(a, b) ((a > b) ? a : b)
#define MIN_2(a, b) ((b > a) ? a : b)
#define MAX_3(a, b, c) (MAX_2(MAX_2(a, b), c))
#define MIN_3(a, b, c) (MIN_2(MIN_2(a, b), c))
#define UMAX_2(a, b) MAX_2(ABS(a), ABS(b))
#define UMIN_2(a, b) MIN_2(ABS(a), ABS(b))
#define UMAX_3(a, b, c) MAX_3(ABS(a), ABS(b), ABS(c))
#define UMIN_3(a, b, c) MIN_3(ABS(a), ABS(b), ABS(c))

#define ARRLEN(a) (sizeof(a)/sizeof(a[0]))
#define STRLEN(s) (ARRLEN(s)-1)


// constants
constexpr float GRAVITATION_CONSTANT = 6.6743015E-11;


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


struct OBJ_Material {
	float ambient[3] = { 0.1f, 0.1f, 0.1f };
	float diffuse[3] = { 1.0f, 0.4f, 0.4f };
	float specular[3] = { 0.0f, 0.0f, 0.0f };
	//float emmision[3] = {0.0f, 0.0f, 0.0f};
	float alpha = 1.0f;
	float specularExponent = 32.0f;
	bool textureDiffuse = false;
	bool textureAmbient = false;
	bool textureSpecular = false;
	//std::string ambientTexturePath;
	//std::string diffuseTexturePath;
	//std::string specularTexturePath;
	uint8_t* ambientTexturePointer = nullptr;
	uint8_t* diffuseTexturePointer = nullptr;
	uint8_t* specularTexturePointer = nullptr;

	bool customShader = false;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
};

// structure describing all data about an object
typedef struct {
	// data about vertices
	std::vector<float> mesh;
	// material data
	std::vector<OBJ_Material> materials;
	// array that describes how materials and
	// vertices are linked
	// Format: vertexindex, materialindex
	std::vector<std::pair<UINT_T, UINT_T>> matIndexes;
} OBJ_Data;

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


template <typename T>
bool readFileToByteArray(T filePath, uint8_t*& writeback, INT_T& wbSize) {
	// oppen file
	std::ifstream f(filePath, std::ifstream::binary);
	// check if file successfully opened
	// therefore check if the file exists
	if (!f.is_open()) {
		std::cout << "Error: could not find file: " << filePath << "\n";
		return false;
	}
	f.seekg(0, f.end);
	INT_T size = f.tellg();
	f.seekg(0, f.beg);
	writeback = (uint8_t*)realloc(writeback, size);
	f.read((char*)writeback, size);
	wbSize = size;
	return true;
}


// check if c++ indexable container type contains value
template <typename T1, typename T2>
bool contains(T1& container, T2 value) {
	return std::find(container.begin(), container.end(), value) != container.end();
}


#endif