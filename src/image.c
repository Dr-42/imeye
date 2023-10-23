#include "image.h"

#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

uint32_t get_image(const char* filename){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image == NULL) {
		printf("Failed to load image: %s\n", filename);
		return -1;
	}

	GLenum format;
	switch (channels) {
		case 1:
			format = GL_RED;
		break;
		case 2:
			format = GL_RG;
		break;
		case 3:
			format = GL_RGB;
		break;
		case 4:
			format = GL_RGBA;
		break;
		default:
			printf("Unsupported number of channels: %d\n", channels);
			return -1;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);
	return texture;
}
