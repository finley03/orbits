// source file for objects class

#include "objects.h"
//#include "obj.h"
#include "simulation.h"
extern "C" {
#include "mat.h"
}

//extern std::vector<Simulation*> simulations;
//extern INT_T activeSimulation;

Objects::Objects(void* parent, bool& status) : textures() {// , shader(defaultVertexPath, defaultFragmentPath, status) {
	simulationptr = parent;
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

	for (auto it = this->begin(); it < this->end(); ++it) {
		deleteObject(*it);
	}
}


UINT_T Objects::newObject(const char* filePath, bool& status) {
	// get data from file
	UINT_T sizeVertices;
	OBJ_Data objdata = OBJ_GenMesh(filePath, sizeVertices, status);
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

	data.nrVertices = sizeVertices;
	data.cRadius = cRadius;

	data.exists = true;

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

	status = true;
	return objectHandle;
}

void Objects::newObjectT(const char* filePath) {
	threadOpen = true;
	// get data from file
	objdata = OBJ_GenMesh(filePath, sizeVertices, status);
	// check obj file loading succedded

	// transfer material data
	for (INT_T i = 0; i < objdata.materials.size(); ++i) {
		// create alias for object data
		OBJ_Material& obj = objdata.materials[i];

		// create textures if applicable
		// ambient on unit 0, diffuse on unit 1, specular on uint 2
		if (obj.textureAmbient) {
			//mat.ambientTextureHandle = textures.newTexture(obj.ambientTexturePath.c_str(), status);
			status = textures.loadTextureToBuffer(obj.ambientTexturePath.c_str(), ambientBuffer);
			//if (!status) return 0;
		}
		if (obj.textureDiffuse) {
			//mat.diffuseTextureHandle = textures.newTexture(obj.diffuseTexturePath.c_str(), status);
			status = textures.loadTextureToBuffer(obj.diffuseTexturePath.c_str(), diffuseBuffer);
			//if (!status) return 0;
		}
		if (obj.textureSpecular) {
			//mat.specularTextureHandle = textures.newTexture(obj.specularTexturePath.c_str(), status);
			status = textures.loadTextureToBuffer(obj.specularTexturePath.c_str(), specularBuffer);
			//if (!status) return 0;
		}
	}


	threadDone = true;

	return;
}

void Objects::newObjectThread(const char* filePath) {
	if (!threadOpen) {
		objectThread = std::thread(&Objects::newObjectT, this, filePath);
	}
}

bool Objects::getThreadDone() {
	return threadDone;
}

bool Objects::getThreadOpen() {
	return threadOpen;
}

UINT_T Objects::joinThread(bool& status) {
	UINT_T objectHandle = 0;
	if (threadOpen) {
		threadOpen = false;
		objectThread.join();
		threadDone = false;


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
				//mat.ambientTextureHandle = textures.newTexture(obj.ambientTexturePath.c_str(), status);
				mat.ambientTextureHandle = textures.newTextureFromBuffer(ambientBuffer, status);
				//if (!status) return 0;
			}
			if (obj.textureDiffuse) {
				mat.textureDiffuse = true;
				//mat.diffuseTextureHandle = textures.newTexture(obj.diffuseTexturePath.c_str(), status);
				mat.diffuseTextureHandle = textures.newTextureFromBuffer(diffuseBuffer, status);
				//if (!status) return 0;
			}
			if (obj.textureSpecular) {
				mat.textureSpecular = true;
				//mat.specularTextureHandle = textures.newTexture(obj.specularTexturePath.c_str(), status);
				mat.specularTextureHandle = textures.newTextureFromBuffer(specularBuffer, status);
				//if (!status) return 0;
			}

			data.materials.push_back(mat);
		}

		// transfer material index data
		data.materialIndexes = objdata.matIndexes;

		data.nrVertices = sizeVertices;
		data.cRadius = cRadius;

		data.exists = true;

		// get object index
		objectHandle = objects.size();
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

		status = true;


	}
	//return threadObjectHandle;
	return objectHandle;
}


void Objects::deleteObject(UINT_T objectHandle) {
	ObjectData& object = objects[objectHandle];
	// deallocate vector memory
	object.materials.clear();
	object.materialIndexes.clear();
	object.materials.shrink_to_fit();
	object.materialIndexes.shrink_to_fit();

	// delete vertex buffer object
	glDeleteBuffers(1, &object.VBO);

	// delete textures
	for (auto it = object.materials.begin(); it != object.materials.end(); ++it) {
		Material& mat = *it;
		if (mat.textureAmbient) textures.deleteTexture(mat.ambientTextureHandle);
		if (mat.textureDiffuse) textures.deleteTexture(mat.diffuseTextureHandle);
		if (mat.textureSpecular) textures.deleteTexture(mat.specularTextureHandle);
	}

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

void Objects::setName(UINT_T objectHandle, std::string name) {
	objects[objectHandle].name = name;
}

void Objects::setVisible(UINT_T objectHandle, bool visible) {
	objects[objectHandle].visible = visible;
}

void Objects::setMass(UINT_T objectHandle, float mass) {
	objects[objectHandle].mass = mass;
}

void Objects::setVelocity(UINT_T objectHandle, float* velocity) {
	for (INT_T i = 0; i < 3; ++i) {
		objects[objectHandle].velocity[i] = velocity[i];
	}
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

std::string Objects::getName(UINT_T objectHandle) {
	return objects[objectHandle].name;
}

bool Objects::getVisible(UINT_T objectHandle) {
	return objects[objectHandle].visible;
}

float Objects::getMass(UINT_T objectHandle) {
	return objects[objectHandle].mass;
}

void Objects::getVelocity(UINT_T objectHandle, float* writeback) {
	for (INT_T i = 0; i < 3; ++i) {
		writeback[i] = objects[objectHandle].velocity[i];
	}
}

float Objects::getRadius(UINT_T objectHandle) {
	return objects[objectHandle].cRadius * objects[objectHandle].scale;
}


void Objects::render(UINT_T objectHandle) {
	//Simulation& simulation = *simulations[activeSimulation];
	Simulation& simulation = *(Simulation*)simulationptr;
	// create alias for object
	ObjectData& object = objects[objectHandle];
	Shader& shader = *shaders[defaultShaderHandle];

	shader.use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::make_vec3(object.position));
	// rotate according to north east down frame euler angles
	// this will likely change later, this is just maths I already have working.
	model = glm::rotate(model, glm::radians(object.orientation[2]), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(object.orientation[1]), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(object.orientation[0]), glm::vec3(0.0f, 0.0f, -1.0f));
	float& scale = object.scale;
	model = glm::scale(model, glm::vec3(scale, scale, scale));

	glm::mat4 view = simulation.camera.getViewMatrix();
	glm::mat4 projection = simulation.camera.getProjectionMatrix();

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


// iterator operator overloading

Objects::iterator& Objects::iterator::operator++() {
	INT_T i;
	for (i = this->n + 1; i < parent->objects.size(); ++i) {
		if (parent->objects[i].exists) {
			this->n = i;
			break;
		}
	}
	// if no valid next object is found, this->n is force incremented
	// to trigger a loop break in the caller
	if (this->n != i) ++this->n;
	return *this;
}

Objects::iterator Objects::iterator::operator++(int) {
	iterator temp = *this;
	INT_T i;
	for (i = this->n + 1; i < parent->objects.size(); ++i) {
		if (parent->objects[i].exists) {
			this->n = i;
			break;
		}
	}
	// if no valid next object is found, this->n is force incremented
	// to trigger a loop break in the caller
	if (this->n != i)++this->n;
	return temp;
}

bool Objects::iterator::operator==(const Objects::iterator& i2) {
	return (this->n == i2.n);
}

bool Objects::iterator::operator!=(const Objects::iterator& i2) {
	return (this->n != i2.n);
}

bool Objects::iterator::operator<(const Objects::iterator& i2) {
	return (this->n < i2.n);
}

UINT_T& Objects::iterator::operator*() {
	return this->n;
}


// iterator functions

Objects::iterator Objects::begin() {
	iterator it;
	it.parent = this;
	// find the first existing object
	for (UINT_T n = 0; n < objects.size(); ++n) {
		if (objects[n].exists) {
			it.n = n;
			break;
		}
	}
	return it;
}

Objects::iterator Objects::end() {
	iterator it;
	it.parent = this;
	//it.n = objects.size() - 1;
	for (INT_T n = objects.size() - 1; n >= 0; --n) {
		if (objects[n].exists) {
			it.n = _UINT(n) + 1;
			break;
		}
	}
	return it;
}

size_t Objects::size() {
	size_t count = 0;
	for (auto it = this->begin(); it != this->end(); ++it) ++count;
	return count;
}