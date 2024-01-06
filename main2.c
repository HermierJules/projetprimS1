#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "operations.h"
#include "raylib.h"
#include <unistd.h>

struct image{
	int w;
	int h;
	int* pixels;
};
typedef struct image image;

struct point{
	int x;
	int y;
};
typedef struct point point;


int rgbtohtml(int r, int g, int b){
	return (r << 16) | (g << 8) | b;
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
    img.pixels = (int*)malloc(width * height * sizeof(int));

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
		img.pixels[i] = rgbtohtml(red,green,blue);
	}
	fclose(fp);
    return img;
}

int get_pixel(image i, point p){
	int x = p.x;
	int y = p.y;
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	return i.pixels[i.w * y + x];
}

int get_pixel_coord(image i, int x, int y){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	return i.pixels[i.w * y + x];
}

bool is_passante(int i){
	int r = i >> 16;
	int g = (i - (r << 16)) >> 8;
	int b = (i - (r << 16) - (g << 8));
	return (r * 0.202 + 0.707 * g + 0.071 * b) >= 128.;
	return true;
}

int get_block_size(image i, int init, int x, int y, bool* traite){
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	if(traite[i.w * y + x]) return 0;
	traite[i.w * y + x] = true;
	if(get_pixel_coord(i,x,y) != init) return 0;
	int v1 = get_block_size(i, init, x + 1, y, traite);
    int v2 = get_block_size(i, init, x - 1, y, traite);
    int v3 = get_block_size(i, init, x, y - 1, traite);
    int v4 = get_block_size(i, init, x, y + 1, traite);
    return 1 + v1 + v2 + v3 + v4;

}


//comparison compare p1 * p2
// ex if p1 upper than p2 then returns 1
// if p1 same level as p2 then 0
// else -1

int lower(point p1, point p2){
	if(p1.y > p2.y) return 1;
	if(p1.y == p2.y) return 0;
	else return -1;
}

int righter(point p1, point p2){
	if(p1.x > p2.x) return 1;
	if(p1.x == p2.x) return 0;
	else return -1;
}

int upper(point p1, point p2){
	if(p1.y < p2.y) return 1;
	if(p1.y == p2.y) return 0;
	else return -1;
}
int lefter(point p1, point p2){
	if(p1.x < p2.x) return 1;
	if(p1.x == p2.x) return 0;
	else return -1;
}

bool est_front(image i, point pos){
	int p1 = get_pixel(i,pos);
	int p2 = get_pixel_coord(i,pos.x + 1, pos.y);
	return p1 != p2;
}


bool ouest_front(image i, point pos){
	int p1 = get_pixel(i,pos);
	int p2 = get_pixel_coord(i,pos.x - 1, pos.y);
	return p1 != p2;
}

bool nord_front(image i, point pos){
	int p1 = get_pixel(i,pos);
	int p2 = get_pixel_coord(i,pos.x, pos.y - 1);
	return p1 != p2;
}
bool sud_front(image i, point pos){
	int p1 = get_pixel(i,pos);
	int p2 = get_pixel_coord(i,pos.x, pos.y + 1);
	return p1 != p2;
}


//is p1 better than p2
bool is_better(image i, point p1, point p2, bool (*front)(image,point),int (*compare1)(point,point), int (*compare2)(point,point)){
	if(!front(i,p2)) return true;
	if(!front(i,p1)) return false;
	if(compare1(p1,p2) > 0) return true;
	if(compare1(p1,p2) == 0) {
		if(compare2(p1,p2) > 0) return true;
		else return false;
	}
	return false;
}

point find_next_border(image i, point start, int x, int y, bool* traite, int (*compare1)(point,point), int (*compare2)(point,point), bool (*front)(image,point)){
	point curr;
	curr.x = x;
	curr.y = y;
	if(traite[i.w * y + x]) return start;
	traite[i.w * y + x] = true;
	int p = get_pixel(i,curr);
	int init = get_pixel(i, start);
	if(p == init){
		point p1 = find_next_border(i, start, x+1, y, traite, compare1, compare2, front); 
		point p2 = find_next_border(i, start, x-1, y, traite, compare1, compare2, front); 
		point p3 = find_next_border(i, start, x, y+1, traite, compare1, compare2, front); 
		point p4 = find_next_border(i, start, x, y-1, traite, compare1, compare2, front);
		if(is_better(i,p1, curr, front, compare1,compare2)) curr= p1;
		if(is_better(i,p2, curr, front, compare1,compare2)) curr= p2;
		if(is_better(i,p3, curr, front, compare1,compare2)) curr= p3;
		if(is_better(i, p4, curr, front, compare1,compare2)) curr= p4;
		return curr;
	}
	return start;
}


//0 est
//1 sud
//2 ouest
//3 nord

//0 babord
//1 tribord
point find_next_edge(image i, point pos,int bord, int dir){
	bool * traite = malloc(sizeof(bool) * i.w * i.h);
	for(int k = 0; k < i.w * i.h; k++) traite[k] = false;
	point p;
	if(dir == 0){
		if(bord == 0){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, righter, upper, est_front);	
		}
		if(bord == 1){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, righter, lower, est_front);	
		}
	}
	if(dir == 1){
		if(bord == 0){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, lower, righter, sud_front);	
		}
		if(bord == 1){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, lower, lefter, sud_front);	
		}
	}
	if(dir == 2){
		if(bord == 0){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, lefter, lower, ouest_front);	
		}
		if(bord == 1){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, lefter, upper, ouest_front);	
		}
	}
	if(dir == 3){
		if(bord == 0){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, upper, lefter, nord_front);	
		}
		if(bord == 1){
			p =	find_next_border(i,pos, pos.x, pos.y, traite, upper, righter, nord_front);	
		}
	}
	free(traite);
	return p;
}





int main(){
	is_passante(rgbtohtml(55,44,22));
	return 0;
}
