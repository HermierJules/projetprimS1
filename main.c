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
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	return i.pixels[i.w * y + x];
}

int rgbtohtml(pixel p){
	return (p.r << 16) | (p.g << 8) | p.b;
}

const int lightred = 0xFF8080;
const int red = 0xFF0000;
const int darkred = 0x800000;

const int lightyellow = 0xFFFF80;
const int yellow = 0xFFFF00;
const int darkyellow = 0x808000;

const int lightgreen = 0x80FF80;
const int green = 0x00FF00;
const int darkgreen = 0x008000;

const int lightcyan = 0x80FFFF;
const int cyan = 0x00FFFF;
const int darkcyan = 0x008080;

const int lightblue = 0x8080FF;
const int blue = 0x0000FF;
const int darkblue = 0x000080;

const int lightmagenta = 0xFF80FF;
const int magenta = 0xFF00FF;
const int darkmagenta = 0x800080;

int colortab[] = {lightred, red, darkred, lightyellow, yellow, darkyellow, lightgreen, green, darkgreen, lightcyan, cyan, darkcyan, lightblue, blue, darkblue, lightmagenta, magenta, darkmagenta};


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

bool is_passante(pixel p){
	return p.r * 0.202 + 0.707 * p.g + 0.071 * p.b >= 128. ;
}

bool is_block_color(pixel p){
	int htcode = rgbtohtml(p);
	for(int i = 0; i < 18; i++){
		if(colortab[i] ==  htcode) return true;
	}
	return false;
}



point get_next_pixel_edge(image i, int x, int y, int direction, int bord){
	point p;
	p.x = x;
	p.y = y;
	bool* traite = malloc(sizeof(bool) * i.h * i.w);
	if(direction == 1){
		if(bord == 0) return rightedge_up(i, p, get_pixel(i, x, y), x, y, traite);
		return rightedge_down(i, p, get_pixel(i, x, y), x, y, traite);
	}
	if(direction == 2){
		if(bord == 0) return downedge_right(i, p, get_pixel(i, x, y), x, y, traite);
		return downedge_left(i, p, get_pixel(i, x, y), x, y, traite);
	}
	if(direction == 3){
		if(bord == 0) return leftedge_down(i, p, get_pixel(i, x, y), x, y, traite);
		return leftedge_up(i, p, get_pixel(i, x, y), x, y, traite);
	}
	if(direction == 4) {
		if(bord == 0) return upedge_left(i, p, get_pixel(i, x, y), x, y, traite);
		return upedge_right(i, p, get_pixel(i, x, y), x, y, traite);
	}
	return rightedge_up(i, p, get_pixel(i, x, y), x, y, traite);
}



point get_next_block(image i, int x, int y,int* bord, int direction, int count){
	int b = *bord;
	int ox = x;
	int oy = y;
	int dx;
	int dy;
	if(direction == 0){
		dx = 0;
		dy = -1;
	}
	if(direction == 1){
		dx = 1;
		dy = 0;
	}
	if(direction == 2){
		dx = 0;
		dy = 1;
	}
	if(direction == 3){
		dx = -1;
		dy = 0;
	}
	bool check = true;
	while(check){
		x = x + dx;
		y = y + dy;
		pixel p = get_pixel(i, x, y);
		point pp;
		pp.x = x;
		pp.y = y;
		if(is_block_color(p)) return pp;
		if(is_passante(p)) continue;
		else{
			if(count == 8){
				exit(1);	
			}
			else{
				*bord = (b + 2) % 4;
				return get_next_block(i, ox, oy, bord, direction, count+1);
			}
		}
	}
}



//0 nord
//1 est
//2 sud
//3 ouest
//0 babord
//1 tribord
void interprete(image i, int x, int y, int direction, int bord, stack* s){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	
		
}




void start(){
	image i = read_ppm("input.ppm");
	stack* s = create_stack();
	interprete(i,0,0, 1, 0, s);
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
