#include "controls.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "image.h"

float get_scale(uint32_t prev_width, uint32_t prev_height, uint32_t width, uint32_t height);

void zoom_(zoom_t zoom, app_data_t* app_data) {
    if (zoom == ZOOM_IN) {
        app_data->scale += 0.01f;
    } else if (zoom == ZOOM_OUT) {
        app_data->scale -= 0.01f;
    } else {
        fprintf(stderr, "Invalid zoom value\n");
        exit(EXIT_FAILURE);
    }
    int prev_width = app_data->im_width;
    int prev_height = app_data->im_height;
    app_data->im_width *= app_data->scale;
    app_data->im_height *= app_data->scale;
    app_data->v_x -= (app_data->im_width - prev_width) / 2;
    app_data->v_y -= (app_data->im_height - prev_height) / 2;
    glViewport(app_data->v_x, app_data->v_y, app_data->im_width, app_data->im_height);
}

void move(direction_t direction, app_data_t* app_data) {
    switch (direction) {
        case UP:
            app_data->v_y += 1;
            break;
        case DOWN:
            app_data->v_y -= 1;
            break;
        case LEFT:
            app_data->v_x -= 1;
            break;
        case RIGHT:
            app_data->v_x += 1;
            break;
        default:
            fprintf(stderr, "Invalid direction value\n");
            exit(EXIT_FAILURE);
    }
    glViewport(app_data->v_x, app_data->v_y, app_data->im_width, app_data->im_height);
}

void fullscreen(app_data_t* app_data, GLFWwindow* window, GLFWmonitor* monitor) {
    int display_width = glfwGetVideoMode(monitor)->width;
    int display_height = glfwGetVideoMode(monitor)->height;
    if (!app_data->fullscreen) {
        glfwSetWindowMonitor(window, monitor, 0, 0, display_width, display_height, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(window, NULL, 0, 0, app_data->im_width, app_data->im_height, GLFW_DONT_CARE);
        glfwSetWindowPos(window, 100, 100);
    }
    app_data->fullscreen = !app_data->fullscreen;
}

void switch_image(control_t control, app_data_t* app_data, GLFWwindow* window) {
    if (app_data->image_count == 0 || app_data->image_count == 1) {
        return;
    }
    if (control == NEXT) {
        app_data->image_index++;
        if (app_data->image_index >= app_data->image_count) {
            app_data->image_index = 0;
        }
    } else if (control == PREVIOUS) {
        if (app_data->image_index == 0) {
            app_data->image_index = app_data->image_count - 1;
        } else {
            app_data->image_index--;
        }
    } else {
        fprintf(stderr, "Invalid control value\n");
        exit(EXIT_FAILURE);
    }
    uint32_t prev_width = app_data->im_width;
    uint32_t prev_height = app_data->im_height;
    app_data->texture = get_image(app_data->image_paths[app_data->image_index], &app_data->im_width, &app_data->im_height);
    // Scale the image to maintain aspect ratio and the scale of previous image
    float new_scale = get_scale(prev_width, prev_height, app_data->im_width, app_data->im_height);
    app_data->im_width *= new_scale;
    app_data->im_height *= new_scale;
    // Center the image
    int32_t prev_center_x = app_data->v_x + prev_width / 2;
    int32_t prev_center_y = app_data->v_y + prev_height / 2;
    app_data->v_x = prev_center_x - app_data->im_width / 2;
    app_data->v_y = prev_center_y - app_data->im_height / 2;
    glViewport(app_data->v_x, app_data->v_y, app_data->im_width, app_data->im_height);
    free(app_data->title);
    app_data->title = malloc(sizeof(char) * (strlen(app_data->image_paths[app_data->image_index]) + sizeof("imeye - ")));
    sprintf(app_data->title, "imeye - %s", app_data->image_paths[app_data->image_index]);
    glfwSetWindowTitle(window, app_data->title);
}

float get_scale(uint32_t prev_width, uint32_t prev_height, uint32_t width, uint32_t height) {
    float prev_ar = (float)prev_width * (float)prev_height;
    float ar = (float)width * (float)height;
    float scale = sqrtf(prev_ar) / sqrtf(ar);
    return scale;
}
