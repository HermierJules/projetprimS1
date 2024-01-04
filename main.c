#include <stdio.h>
#include <stdlib.h>
#include "operations.h"

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

/*
int get_block_size(image i, pixel init, int x, int y, bool* traite) {
    if (x < 0 || x >= i.w || y < 0 || y >= i.h || traite[i.w * y + x]) {
        return 0;
    }

    traite[i.w * y + x] = true;
    if (!pixel_eq(init, get_pixel(i, x, y))) {
        return 0;
    }

    int v1 = get_block_size(i, init, x + 1, y, traite);
    int v2 = get_block_size(i, init, x - 1, y, traite);
    int v3 = get_block_size(i, init, x, y - 1, traite);
    int v4 = get_block_size(i, init, x, y + 1, traite);

    return 1 + v1 + v2 + v3 + v4;
}
*/
int get_block_size(image i, pixel init ,int x, int y, bool* traite){
	if(traite[i.w * y + x]) return 0;
	traite[i.w * y + x] = true;
	if(!pixel_eq(init, get_pixel(i,x,y))) return 0;
	int v1 = get_block_size(i,init,x+1,y,traite);
	int v2 = get_block_size(i,init,x-1,y,traite);
	int v3 = get_block_size(i,init,x,y-1,traite);
	int v4 = get_block_size(i,init,x,y+1,traite);
	return 1 + v1 + v2 + v3 + v4;
}

//NEED TO FREE TRAITE
point get_next_pixel_edge(image i, int x, int y, int direction, int bord, bool* traite){
	point p;
	p.x = x;
	p.y = y;
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



point get_next_block(image i, int x, int y,int* bord, int* d, int count, bool has_turned){
	bool* traite = calloc(i.h * i.w, sizeof(bool));
	point ed = get_next_pixel_edge(i, x, y, *d, *bord,traite);
	free(traite);
	x = ed.x;
	y = ed.y;
	int b = *bord;
	int direction = *d;
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
	while(true){
		x = x + dx;
		y = y + dy;
		pixel p = get_pixel(i, x, y);
		point pp;
		pp.x = x;
		pp.y = y;
		if(is_block_color(p)) return pp;
		if(is_passante(p)) continue;
		else{
			if(has_turned){
				if(count == 8){
					exit(2);	
				}
				else{
					*d = (*d + 1) % 4;
					return get_next_block(i, ox, oy, bord, d, count+1, false);
				}
			}
			else{
				*bord = (b + 2) % 4;
				return get_next_block(i, ox, oy, bord, d, count, true);
			}
		}
	}
}

//0 red
//1 yellow
//2 green
//3 cyan
//4 blue
//5 magenta
int associate_col(int c) {
	//peut aussi être coder en parcourant colortab 3 par 3
	if (c == lightred || c == red || c == darkred) return 0;
	else if (c == lightyellow || c == yellow || c == darkyellow) return 1;
	else if (c == lightgreen || c == green || c == darkgreen) return 2;
	else if (c == lightcyan || c == cyan || c == darkcyan) return 3;
	else if (c == lightblue || c == blue || c == darkblue) return 4;
	else if (c == lightmagenta || c == magenta || c == darkmagenta) return 5;
	return -1; 
}
//0 light
//1 normal
//2 dark
int associate_lum(int c) {
	if (c == lightred || c == lightyellow || c == lightgreen || c == lightcyan || c == lightblue || c == lightmagenta) return 0;
	if (c == red || c == yellow || c == green || c == cyan || c == blue || c == magenta) return 1;
	if (c == darkred || c == darkyellow || c == darkgreen || c == darkcyan || c == darkblue || c == darkmagenta) return 2;
	return -1; 
}

void operate(image i,stack* s, int* dir, int* bo, point prev_block, point  curr_block){
	int prev = rgbtohtml(get_pixel(i, prev_block.x, prev_block.y));
	int curr = rgbtohtml(get_pixel(i, curr_block.x, curr_block.y));
	int c1 = associate_col(prev);
	int c2 = associate_col(curr);
	int cran = (c1 - c2 < 0) ? c2 - c1 : c1 - c2;
	int l1 = associate_lum(prev);
	int l2 = associate_lum(curr);
	int diff_lum = (l1 - l2 < 0) ? l2 - l1 : l1 - l2;
	if(cran == 0){
		if(diff_lum == 1){
			bool* traite = malloc(i.w * i.h *sizeof(bool));
			for(int j = 0; j < i.w * i.h; j++) traite[j] = false;
			int size = get_block_size(i, get_pixel(i,prev_block.x,prev_block.y), prev_block.x,prev_block.y, traite);
			free(traite);
			push(s,size);
		}
		if(diff_lum == 2 && s->n > 0) pop(s);
	}
	if(cran == 1){
		if(diff_lum == 0) plus(s);
		if(diff_lum == 1) moins(s);
		if(diff_lum == 2) fois(s);
	}
	if(cran == 2){
		if(diff_lum == 0) divise(s);
		if(diff_lum == 1) reste(s);
		if(diff_lum == 2) non(s);
	}
	if(cran == 3){
		if(diff_lum == 0) plus_grand(s);
		if(diff_lum == 1) {
			int new_d = direction(s, *dir);
			*dir = new_d;
		}
		if(diff_lum == 2){
			int new_b = bord(s, *bo);
			*bo = new_b;
		}
	}
	if(cran == 4){
		if(diff_lum == 0) duplique(s);
		if(diff_lum == 1) tourne(s);
		if(diff_lum == 2) in_num(s);
	}
	if(cran == 5){
		if(diff_lum == 0) in_char(s); 
		if(diff_lum == 1) out_num(s);
		if(diff_lum == 2) out_char(s);
	}
}




//0 nord
//1 est
//2 sud
//3 ouest
//0 babord
//1 tribord
void interprete(image i, int x, int y, int dir, int bo, stack* s){
	while(true){
	printf("%d, %d\n", x, y);
	fflush(stdout);
	//x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	//y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	point next_block = get_next_block(i, x, y, &bo, &dir, 0, false);
	point curr;
	curr.x = x;
	curr.y = y;
	operate(i, s, &dir, &bo, curr, next_block);
	x = next_block.x;
	y = next_block.y;
   }
//	interprete(i, next_block.x, next_block.y, dir, bo, s);
}




void start(){
	image i = read_ppm("input.ppm");
	stack* s = create_stack();
	interprete(i,0,0, 1, 0, s);
	free(s);
}


 
int main() {
    char filename[] = "input.ppm";
    image img = read_ppm(filename);
	pixel p;
	printf("w: %d, h : %d", img.w, img.h);
	//printf("r: %d, g: %d, b: %d", p.r, p.g, p.b);
	free(img.pixels);
	//start();
    return 0;
}
