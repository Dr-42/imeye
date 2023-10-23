#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "image.h"

float vertices[20] = {
    // Position		    //Tex coords
     1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
     1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
    -1.0f,  1.0f, -1.0f,    0.0f, 1.0f
};

unsigned int indices[6] = {
    0, 3, 1,
    1, 3, 2
};

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

    uint32_t texture = get_image(filename);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

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
