// class for managing the graphics renderbuffer

#ifndef SCREEN_H
#define SCREEN_H

#include <SDL.h>
#include "util.h"
#include "shaders.h"

// Class for handling the rendering viewport.
// Contains functions for configuring, clearing,
// and swapping the screen
// uses off screen rendering for future post-processing
class Screen {
private:
	uint32_t IFBO;
	// opengl framebuffer object handle
	uint32_t FBO;
	// renderbuffer object handle
	uint32_t RBO;
	// screen texture handle
	uint32_t SCREENTEX;
	// multisample buffer
	uint32_t MSAABUFFER;

	// vertex buffer object for quadilerateral
	uint32_t VBO;

	// screen size
	INT_T width, height;

	// vertices for quad to render to
	const float vertices[24] = {
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f
	};

	const char* vertexFilePath = "./shaders/screen.vert";
	const char* fragmentFilePath = "./shaders/screen.frag";

	bool shaderStatus;
	Shader screenShader;

	// texture unit number
	INT_T textureUnit;

	SDL_Window* window;

	// Color type for use in getClearColor() function
	Color clearColor;
public:
	// Pass a pointer to the window object to the constructor.
	//Screen(SDL_Window* window);
	// constructor for screen for full screen
	Screen(SDL_Window* window, bool& success);
	// constructor for texture (window)
	Screen(INT_T width, INT_T height, bool& success);
	// destuctor
	~Screen();
	// Clears the screen using the pre set clear color
	void clear();
	// Swaps the current renderbuffer on to the screen
	void swap();
	// get texture
	uint32_t getTexture();
	// Sets the clear color of the screen
	void setClearColor(float r, float g, float b, float a);
	// Sets the clear color of the screen
	void setClearColor(float* color);
	// Returns the clear color of the screen
	void getClearColor(float& r, float& g, float& b, float& a);
	// Returns the clear color of the screen
	void getClearColor(float* color);
	// Sets the dimensions of the rendering viewport
	void setViewport(int32_t width, int32_t height);
};

#endif