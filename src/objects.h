// header file for objects class
// this file handles object generation, manipulation
// and rendering

#ifndef OBJECTS_H
#define OBJECTS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"
#include "textures.h"
#include "shaders.h"
#include "camera.h"


// extern camera pointer declaration
extern Camera* camera;

class Objects {
private:
	struct Material {
		float ambient[3] = { 0.1f, 0.1f, 0.1f };
		float diffuse[3] = { 0.4f, 0.4f, 0.4f };
		float specular[3] = { 0.0f, 0.0f, 0.0f };
		float alpha = 1.0f;
		float specularExponent = 32.0f;
		bool textureAmbient = false;
		bool textureDiffuse = false;
		bool textureSpecular = false;
		uint32_t ambientTextureHandle;
		uint32_t diffuseTextureHandle;
		uint32_t specularTextureHandle;

		UINT_T shaderHandle;
	};

	struct ObjectData {
		float position[3] = { 0.0f, 0.0f ,0.0f };
		float velocity[3] = { 0.0f, 0.0f, 0.0f };
		float orientation[3] = { 0.0f, 0.0f, 0.0f };
		float mass = 1.0f;
		float scale = 1.0f;
		float cRadius = 1.0f;

		bool visible = true;
		bool exists = true;

		UINT_T nrVertices;

		uint32_t VBO;
		//std::vector<float> mesh;
		std::vector<Material> materials;
		std::vector<std::pair<UINT_T, UINT_T>> materialIndexes;
	};

	const char* defaultVertexPath = "./shaders/default.vert";
	const char* defaultFragmentPath = "./shaders/default.frag";
	UINT_T defaultShaderHandle;

	uint32_t VAO;

	std::vector<Shader*> shaders;
	std::vector<ObjectData> objects;
	std::vector<UINT_T> freeObjectIndexes;

	Textures textures;
public:
	Objects(bool& status);
	~Objects();

	UINT_T newObject(const char* filePath, bool& status);
	void deleteObject(UINT_T objectHandle);

	void setPosition(UINT_T objectHandle, float* position);
	void setOrientation(UINT_T objectHandle, float* orientation);
	void setScale(UINT_T objectHandle, float scale);

	void getPosition(UINT_T objectHandle, float* writeback);
	void getOrientation(UINT_T objectHandle, float* writeback);
	float getScale(UINT_T objectHandle);
	UINT_T getSize(UINT_T objectHandle);

	void render(UINT_T objectHandle);
	void renderAll();
};

#endif