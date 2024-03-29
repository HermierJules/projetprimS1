#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "operations.h"
#include "raylib.h"
#include <unistd.h>
#include <string.h>

#include "read.h"

#include "stb_image.h"


struct point{
	int x;
	int y;
};
typedef struct point point;




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


bool is_codante(int v){
	for(int k = 0; k < 18; k++){
		if(colortab[k] == v) return true;
	}
	return false;
}

int associate_lum(int c) {
	if (c == lightred || c == lightyellow || c == lightgreen || c == lightcyan || c == lightblue || c == lightmagenta) return 0;
	if (c == red || c == yellow || c == green || c == cyan || c == blue || c == magenta) return 1;
	if (c == darkred || c == darkyellow || c == darkgreen || c == darkcyan || c == darkblue || c == darkmagenta) return 2;
	return -1; 
}


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
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	point curr;
	curr.x = x;
	curr.y = y;
	if(traite[i.w * y + x]) return start;
	traite[i.w * y + x] = true;
	fflush(stdout);
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

int get_dx(int dir){
	if(dir == 0) return 1;
	if(dir == 2) return -1;
	return 0;
}
int get_dy(int dir){
	if(dir == 1) return 1;
	if(dir == 3) return -1;
	return 0;
}


point normalize_point(image i, point p){
	int x = p.x;
	int y = p.y;
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	p.x = x;
	p.y = y;
	return p;
}

point get_next_block(image i, point start, int* dir, int* bo,int count, bool has_turned, bool* passant, bool* finished){
	start = normalize_point(i, start);
	point edge = find_next_edge(i, start, *bo, *dir);
	edge = normalize_point(i, edge);
	int dx = get_dx(*dir);
	int dy = get_dy(*dir);
	point pos;
	pos.x = edge.x + dx;
	pos.y = edge.y + dy;
	if(is_codante(get_pixel(i,pos))) return pos;
	if(!is_passante(get_pixel(i,pos))){
			//cas bloquant
			if(!has_turned){
				*bo = 1 - *bo;
				return get_next_block(i, start, dir, bo, count, true, passant, finished);
			}
			else {
				if(count == 8){
					*finished = true;
					return start;
				}
				*dir = (*dir + 1) % 4;
				return get_next_block(i, start, dir, bo, count + 1, false, passant, finished);
			}
	}
	else {
		*passant = true;
		//cas couleur passante
		while(is_passante(get_pixel(i, pos)) && !is_codante(get_pixel(i,pos))){
			//tant que passante et non codante on avance
			pos.x = pos.x + dx;
			pos.y = pos.y + dy; 
		}
		if(is_codante(get_pixel(i,pos))) return pos;
		else{
			pos.x = pos.x - dx;
			pos.y = pos.y - dy;
			return get_next_block(i, pos, dir, bo, 0, false, passant, finished);
		}
	
	}
}


//TODO CHECK

//going from i1 to i2
int calculate_cran(int i1, int i2){
	if(i2 >= i1) {
		return i2 - i1;
	}
	return 6 - (i1 - i2);
}

int calculate_lum_diff(int i1, int i2){
	if(i2 >= i1) {
		return i2 - i1;
	}
	return 3 - (i1 - i2);

}




void operate_debug(point prev_pos,image i, int prev_col, int next_col, int* dir, int* bo, stack* s){
	int c1 = associate_col(prev_col);
	int c2 = associate_col(next_col);
	int l1 = associate_lum(prev_col);
	int l2 = associate_lum(next_col);

	int cran = calculate_cran(c1, c2);
	int diff_lum = calculate_lum_diff(l1, l2);
	printf("cran: %d, diff_lum: %d, operation: ", cran, diff_lum);
	if(cran == 0){
		if(diff_lum == 1){
			printf("empile");
			bool* traite = malloc(sizeof(bool) * i.w * i.h);
			for(int j = 0; j < i.w * i.h; j++) traite[j] = false;
			int k = get_block_size(i, prev_col, prev_pos.x, prev_pos.y, traite);
			push(s, k);
			free(traite);	
		}
		if(diff_lum == 2 && s->n > 0){
			printf("depile");
			pop(s);
		}
	}
	if(cran == 1){
		if(diff_lum == 0) {
			printf("plus");
			plus(s);
		}
		if(diff_lum == 1){
			printf("moins");
			moins(s);
		}
		if(diff_lum == 2){
			printf("fois");
			fois(s);
		}
	}
	if(cran == 2){
		if(diff_lum == 0){
			printf("divise");
			divise(s);
		}
		if(diff_lum == 1){
			printf("reste");
			reste(s);
		}
		if(diff_lum == 2){
			printf("non");
				non(s);
		}
	}
	if(cran == 3){
		if(diff_lum == 0){
			printf("plus grand");
			plus_grand(s);
		}
		if(diff_lum == 1){
			printf("direction");
			direction(s, dir);
			printf("nouvelle dir %d", *dir);
		}
		if(diff_lum == 2){
			printf("bord");
			bord(s, bo);
		}
	}
	if(cran == 4){
		if(diff_lum == 0) {
			printf("duplique");
			duplique(s);
		}
		if(diff_lum == 1){
			printf("tourne");
			tourne(s);
		}
		if(diff_lum == 2){
			printf("in num");
			in_num(s);
		}
	}
	if(cran == 5){
		if(diff_lum == 0){
			printf("in char");
			in_char(s);
		}
		if(diff_lum == 1){
			printf("out_num");
			out_num(s);
		}
		if(diff_lum == 2){
			printf("out char");
			out_char(s);
		}
	}
}

void operate(point prev_pos,image i, int prev_col, int next_col, int* dir, int* bo, stack* s){
	int c1 = associate_col(prev_col);
	int c2 = associate_col(next_col);
	int l1 = associate_lum(prev_col);
	int l2 = associate_lum(next_col);

	int cran = calculate_cran(c1, c2);
	int diff_lum = calculate_lum_diff(l1, l2);
	
	if(cran == 0){
		if(diff_lum == 1){
			bool* traite = malloc(sizeof(bool) * i.w * i.h);
			for(int j = 0; j < i.w * i.h; j++) traite[j] = false;
			int k = get_block_size(i, prev_col, prev_pos.x, prev_pos.y, traite);
			push(s, k);
			free(traite);	
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
		if(diff_lum == 1) direction(s, dir);
		if(diff_lum == 2) bord(s, bo);
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



void draw_image_and_highlight(image i, point p, int scale, bool step){
	bool check = true;
	while(check){
		if(step) check = !IsKeyPressed(KEY_RIGHT);
		else check = false;
		BeginDrawing();
		ClearBackground(RAYWHITE);
		for(int x = 0; x < i.w; x++){
			for(int y = 0; y < i.h; y++){
				int c = get_pixel_coord(i,x,y);
				if(is_codante(c)){
					int r = c >> 16;
					int g = (c - (r << 16)) >> 8;
					int b = (c - (r << 16) - (g << 8));
					Color col = (Color){r,g,b, 255};
					DrawRectangle(x * scale, y * scale, scale, scale, col);
				}
				else if(is_passante(c)){
					DrawRectangle(x * scale, y * scale, scale, scale, RAYWHITE);
				}
				else {
					DrawRectangle(x * scale, y * scale, scale, scale, BLACK);

				}
			}
		}
		Color col = (Color){GetRandomValue(0, 250), GetRandomValue(50, 250), GetRandomValue(10, 200), 255 };
		DrawRectangle(p.x * scale, p.y * scale, scale, scale, col);
		EndDrawing();

	}
}

bool breakpoint_mem(stack* s, int x, int y){
	fflush(stdout);
	bool check = false;
	stack* s2 = create_stack();
	while(s->n > 0){
		int kx = pop(s);
		int ky = pop(s);
		if(kx == x && ky == y) check = true;
		push(s2, kx);
		push(s2, ky);
	}
	while(s2-> n > 0){
		push(s, pop(s2));
	}
	return check;
}


void interprete(int scale, image i, stack* bp, bool visu, bool step){
	stack* s = create_stack();
	point pos;
	pos.x = 0;
	pos.y = 0;
	int bo = 0;
	int dir = 0;
	bool finished = false;
	while(true){
		if(breakpoint_mem(bp, pos.x, pos.y)){
			char buf[256];
			printf("Breakpoint reached, switch to step by step mode ?[y/n] ");
			fgets(buf, 256, stdin);
			char ch;
			sscanf(buf, "%c\n", &ch);
			if(ch == 'y') step = true;
		}

		printf("\nx: %d, y: %d, bord: %d, dir : %d", pos.x, pos.y, bo, dir);
		if(visu || step){
		draw_image_and_highlight(i, pos, scale, step);
		}
		bool passant = false;
		point next_block = get_next_block(i, pos, &dir, &bo, 0, false, &passant, &finished);
		if(finished) break;
		if(!passant){
			operate_debug(pos, i, get_pixel(i, pos), get_pixel(i, next_block), &dir, &bo, s);
		}
		else {
			printf("passed by a passing color, nothing");
		}
		pos.x = next_block.x;
		pos.y = next_block.y;
		print_stack(s);
	}
	free(s);
}


void start(char* file_path, stack* breakpoints, bool visu, bool step, int scale){
	image i = read_image(file_path);
	if(visu){
		const int screenWidth = i.w * scale;
		const int screenHeight = i.h * scale;
		printf("%d %d", i.w, screenHeight); 
		InitWindow(screenWidth, screenHeight, "raylib [shapes] example - colors palette");
		SetTargetFPS(60);
	}
	interprete(scale, i, breakpoints, visu, step);
	free(i.pixels);
}

void add_breakpoint(stack* s){
	printf("Input the x and y values as such: x y\n");
	int x, y;
	char buf[256];
	fgets(buf, 256, stdin);
	sscanf(buf,"%d %d\n", &x, &y);
	push(s,y);
	push(s,x);
}

void print_breakpoints(stack* s){
	stack* s2 = create_stack();
	while(s->n > 0){
		int kx = pop(s);
		int ky = pop(s);
		printf("(%d, %d) | ", kx, ky);
		push(s2, kx);
		push(s2, ky);
	}
	printf("\n");
	while(s2-> n > 0){
	push(s, pop(s2));
	push(s, pop(s2));
	}
}

void print_ui(char* fp){
	char* on1 = "off";
	char* on2 = "off";
	bool visu = false;
	stack* breakpoints = create_stack();
	bool step = false;
	printf("Welcome to the Cornelis debugger\n");
	bool st = true;
	int scale = 10;
	while(st){
		int n;
		printf("--------------------------------------------------------\n");
		printf("1| Start Execution\n2| Visualise Program: %s\n3| Add Breakpoint\n4| Step By Step (requires visualisation) : %s\n5| Change Scale: current scale is %d\n", on1, on2, scale);
		printf("Current Breakpoints:\n");
		if(breakpoints->n == 0) printf("None\n");
		else print_breakpoints(breakpoints);
		printf("--------------------------------------------------------\n");
		char buf[256];
		fgets(buf, 256, stdin);
		sscanf(buf,"%d\n", &n);
		switch(n){
			case 1: st = false; break;
			case 2: visu = 1 - visu; if(strcmp(on1, "off") == 0) on1 = "on"; else on1 = "off"; break;
			case 3: add_breakpoint(breakpoints); break;
			case 4: step = 1 - step; if(strcmp(on2, "off") == 0) on2 = "on"; else on2 = "off";visu = true; on1 = "on"; break;
			case 5: printf("Input new scale:\n"); fgets(buf, 256, stdin); sscanf(buf, "%d\n", &scale); break;
			default: printf("Incorrect input"); break;
		}
	}

start(fp, breakpoints, visu, step, scale);
}


int main(int argc, char** argv){
	if(argc != 2){
		fprintf(stderr, "Missing arguments");
		return 1;
	}
	print_ui(argv[1]);
	return 0;
}
