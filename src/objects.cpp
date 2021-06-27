// source file for objects class

#include "objects.h"
#include "obj.h"

Objects::Objects(bool& status) : textures() {// , shader(defaultVertexPath, defaultFragmentPath, status) {
	// shader vector is empty therefore default handle is zero
	defaultShaderHandle = shaders.size();
	// create default shader
	// this currently only works with heap allocated shaders
	// so I have to have a vector of pointers
	// right now I don't know why
	shaders.push_back(new Shader(defaultVertexPath, defaultFragmentPath, status));
	// check id shader successfully generated
	if (!status) return;

	// generate vertex array object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

Objects::~Objects() {
	for (UINT_T i = 0; i < shaders.size(); ++i) {
		delete shaders[i];
	}
}


UINT_T Objects::newObject(const char* filePath, bool& status) {
	// get data from file
	UINT_T size;
	OBJ_Data objdata = OBJ_GenMesh(filePath, size, status);
	// check obj file loading succedded
	if (!status) return 0;

	// construct object data
	ObjectData data;
	// generate vertex buffer object
	glGenBuffers(1, &data.VBO);
	// buffer data to GPU
	glBindBuffer(GL_ARRAY_BUFFER, data.VBO);
	// pass size of mesh and position of first value
	glBufferData(GL_ARRAY_BUFFER, objdata.mesh.size() * sizeof(float), &objdata.mesh[0], GL_STATIC_DRAW);

	// transfer material data
	for (INT_T i = 0; i < objdata.materials.size(); ++i) {
		// create material data structure
		Material mat;
		// create alias for object data
		OBJ_Material& obj = objdata.materials[i];

		// transfer data
		for (INT_T j = 0; j < 3; ++j) {
			mat.ambient[j] = obj.ambient[j];
			mat.diffuse[j] = obj.diffuse[j];
			mat.specular[j] = obj.specular[j];
		}
		mat.alpha = obj.alpha;
		mat.specularExponent = obj.specularExponent;

		// create textures if applicable
		// ambient on unit 0, diffuse on unit 1, specular on uint 2
		if (obj.textureAmbient) {
			mat.textureAmbient = true;
			mat.ambientTextureHandle = textures.newTexture(obj.ambientTexturePath.c_str(), status);
			if (!status) return 0;
		}
		if (obj.textureDiffuse) {
			mat.textureDiffuse = true;
			mat.diffuseTextureHandle = textures.newTexture(obj.diffuseTexturePath.c_str(), status);
			if (!status) return 0;
		}
		if (obj.textureSpecular) {
			mat.textureSpecular = true;
			mat.specularTextureHandle = textures.newTexture(obj.specularTexturePath.c_str(), status);
			if (!status) return 0;
		}

		data.materials.push_back(mat);
	}

	// transfer material index data
	data.materialIndexes = objdata.matIndexes;

	data.nrVertices = size;

	// get object index
	UINT_T objectHandle = objects.size();
	// push object data to objects vector
	if (freeObjectIndexes.size()) {
		objects[freeObjectIndexes[0]] = data;
		objectHandle = freeObjectIndexes[0];
		freeObjectIndexes.erase(freeObjectIndexes.begin());
		freeObjectIndexes.shrink_to_fit();
	}
	else {
		objects.push_back(data);
	}

	//glBindBuffer(GL_ARRAY_BUFFER, data.VBO); // bind vbo of shape

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex attribute pointer
	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // vertex normal attribute pointer
	//glEnableVertexAttribArray(1);

	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture attribute pointer
	//glEnableVertexAttribArray(2);

	status = true;
	return objectHandle;
}

void Objects::deleteObject(UINT_T objectHandle) {
	ObjectData& object = objects[objectHandle];
	// deallocate vector memory
	object.materials.clear();
	object.materialIndexes.clear();
	object.materials.shrink_to_fit();
	object.materialIndexes.shrink_to_fit();
	object.exists = false;
	freeObjectIndexes.push_back(objectHandle);
}


void Objects::setPosition(UINT_T objectHandle, float *position) {
	for (INT_T i = 0; i < 3; ++i) {
		objects[objectHandle].position[i] = position[i];
	}
}

void Objects::setOrientation(UINT_T objectHandle, float* orientation) {
	for (INT_T i = 0; i < 3; ++i) {
		objects[objectHandle].orientation[i] = orientation[i];
	}
}

void Objects::setScale(UINT_T objectHandle, float scale) {
	objects[objectHandle].scale = scale;
}


void Objects::getPosition(UINT_T objectHandle, float* writeback) {
	for (INT_T i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].position[i];
	}
}

void Objects::getOrientation(UINT_T objectHandle, float* writeback) {
	for (INT_T i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].orientation[i];
	}
}

float Objects::getScale(UINT_T objectHandle) {
	return objects[objectHandle].scale;
}

UINT_T Objects::getSize(UINT_T objectHandle) {
	return objects[objectHandle].nrVertices;
}


void Objects::render(UINT_T objectHandle) {
	// create alias for object
	ObjectData& object = objects[objectHandle];
	Shader& shader = *shaders[defaultShaderHandle];

	shader.use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::make_vec3(object.position));
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();

	shader.setMat4("model", glm::value_ptr(model));
	shader.setMat4("view", glm::value_ptr(view));
	shader.setMat4("projection", glm::value_ptr(projection));

	glBindBuffer(GL_ARRAY_BUFFER, object.VBO); // bind vbo of shape

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertex attribute pointer
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // vertex normal attribute pointer
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture attribute pointer
	glEnableVertexAttribArray(2);

	// load materials
	for (INT_T i = 0; i < object.materialIndexes.size(); ++i) {
		// 3 because triangles
		int startVertex = object.materialIndexes[i].first;

		int nrVertices;
		if (i + 1 >= object.materialIndexes.size()) { // check if i is out of bounds
			nrVertices = object.nrVertices - object.materialIndexes[i].first;
		}
		else {
			nrVertices = object.materialIndexes[i + 1].first - object.materialIndexes[i].first;
		}


		if (object.materials[object.materialIndexes[i].second].textureDiffuse) {
			textures.bind(object.materials[object.materialIndexes[i].second].diffuseTextureHandle, 1);
			shader.setInt("texture0", 1);
			shader.setInt("textureDiffuse", 1);
		}
		else {
			shader.setVec3("color", object.materials[object.materialIndexes[i].second].diffuse);
			shader.setInt("textureDiffuse", 0);
		}

		glDrawArrays(GL_TRIANGLES, startVertex, nrVertices);
	}
}

void Objects::renderAll() {
	for (UINT_T i = 0; i < objects.size(); ++i) {
		if (objects[i].exists && objects[i].visible) {
			render(i);
		}
	}
}