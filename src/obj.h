// header file for .obj file interpreter

#ifndef OBJ_H
#define OBJ_H

#include "util.h"

typedef struct {
	float diffuse[4];
	float ambient[4];
	float specular[4];
	float specularIntensity;
	bool textureDiffuse;
	bool textureAmbient;
	bool textureSpecular;
	const char* ambientTexturePath;
	const char* diffuseTexturePath;
	const char* specularTexturePath;
} OBJ_Material;

// Function to load an object mesh from file
// Mesh format:
// float position[3]
// float normalVector[3]
// float textureCoords[2]
std::vector<float> OBJ_GenMesh(const char* filePath, uint32_t& size, bool& status);

// Searches .obj file for .mtl file name
std::string OBJ_GetMTLpath(const char* filePath, bool& status);

// Function returns all materials associated with an object
std::vector<OBJ_Material> OBJ_GetMaterials(const char* filePath, bool& status);

// Function returns indexes of where materials are
// applied to an object
std::vector<uint32_t> OBJ_GetMaterialIndexes(const char* filePath, bool& success);

#endif