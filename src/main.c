#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#ifdef __WIN64
#include <windows.h>
#endif

#include "shader.h"
#include "image.h"
#include "dir_splore.h"
#include "icon.h"
#include "controls.h"

#define MARGIN 100
#define FPS 10

float vertices[20] = {
    // Position		    //Tex coords
    1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f};

unsigned int indices[6] = {0, 3, 1, 1, 3, 2};

app_data_t app_data = {0};
GLFWmonitor* monitor = NULL;

void glfw_resize_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    // Maintain the original size.
    // Keep the image in the center of the screen.
    app_data.v_x = (width - app_data.im_width) / 2;
    app_data.v_y = (height - app_data.im_height) / 2;
    glViewport(app_data.v_x, app_data.v_y, app_data.im_width, app_data.im_height);
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    // Zoom
    if (yoffset > 0) {
        app_data.scroll = 1;
    } else if (yoffset < 0) {
        app_data.scroll = -1;
    }
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    app_data.scale = 1.0f;
    // Zoom
    if ((key == GLFW_KEY_UP && action == GLFW_PRESS) || app_data.scroll == 1) {
        zoom_(ZOOM_IN, &app_data);
    }

    if ((key == GLFW_KEY_DOWN && action == GLFW_PRESS) || app_data.scroll == -1) {
        zoom_(ZOOM_OUT, &app_data);
    }
    // Move
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        move(RIGHT, &app_data);
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        move(LEFT, &app_data);
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        move(UP, &app_data);
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        move(DOWN, &app_data);
    }

    // Fullscreen
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        fullscreen(&app_data, window, monitor);
        usleep(100000);
    }

    // Next image
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        switch_image(NEXT, &app_data, window);
        usleep(100000);
    }

    // Previous image
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        switch_image(PREVIOUS, &app_data, window);
        usleep(100000);
    }
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

    uint32_t texture = get_image(filename, &app_data.im_width, &app_data.im_height);

    if (app_data.im_width == 0 || app_data.im_height == 0) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        return -1;
    }

    fprintf(stdout, "Image size: %dx%d\n", app_data.im_width, app_data.im_height);

    int32_t display_width = 0, display_height = 0;
    monitor = glfwGetPrimaryMonitor();
    if (monitor) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (mode) {
            display_width = mode->width - MARGIN;
            display_height = mode->height - MARGIN;
        }
    } else {
        fprintf(stderr, "Failed to get primary monitor\n");
        return -1;
    }

    if (display_width == 0 || display_height == 0) {
        fprintf(stderr, "Failed to get display size\n");
        return -1;
    }

    // Scale the image to fit the display
    float scale = 1.0f;

    if (app_data.im_width > display_width) {
        scale = (float)display_width / (float)app_data.im_width;
    }

    if (app_data.im_height > display_height) {
        float height_scale = (float)display_height / (float)app_data.im_height;
        if (height_scale < scale) {
            scale = height_scale;
        }
    }

    app_data.im_width *= scale;
    app_data.im_height *= scale;
    app_data.title = malloc(sizeof(char) * (strlen(filename) + sizeof("imeye - ")));
    sprintf(app_data.title, "imeye - %s", filename);
    GLFWwindow* window = glfwCreateWindow(app_data.im_width, app_data.im_height, app_data.title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    stbi_set_flip_vertically_on_load(false);
    GLFWimage icon;
    icon.width = icon_width;
    icon.height = icon_height;
    icon.pixels = icon_pixels;

    glfwSetWindowIcon(window, 1, &icon);
    stbi_set_flip_vertically_on_load(true);

    glfwMakeContextCurrent(window);

    // Focus the window
    glfwRequestWindowAttention(window);

#if defined(__WIN64) || defined(__WIN32)
    FreeConsole();
#endif

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

    int w, h;
    texture = get_image(filename, &w, &h);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLint tex_uniform = glGetUniformLocation(shader_program, "image");
    glUniform1i(tex_uniform, 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glfwSetFramebufferSizeCallback(window, glfw_resize_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetKeyCallback(window, glfw_key_callback);

    app_data.image_paths = list_images(filename);

    if (app_data.image_paths == NULL) {
        fprintf(stderr, "Failed to list app_data.image_paths\n");
        return -1;
    }

    for (size_t i = 0; app_data.image_paths[i] != NULL; i++) {
        app_data.image_count++;
        if (strcmp(app_data.image_paths[i], filename) == 0) {
            app_data.image_index = i;
        }
    }

    float time_start = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        app_data.scroll = 0;

        float time_end = glfwGetTime();
        float time_diff = time_end - time_start;
        if (time_diff < 1.0f / FPS) {
            usleep((1.0f / FPS - time_diff) * 1000000);
        }
    }

    glfwTerminate();
    free(app_data.title);
    return 0;
}
