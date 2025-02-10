#include "shader.h"

#include <GL/glew.h>
#include <stdio.h>

const char* vert_shad = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertex;\n"
    "layout (location = 1) in vec2 texCoord;\n"
    "out vec2 TexCoords;\n"
    "uniform float rotation_angle;\n"
    "void main()\n"
    "{\n"
    "   float rad = radians(rotation_angle);\n"
    "   mat4 rotation = mat4(\n"
    "       cos(rad), sin(rad), 0.0, 0.0,\n"
    "       -sin(rad), cos(rad), 0.0, 0.0,\n"
    "       0.0, 0.0, 1.0, 0.0,\n"
    "       0.0, 0.0, 0.0, 1.0\n"
    "   );\n"
    "   vec4 pos = rotation * vec4(vertex, 1.0);\n"
    "   TexCoords = texCoord;\n"
    "   gl_Position = pos;\n"
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


uint32_t get_shader(){
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_shad, NULL);
	glCompileShader(vertex_shader);

	GLint vertex_shader_compile_status;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compile_status);
	if (vertex_shader_compile_status != GL_TRUE) {
		fprintf(stderr, "Failed to compile vertex shader\n");
		return -1;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_shad, NULL);
	glCompileShader(fragment_shader);

	GLint fragment_shader_compile_status;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_compile_status);
	if (fragment_shader_compile_status != GL_TRUE) {
		fprintf(stderr, "Failed to compile fragment shader\n");
		return -1;
	}

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	GLint shader_program_link_status;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_program_link_status);
	if (shader_program_link_status != GL_TRUE) {
		fprintf(stderr, "Failed to link shader program\n");
		return -1;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

