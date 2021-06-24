#include <glad/glad.h>
#include "screen.h"


Screen::Screen(SDL_Window* window) {
	this->window = window;
}

void Screen::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Screen::swap() {
	SDL_GL_SwapWindow(window);
}

void Screen::setClearColor(float r, float g, float b, float a) {
	clearColor.bit.r = r;
	clearColor.bit.g = g;
	clearColor.bit.b = b;
	clearColor.bit.a = a;
	// function clamps values from 0 to 1
	glClearColor(r, g, b, a);
}

void Screen::setClearColor(float* color) {
	for (uint8_t i = 0; i < 4; ++i) clearColor.reg[i] = color[i];
	Color_Type& c = clearColor.bit;
	glClearColor(c.r, c.b, c.g, c.a);
}

void Screen::getClearColor(float& r, float& g, float& b, float& a) {
	Color_Type& c = clearColor.bit;
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}

void Screen::getClearColor(float* color) {
	for (uint8_t i = 0; i < 4; ++i) color[i] = clearColor.reg[i];
}

void Screen::setViewport(int32_t width, int32_t height) {
	glViewport(0, 0, width, height);
}