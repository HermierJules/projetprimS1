#include "stb_image.h"

struct image{
	int w;
	int h;
	int* pixels;
};
typedef struct image image;

int rgbtohtml(int r, int g, int b);

image read_image(char* filename);
