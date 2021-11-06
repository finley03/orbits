#ifndef GRID_H
#define GRID_H

#include "util.h"
#include "shaders.h"
#include "camera.h"

class Grid {
private:
	unsigned int VBO;

	const char* gridVert = "./shaders/grid.vert";
	const char* gridFrag = "./shaders/grid.frag";
	bool shaderStatus;

	Shader shader;
	Camera* camera;

	const float vertices[18] = {
	1.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, -1.0f
	};

	glm::vec3 position;
	float scale;
	glm::mat4 model;
public:
	Grid(Camera* camera);

	void setPosition(float* position);
	void setSize(float scale);
	void calculateModelMatrix();

	void draw();

	bool show = true;

};

#endif