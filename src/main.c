#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

float vertices[20] = {
	// Position					//Tex coords
	  1.0f,   1.0f, -1.0f,		 1.0f,  1.0f,
	  1.0f,  -1.0f, -1.0f,		 1.0f, 0.0f,
	 -1.0f,  -1.0f, -1.0f,		 0.0f, 0.0f,
	 -1.0f,   1.0f, -1.0f,		 0.0f,  1.0f,
};

unsigned int indices[6] = {
	0, 3, 1,
	1, 3, 2
};

const char* vert_shad = 
				"#version 330 core\n"
				"layout (location = 0) in vec3 vertex;\n"
				"layout (location = 1) in vec2 texCoord;\n"
				"out vec2 TexCoords;\n"
				"void main()\n"
				"{\n"
					"TexCoords = texCoord;\n"
					"gl_Position = vec4(vertex, 1.0);\n"
				"}";

const char* frag_shad =
				"#version 330 core\n"
				"in vec2 TexCoords;\n"
				"out vec4 color;\n"
				"uniform sampler2D image;\n"
				"void main()\n"
				"{   \n"
					"vec4 texColor = texture(image, TexCoords);\n"
					"if (texColor.a < 0.1)\n"
						"discard;\n"
					"color = texColor;\n"
				"}";


GLuint get_shader(){
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_shad, NULL);
	glCompileShader(vertex_shader);

	GLint vertex_shader_compile_status;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compile_status);
	if (vertex_shader_compile_status != GL_TRUE) {
		printf("Failed to compile vertex shader\n");
		return -1;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_shad, NULL);
	glCompileShader(fragment_shader);

	GLint fragment_shader_compile_status;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compile_status);
	if (fragment_shader_compile_status != GL_TRUE) {
		printf("Failed to compile fragment shader\n");
		return -1;
	}

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint shader_program_link_status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_program_link_status);
	if (shader_program_link_status != GL_TRUE) {
		printf("Failed to link shader program\n");
		return -1;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

int main(int argc, char** argv) {
	const char* filename;
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return -1;
	} else {
		filename = argv[1];
	}

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Image Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint shader_program = get_shader();
	glUseProgram(shader_program);

	GLint pos_attrib = glGetAttribLocation(shader_program, "vertex");
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(pos_attrib);

	GLint tex_attrib = glGetAttribLocation(shader_program, "texCoord");
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(tex_attrib);

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
	
	GLint tex_uniform = glGetUniformLocation(shader_program, "image");
	glUniform1i(tex_uniform, 0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
    }

    glfwTerminate();
    return 0;
}
