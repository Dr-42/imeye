#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

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

#define MARGIN 100
#define FPS 10

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
int32_t o_width = 0, o_height = 0;
int32_t v_x = 0, v_y = 0;
int8_t scroll = 0;
size_t image_index = 0;
bool fullscreen = false;

void glfw_resize_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    // Maintain the original size. 
    // Keep the image in the center of the screen.
    v_x = (width - o_width) / 2;
    v_y = (height - o_height) / 2;
    glViewport(v_x, v_y, o_width, o_height);
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    // Zoom
    if (yoffset > 0) {
        scroll = 1;
    } else if (yoffset < 0) {
        scroll = -1;
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

    uint32_t texture = get_image(filename, &o_width, &o_height);

    if (o_width == 0 || o_height == 0) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        return -1;
    }

    fprintf(stdout, "Image size: %dx%d\n", o_width, o_height);

    int32_t display_width = 0, display_height = 0;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (mode) {
            display_width = mode->width - MARGIN;
            display_height = mode->height - MARGIN;
        }
    }

    if (display_width == 0 || display_height == 0) {
        fprintf(stderr, "Failed to get display size\n");
        return -1;
    }

    // Scale the image to fit the display
    float scale = 1.0f;

    if (o_width > display_width) {
        scale = (float)display_width / (float)o_width;
    }
    
    if (o_height > display_height) {
        float height_scale = (float)display_height / (float)o_height;
        if (height_scale < scale) {
            scale = height_scale;
        }
    }

    o_width *= scale;
    o_height *= scale;
    char* title = malloc(sizeof(char) * (strlen(filename) + sizeof("imeye - ")));
    sprintf(title, "imeye - %s", filename);
    GLFWwindow* window = glfwCreateWindow(o_width, o_height, title, NULL, NULL);
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

    char** images = list_images(filename);

    if (images == NULL) {
        fprintf(stderr, "Failed to list images\n");
        return -1;
    }

    size_t num_images = 0;
    for (size_t i = 0; images[i] != NULL; i++) {
        num_images++;
        if(strcmp(images[i], filename) == 0) {
            image_index = i;
        }
    }

    float time_start = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        float l_scale = 1.0f;
        // Zoom
        if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) || scroll == 1) {
            l_scale += 0.01f;
            int prev_width = o_width;
            int prev_height = o_height;
            o_width *= l_scale;
            o_height *= l_scale;
            v_x -= (o_width - prev_width) / 2;
            v_y -= (o_height - prev_height) / 2;
            glViewport(v_x, v_y, o_width, o_height);
        }

        if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) || scroll == -1) {
            l_scale -= 0.01f;
            int prev_width = o_width;
            int prev_height = o_height;
            o_width *= l_scale;
            o_height *= l_scale;
            v_x -= (o_width - prev_width) / 2;
            v_y -= (o_height - prev_height) / 2;
            glViewport(v_x,v_y, o_width, o_height);
        }
        // Move
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            v_x += 1;
            glViewport(v_x, v_y, o_width, o_height);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            v_x -= 1;
            glViewport(v_x, v_y, o_width, o_height);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            v_y += 1;
            glViewport(v_x, v_y, o_width, o_height);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            v_y -= 1;
            glViewport(v_x, v_y, o_width, o_height);
        }

        // Fullscreen
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            if(!fullscreen){
                glfwSetWindowMonitor(window, monitor, 0, 0, display_width, display_height, GLFW_DONT_CARE);
            } else {
                glfwSetWindowMonitor(window, NULL, 0, 0, o_width, o_height, GLFW_DONT_CARE);
                glfwSetWindowPos(window, 100, 100);
            }
            fullscreen = !fullscreen;
            usleep(100000);
        }

        // Next image
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (num_images == 0 || num_images == 1) {
                continue;
            }
            image_index++;
            if (image_index >= num_images) {
                image_index = 0;
            }
            uint32_t prev_width = o_width;
            uint32_t prev_height = o_height;
            texture = get_image(images[image_index], &o_width, &o_height);
            // Scale the image to maintain aspect ratio and the scale of previous image
            float width_scale = (float)prev_width / (float)o_width;
            float height_scale = (float)prev_height / (float)o_height;

            if (width_scale < height_scale) {
                o_width *= width_scale;
                o_height *= width_scale;
            } else {
                o_width *= height_scale;
                o_height *= height_scale;
            }
            glViewport(v_x, v_y, o_width, o_height);
            free(title);
            title = malloc(sizeof(char) * (strlen(images[image_index]) + sizeof("imeye - ")));
            sprintf(title, "imeye - %s", images[image_index]);
            glfwSetWindowTitle(window, title);
            usleep(100000);
        }

        // Previous image
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (num_images == 0 || num_images == 1) {
                continue;
            }
            if (image_index == 0) {
                image_index = num_images - 1;
            } else {
                image_index--;
            }
            uint32_t prev_width = o_width;
            uint32_t prev_height = o_height;
            texture = get_image(images[image_index], &o_width, &o_height);
            // Scale the image to maintain aspect ratio and the scale of previous image
            float width_scale = (float)prev_width / (float)o_width;
            float height_scale = (float)prev_height / (float)o_height;

            if (width_scale < height_scale) {
                o_width *= width_scale;
                o_height *= width_scale;
            } else {
                o_width *= height_scale;
                o_height *= height_scale;
            }
            glViewport(v_x, v_y, o_width, o_height);
            free(title);
            title = malloc(sizeof(char) * (strlen(images[image_index]) + sizeof("imeye - ")));
            sprintf(title, "imeye - %s", images[image_index]);
            glfwSetWindowTitle(window, title);
            usleep(100000);
        }

        scroll = 0;

        float time_end = glfwGetTime();
        float time_diff = time_end - time_start;
        if (time_diff < 1.0f / FPS) {
            usleep((1.0f / FPS - time_diff) * 1000000);
        }
    }

    glfwTerminate();
    free(title);
    return 0;
}
