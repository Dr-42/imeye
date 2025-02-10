#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum zoom_t {
	ZOOM_IN,
	ZOOM_OUT
} zoom_t;

typedef enum move_direction_t { 
	UP,
	DOWN,
	LEFT,
	RIGHT
} direction_t;

typedef enum rotate_direction_t {
	CLOCKWISE,
	ANTICLOCKWISE
} rotate_direction_t;

typedef enum control_t {
	NEXT,
	PREVIOUS
} control_t;

typedef struct display_scale_t {
	float x_scale;
	float y_scale;
} display_scale_t;

typedef struct app_data_t {
	GLFWmonitor* monitor;
	GLFWwindow* window;
	char* title;
	uint32_t texture;
	int32_t im_width;
	int32_t im_height;
	int32_t v_x;
	int32_t v_y;
	int32_t rotation;
	int8_t scroll;
	size_t image_index;
	size_t image_count;
	char** image_paths;
	bool fullscreen;
	float scale;
} app_data_t;

void zoom_(zoom_t zoom, app_data_t* app_data);
void move(direction_t direction, app_data_t* app_data);
void fullscreen(app_data_t* app_data, GLFWwindow* window, GLFWmonitor* monitor);
void switch_image(control_t control, app_data_t* app_data, GLFWwindow* window);
int reset_viewer(app_data_t* app_data);
void rotate(rotate_direction_t direction, app_data_t* app_data);
