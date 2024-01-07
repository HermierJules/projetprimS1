#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1];
	printf("%s\n", file_path);
    int width, height, channels;

    unsigned char *image_data = stbi_load(file_path, &width, &height, &channels, 0);

    if (!image_data) {
        fprintf(stderr, "Error loading image: %s\n", stbi_failure_reason());
        return 1;
    }

    printf("Image Width: %d\n", width);
    printf("Image Height: %d\n", height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixel_index = (y * width + x) * channels;

            printf("Pixel at (%d, %d): ", x, y);
            for (int c = 0; c < channels; c++) {
                printf("c: %d ", image_data[pixel_index + c]);
            }
            printf("\n");
        }
    }

    stbi_image_free(image_data);
    return 0;
}

