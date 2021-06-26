// header file for texture handling class

#ifndef TEXTURES_H
#define TEXTURES_H

#include "util.h"

class Textures {
private:
	// private function for returning the correct OpenGL mask
	// for the given colorspace
	bool colorSpace(int nrChannels, uint32_t& cSpaceMask);

public:
	// Function to generate new texture
	// must pass texture unit to bind texture to
	uint32_t newTexture(const char* filePath, uint32_t textureUnit, bool& status);

	// set active texture unit
	void active(uint32_t textureUnit);
	// bind texture to texture unit
	void bind(uint32_t textureHandle, uint32_t textureUnit);
};

#endif