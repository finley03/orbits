// header file for .obj file interpreter
// this largely uses C where appropriate,
// for speed reasons, however C++ containers
// are sometimes used to avoid the pitfalls
// of static types

#ifndef OBJ_H
#define OBJ_H

#include "util.h"

struct OBJ_Material {
	float ambient[3] = {0.1f, 0.1f, 0.1f};
	float diffuse[3] = {0.4f, 0.4f, 0.4f};
	float specular[3] = {0.0f, 0.0f, 0.0f};
	float emmision[3] = {0.0f, 0.0f, 0.0f};
	float alpha = 1.0f;
	float specularExponent = 32.0f;
	bool textureDiffuse = false;
	bool textureAmbient = false;
	bool textureSpecular = false;
	std::string ambientTexturePath;
	std::string diffuseTexturePath;
	std::string specularTexturePath;
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

// Function to load an object mesh from file
// Mesh format:
// float position[3]
// float normalVector[3]
// float textureCoords[2]
OBJ_Data OBJ_GenMesh(const char* filePath, UINT_T& size, bool& status);

//// Function returns all materials associated with an object
//// pass name of mtl file
//std::vector<OBJ_Material> OBJ_GetMaterials(const char* mtlFilePath, bool& status);
//
//// Function returns indexes of where materials are
//// applied to an object
//std::vector<UINT_T> OBJ_GetMaterialIndexes(const char* filePath, const char* mtlFilePath, bool& status);

#endif