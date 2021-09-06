// source file for gltf.h

#include "gltf.h"
#include "obj.h"
#include "nlohmann/json.hpp"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

using json = nlohmann::json;


OBJ_Data GLTF_GenMesh(const char* filePath, UINT_T& size, Textures& textures, bool& status) {
	return {};
}


OBJ_Data GLB_GenMesh(const char* filePath, UINT_T& size, Textures& textures, bool& status) {
	status = false;

	OBJ_Data objectData;

	// read file function will allocate memory for array
	// must pass nullptr to avoid memory exception
	uint8_t* data8 = nullptr;
	INT_T nrBytes;
	if (!readFileToByteArray(filePath, data8, nrBytes)) return {};

	const uint32_t MAGIC = 0x46546C67;
	const uint32_t JSON = 0x4E4F534A;
	const uint32_t BIN = 0x004E4942;

	uint32_t* data32 = (uint32_t*)data8;

	uint8_t* json8 = data8 + 12;
	uint32_t* json32 = (uint32_t*)json8;

	uint32_t magic = data32[0];
	uint32_t version = data32[1];
	uint32_t length = data32[2];
	uint32_t chunkLengthJson = json32[0];
	uint32_t chunkTypeJson = json32[1];
	
	char* jsonStr = (char*)(json8 + 8);
	
	uint8_t* binary8 = json8 + chunkLengthJson + 8;
	uint32_t* binary32 = (uint32_t*)binary8;

	uint32_t chunkLengthBin = binary32[0];
	uint32_t chunkTypeBin = binary32[1];

	uint8_t* bin8 = binary8 + 8;
	uint32_t* bin32 = (uint32_t*)bin8;

	if (magic != MAGIC || chunkTypeJson != JSON || chunkTypeBin != BIN || length - chunkLengthJson - 28 != chunkLengthBin) {
		printf("Error: %s is not valid\n", filePath);
		printf("%08X\n%08X\n%08X\n", magic, chunkTypeJson, chunkTypeBin);
		return {};
	}

	// parse json
	json j = json::parse(jsonStr, jsonStr + chunkLengthJson);
	std::cout << j["asset"].dump() << "\n";

	INT_T scene = j["scene"];
	INT_T node = j["scenes"][scene]["nodes"][0];
	INT_T mesh = j["nodes"][node]["mesh"];
	INT_T material = j["meshes"][mesh]["primitives"][0]["material"];
	INT_T diffuseTexture = j["materials"][material]["normalTexture"]["index"];
	//INT_T material = j["meshes"][mesh]["primitives"][0]["material"];
	printf("%d, %d, %d\n", scene, node, mesh);

	std::vector<OBJ_Material>& materials = objectData.materials;
	materials.push_back(OBJ_Material());

	//for (auto it = j["materials"].begin(); it != j["materials"].end(); ++it) {

	//}

	INT_T positionAccessor = j["meshes"][mesh]["primitives"][0]["attributes"]["POSITION"];
	INT_T normalAccessor = j["meshes"][mesh]["primitives"][0]["attributes"]["NORMAL"];
	INT_T texcoordAccessor = j["meshes"][mesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	INT_T indexAccessor = j["meshes"][mesh]["primitives"][0]["indices"];
	//INT_T diffuseAccessor = j["images"][diffuseTexture]["bufferView"];
	printf("%d, %d, %d, %d\n", positionAccessor, normalAccessor, texcoordAccessor, indexAccessor);

	INT_T positionBuffer = j["accessors"][positionAccessor]["bufferView"];
	INT_T normalBuffer = j["accessors"][normalAccessor]["bufferView"];
	INT_T texcoordBuffer = j["accessors"][texcoordAccessor]["bufferView"];
	INT_T indexBuffer = j["accessors"][indexAccessor]["bufferView"];
	INT_T diffuseBuffer = j["images"][diffuseTexture]["bufferView"] + 1;
	printf("%d, %d, %d, %d\n", positionBuffer, normalBuffer, texcoordBuffer, indexBuffer);

	float* positionPointer = (float*)(bin8 + j["bufferViews"][positionBuffer]["byteOffset"]);
	float* normalPointer = (float*)(bin8 + j["bufferViews"][normalBuffer]["byteOffset"]);
	float* texcoordPointer = (float*)(bin8 + j["bufferViews"][texcoordBuffer]["byteOffset"]);
	uint16_t* indexPointer = (uint16_t*)(bin8 + j["bufferViews"][indexBuffer]["byteOffset"]);
	uint8_t* diffusePointer = bin8 + j["bufferViews"][diffuseBuffer]["byteOffset"];
	INT_T diffuseSize = j["bufferViews"][diffuseBuffer]["byteLength"];
	//size = _UINT(j["bufferViews"][indexBuffer]["byteLength"] / 2);
	printf("image size: %d\n", diffuseSize);

	// uncompress image
	INT_T x, y, nrChannels = 0;
	//stbi_set_flip_vertically_on_load(true);
	//objectData.materials[0].diffuseTexturePointer = stbi_load_from_memory(diffusePointer, diffuseSize, &x, &y, &nrChannels, 0);
	textures.parseImageToBuffer(diffusePointer, diffuseSize, objectData.materials[0].diffuseTexturePointer);
	objectData.materials[0].textureDiffuse = true;
	// buffer = stbi_load(filePath, &texWidth, &texHeight, &nrChannels, 0);
	//printf("%d, %d, %d\n", nrChannels, x, y);
	//printf("%s", stbi_failure_reason());

	//stbi_write_png("./assets/EarthCloudsTex.png", x, y, 3, objectData.materials[0].diffuseTexturePointer, nrChannels * x);

	//objectData.mesh.reserve(size * 8);
	// use array syntax to speed up parsing
	//float* mesharr = &objectData.mesh[0];

	OBJ_Data cubeData = OBJ_GenMesh("../glTests/assets/floor/woodfloor.obj", size, textures, status);
	objectData.mesh = cubeData.mesh;

	//// return number of vertices
	//// loop through all vertexes to arrange data
	//for (UINT_T i = 0; i < size; ++i) {
	//	// push vertex coordinates
	//	//mesh.push_back(vertices[vertexIndexes[i] * 3]);
	//	//mesh.push_back(vertices[vertexIndexes[i] * 3 + 1]);
	//	//mesh.push_back(vertices[vertexIndexes[i] * 3 + 2]);
	//	objectData.mesh.push_back(positionPointer[indexPointer[i] * 3]);
	//	objectData.mesh.push_back(positionPointer[indexPointer[i] * 3 + 1]);
	//	objectData.mesh.push_back(positionPointer[indexPointer[i] * 3 + 2]);

	//	// push vertex normal vectors
	//	//mesh.push_back(vertexNormals[normalIndexes[i] * 3]);
	//	//mesh.push_back(vertexNormals[normalIndexes[i] * 3 + 1]);
	//	//mesh.push_back(vertexNormals[normalIndexes[i] * 3 + 2]);
	//	objectData.mesh.push_back(normalPointer[indexPointer[i] * 3]);
	//	objectData.mesh.push_back(normalPointer[indexPointer[i] * 3 + 1]);
	//	objectData.mesh.push_back(normalPointer[indexPointer[i] * 3 + 2]);

	//	// push vertex texture coordinates
	//	//mesh.push_back(textureCoords[textureIndexes[i] * 2]);
	//	//mesh.push_back(textureCoords[textureIndexes[i] * 2 + 1]);
	//	objectData.mesh.push_back(texcoordPointer[indexPointer[i] * 2]);
	//	objectData.mesh.push_back(texcoordPointer[indexPointer[i] * 2 + 1]);
	//	printf("%f %f\n", objectData.mesh[i * 8 + 6], objectData.mesh[i * 8 + 7]);
	//}

	printf("test");

	objectData.matIndexes.push_back(std::pair(0, 0));

	// populate materials

	free(data8);

	return objectData;
}