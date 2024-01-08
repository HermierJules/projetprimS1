#include "stb_image.h"
#include "read.h" 
int rgbtohtml(int r, int g, int b){
	return (r << 16) | (g << 8) | b;
}

image read_image(char* filename){
	int w, h, ch;
	unsigned char* image_data = stbi_load(filename, &w, &h, &ch, 0);
	if(!image_data){
		fprintf(stderr, "Couldn't load the image: %s\n", stbi_failure_reason());
		exit(1);
	}
	image i;
	i.w = w;
	i.h = h;
	i.pixels = malloc(sizeof(int) * w * h);
	for(int x = 0; x < w; x++){
		for(int y = 0; y < h; y++){
			int pixel_index = (y * w + x) * ch;
			int r = image_data[pixel_index ];
			int g = image_data[pixel_index + 1];
			int b = image_data[pixel_index + 2];
			i.pixels[y * w + x] = rgbtohtml(r,g,b);
		}
	}
	stbi_image_free(image_data);
	return i;
}
