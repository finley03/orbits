#include "csys.h"
extern "C" {
#include "mat.h"
}


Csys::Csys(Camera* camera, INT_T width, INT_T height) :
	shader(csysVert, csysFrag, shaderStatus),
	screen(width, height, screenStatus) {
	//printf("%s\n", shaderStatus ? "Compile successful" : "Compile failed");
	// generate VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	this->camera = camera;

	//projection = glm::ortho(-5.0f, (float)width / 5, -5.0f, (float)height / 5, 0.1f, 30.0f);
	projection = glm::ortho(-1.5f, 1.5f, 1.5f, -1.5f, 0.1f, 2.0f);

	screen.setViewport(width, height);
	screen.setClearColor(0.1f, 0.1f, 0.1f, 0.0f);
}


void Csys::draw() {
	screen.clear();

	shader.use();

	// pull data from camera class
	float position[3], target[3], up[2];
	float viewPosition[3], viewTarget[3] = {0, 0, 0};
	camera->getPosition(position);
	camera->getTarget(target);
	camera->getUp(up);
	mat_subtract(position, target, 3, viewPosition);
	vec_3_normalize(viewPosition, viewPosition);

	//printf("%f, %f, %f\n", viewPosition[0], viewPosition[1], viewPosition[2]);

	view = glm::lookAt(glm::make_vec3(viewPosition), glm::make_vec3(viewTarget), glm::vec3(glm::make_vec2(up), 0.0f));
	shader.setMat4("view", glm::value_ptr(view));
	shader.setMat4("projection", glm::value_ptr(projection));

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// create vertex attribute pointer to position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// create vertex attribute pointer to color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_LINES, 0, 6);
}


uint32_t Csys::getTexHandle() {
	return screen.getTexture();
}


glm::mat4 Csys::getViewMatrix() {
	return view;
}

glm::mat4 Csys::getProjectionMatrix() {
	return projection;
}