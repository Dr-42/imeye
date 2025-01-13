#include "dir_splore.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

char* images[1024];

char* parent_directory(const char* filepath) {
	size_t last_slash = 0;
	size_t file_len = strlen(filepath);
	for (size_t i = file_len; i > 0; i--) {
		if (filepath[i] == '/' || filepath[i] == '\\') {
			last_slash = i;
			break;
		}
	}

	if (last_slash == 0) {
		char* dot = malloc(2);
		dot[0] = '.'; dot[1] = '\0';
		return dot;
	}

	char* parent = malloc(last_slash + 1);
	memcpy(parent, filepath, last_slash);
	parent[last_slash] = '\0';
	return parent;
}

int compare_strings(const void* a, const void* b) {
	char* a_copy = strdup(*(char**)a);
	char* b_copy = strdup(*(char**)b);

	size_t a_len = strlen(a_copy);
	size_t b_len = strlen(b_copy);

	if (a_len > 4 && b_len > 4) {
		a_copy[a_len - 4] = '\0';
		b_copy[b_len - 4] = '\0';
	}

	int result = strcmp(a_copy, b_copy);
	free(a_copy);
	free(b_copy);
	return result;
}

char** list_images(const char* filepath) {
	char* directory = parent_directory(filepath);
	DIR* dir = opendir(directory);
	if (dir == NULL) {
		fprintf(stderr, "Could not open directory %s\n", directory);
		return NULL;
	}

	struct dirent* entry;
	
	size_t i = 0;
	while ((entry = readdir(dir)) != NULL) {
		const char* name = entry->d_name;
		// Check if the file is an image
		// Extensions: .png, .jpg, .jpeg, .bmp, .gif, .tga, .svg
		size_t len = strlen(name);
		if (len > 4) {
			if (strcmp(name + len - 4, ".png") == 0 ||
				strcmp(name + len - 4, ".jpg") == 0 ||
				strcmp(name + len - 5, ".jpeg") == 0 ||
				strcmp(name + len - 4, ".bmp") == 0 ||
				strcmp(name + len - 4, ".PNG") == 0 ||
				strcmp(name + len - 4, ".JPG") == 0 ||
				strcmp(name + len - 5, ".JPEG") == 0 ||
				strcmp(name + len - 4, ".BMP") == 0 ||
				images[i] = malloc(strlen(directory) + strlen(name) + 2);
				strcpy(images[i], directory);
				#if defined(_WIN32) || defined(_WIN64)
					strcat(images[i], "\\");
				#else
					strcat(images[i], "/");
				#endif
				strcat(images[i], name);
				i++;
			}
		}
	}

	closedir(dir);
	free(directory);

	// Sort the images alphabetically
	qsort(images, i, sizeof(char*), compare_strings);
	return images;
}
