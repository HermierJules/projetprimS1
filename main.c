#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
struct pixel{
	int r;
	int g;
	int b;
};
typedef struct pixel pixel;

struct image{
	int w;
	int h;
	pixel* pixels;
};
typedef struct image image;

struct point{
	int x;
	int y;
};
typedef struct point point;

pixel get_pixel(image i, int x, int y){
	return i.pixels[i.w * y + x];
}


int lightred = 0xFF8080;
int red = 0xFF0000;
int darkred = 0x800000;

int lightyellow = 0xFFFF80;
int yellow = 0xFFFF00;
int darkyellow = 0x808000;

int lightgreen = 0x80FF80;
int green = 0x00FF00;
int darkgreen = 0x008000;

int lightcyan = 0x80FFFF;
int cyan = 0x00FFFF;
int darkcyan = 0x008080;

int lightblue = 0x8080FF;
int blue = 0x0000FF;
int darkblue = 0x000080;

int lightmagenta = 0xFF80FF;
int magenta = 0xFF00FF;
int darkmagenta = 0x800080;



image read_ppm(char* filename) {
    FILE* fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Read the PPM header
    char magic[3];
    int width, height, max_val;

    // Read magic number (P6 for binary PPM)
    fscanf(fp, "%2s", magic);

    // Check if the file is a binary PPM
    if (magic[0] != 'P' || magic[1] != '6') {
        fprintf(stderr, "Invalid PPM file format: %s\n", filename);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // Skip comments
    char c = getc(fp);
    while (c == '#') {
        while (getc(fp) != '\n');  // Skip the rest of the line
        c = getc(fp);
    }
    ungetc(c, fp);  // Return the non-# character back to the stream

    // Read width, height, and max pixel value
    fscanf(fp, "%d %d %d", &width, &height, &max_val);

    // Allocate memory for the image structure
    image img;
    img.w = width;
    img.h = height;

    // Allocate memory for the pixels
    img.pixels = (pixel*)malloc(width * height * sizeof(pixel));

    // Check if memory allocation was successful
    if (img.pixels == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // Consume the newline character following the header
    getc(fp);

    // Read pixel data
	for(int i = 0; i < width * height; i++){
		int red = 0;
		int blue = 0;
		int green = 0;
    	fread(&red, 1, 1, fp);
    	fread(&green, 1, 1, fp);
    	fread(&blue, 1, 1, fp);
		printf("the %dth pixel:%d %d %d \n", i,red,green,blue);
		img.pixels[i].r = red;
		img.pixels[i].b = blue;
		img.pixels[i].g = green;
	}
	fclose(fp);
    return img;
}

bool pixel_eq(pixel p1, pixel p2){
	return  p1.r == p2.r && p1.g == p2.g && p1.b == p2.b; 
}

int normalize(int x, int y, image i){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	return y * i.w + x;
}



point rightedge_up(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = rightedge_up(i, start ,init, x, y+1, traite);
			if((p.x < v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v =rightedge_up(i, start,init, x, y-1, traite);
			if((p.x < v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =rightedge_up(i, start,init, x+1, y, traite);
			if((p.x < v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =rightedge_up(i, start,init, x-1, y, traite);
			if((p.x < v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
	}
	return p;
}


point rightedge_down(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = rightedge_down(i, start ,init, x, y+1, traite);
			if((p.x < v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v =rightedge_down(i, start,init, x, y-1, traite);
			if((p.x < v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =rightedge_down(i, start,init, x+1, y, traite);
			if((p.x < v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =rightedge_down(i, start,init, x-1, y, traite);
			if((p.x < v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
	}
	return p;
}



point leftedge_up(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = leftedge_up(i, start ,init, x, y+1, traite);
			if((p.x > v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v = leftedge_up(i, start,init, x, y-1, traite);
			if((p.x > v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =leftedge_up(i, start,init, x+1, y, traite);
			if((p.x > v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =leftedge_up(i, start,init, x-1, y, traite);
			if((p.x > v.x) || (p.x == v.x && p.y < v.y)) p = v;	
		}
	}
	return p;
}

point leftedge_down(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = leftedge_down(i, start ,init, x, y+1, traite);
			if((p.x > v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v = leftedge_down(i, start,init, x, y-1, traite);
			if((p.x > v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v = leftedge_down(i, start,init, x+1, y, traite);
			if((p.x > v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v = leftedge_down(i, start,init, x-1, y, traite);
			if((p.x > v.x) || (p.x == v.x && p.y > v.y)) p = v;	
		}
	}
	return p;
}

point downedge_right(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = downedge_right(i, start ,init, x, y+1, traite);
			if((p.y < v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v =downedge_right(i, start,init, x, y-1, traite);
			if((p.y < v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =downedge_right(i, start,init, x+1, y, traite);
			if((p.y < v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =downedge_right(i, start,init, x-1, y, traite);
			if((p.y < v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
	}
	return p;
}

point downedge_left(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = downedge_left(i, start ,init, x, y+1, traite);
			if((p.y < v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v =downedge_left(i, start,init, x, y-1, traite);
			if((p.y < v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =downedge_left(i, start,init, x+1, y, traite);
			if((p.y < v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =downedge_left(i, start,init, x-1, y, traite);
			if((p.y < v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
	}
	return p;
}

point upedge_left(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = upedge_left(i, start ,init, x, y+1, traite);
			if((p.y > v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v =upedge_left(i, start,init, x, y-1, traite);
			if((p.y > v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =upedge_left(i, start,init, x+1, y, traite);
			if((p.y > v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =upedge_left(i, start,init, x-1, y, traite);
			if((p.y > v.y) || (p.y == v.y && p.x > v.x)) p = v;	
		}
	}
	return p;
}

point upedge_right(image i, point start ,pixel init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h; 
	traite[y * i.w + x] = 1;
	point p = start;
	if(pixel_eq(get_pixel(i, x, y), init)) {
		if(!traite[normalize(x, y+1, i)]){
			point v = upedge_right(i, start ,init, x, y+1, traite);
			if((p.y > v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
		if(!traite[normalize(x, y-1, i)]){
			point v =upedge_right(i, start,init, x, y-1, traite);
			if((p.y > v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
		if(!traite[normalize(x+1, y, i)]){
			point v =upedge_right(i, start,init, x+1, y, traite);
			if((p.y > v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
		if(!traite[normalize(x-1, y, i)]){
			point v =upedge_right(i, start,init, x-1, y, traite);
			if((p.y > v.y) || (p.y == v.y && p.x < v.x)) p = v;	
		}
	}
	return p;
}
//0 nord
//1 est
//2 sud
//3 ouest
//0 babord
//1 tribord
void interprete(image i, int x, int y, int direction, int bord, stack* s){
		
}




void start(){
	image i = read_ppm("input.ppm");
	stack* s = create_stack();
	interprete(i,0,0, 1, 0, s);
}


int rgbtohtml(pixel p){
	return (p.r << 16) | (p.g << 8) | p.b;
}
 
int main() {
    char filename[] = "example.ppm";
    image img = read_ppm(filename);
	pixel p = get_pixel(img, 1, 6);
	int rg = rgbtohtml(p);
	printf("%#08x\n",rg);
	free(img.pixels);
    return 0;
}
