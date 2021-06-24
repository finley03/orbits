// class for managing the graphics renderbuffer

#ifndef SCREEN_H
#define SCREEN_H

#include <SDL.h>
#include "util.h"

// Class for handling the rendering viewport.
// Contains functions for configuring, clearing,
// and swapping the screen
class Screen {
private:
	SDL_Window* window;

	// Color type for use in getClearColor() function
	Color clearColor;
public:
	// Pass a pointer to the window object to the constructor.
	Screen(SDL_Window* window);
	// Clears the screen using the pre set clear color
	void clear();
	// Swaps the current renderbuffer on to the screen
	void swap();
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