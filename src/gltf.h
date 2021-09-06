// header file for GLTF / GLB file interpreter
// this largely uses C where appropriate
// for speed reasons, however C++ containers
// are sometimes used to avoid the pitfalls
// of static types

#ifndef GLTF_H
#define GLTF_H

#include "util.h"
#include "textures.h"

// Function to load an object mesh from file
// Mesh format:
// float position[3]
// float normalVector[3]
// float textureCoords[2]
OBJ_Data GLTF_GenMesh(const char* filePath, UINT_T& size, Textures& textures, bool& status);

OBJ_Data GLB_GenMesh(const char* filePath, UINT_T& size, Textures& textures, bool& status);

#endif