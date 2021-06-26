// source file for texure class

#include "textures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad/glad.h"


bool Textures::colorSpace(int nrChannels, uint32_t& cSpaceMask) {
	switch (nrChannels) {
	case 3:
		cSpaceMask = GL_RGB;
		return true;

	case 4:
		cSpaceMask = GL_RGBA;
		return true;

	default:
		std::cout << "Error: texture uses unsupported colorspace " << nrChannels << "\n";
		return false;
	}
}


uint32_t Textures::newTexture(const char* filePath, uint32_t textureUnit, bool& status) {
	// load texture from file
	int texWidth, texHeight, nrChannels;
	uint32_t cSpace;
	uint8_t* data = stbi_load(filePath, &texWidth, &texHeight, &nrChannels, 0);
	// check colorspace is correct
	if (!colorSpace(nrChannels, cSpace)) {
		// free data and return
		stbi_image_free(data);
		status = false;
		return 0;
	}

	// select active texture unit
	glActiveTexture(GL_TEXTURE0 + textureUnit);

	// texture handle writeback
	uint32_t texture;
	// generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// check data loaded
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, cSpace, texWidth, texHeight, 0, cSpace, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Error: " << filePath << " failed to load\n";
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	std::cout << "loaded texture\n";

	status = true;
	return texture;
}

void Textures::active(uint32_t textureUnit) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
}

void Textures::bind(uint32_t textureHandle, uint32_t textureUnit) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}