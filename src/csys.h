#ifndef CSYS_H
#define CSYS_H

#include "util.h"
#include "camera.h"
#include "shaders.h"
#include "screen.h"

class Csys {
private:
	// vertex buffer object
	unsigned int VBO;
	// paths to shaders
	const char* csysVert = "./shaders/csys.vert";
	const char* csysFrag = "./shaders/csys.frag";
	bool shaderStatus;

	// shader object
	Shader shader;

	// screen object
	Screen screen;
	bool screenStatus;

	// pointer to camera object
	Camera* camera;

	// data for CSYS
	// 3 floats for position, 4 for color
	const float vertices[42] = {
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	};

	glm::mat4 view;
	// store orthographic projection matrix here
	glm::mat4 projection;
public:
	Csys(Camera* camera, INT_T width, INT_T height);

	~Csys();

	void draw();

	uint32_t getTexHandle();

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
};

#endif