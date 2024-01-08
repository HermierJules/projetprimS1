#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "operations.h"
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

//@requires v to be an html color code
//@assigns
//@ensures returns true if v is a coding color, false otherwise
bool is_codante(int v){
	for(int k = 0; k < 18; k++){
		if(colortab[k] == v) return true;
	}
	return false;
}



//@requires c to be an html color code 
//@assigns
//@ensures returns the associated luminescence value, 0 for light, 1 for normal and 2 for dark 
int associate_lum(int c) {
	if (c == lightred || c == lightyellow || c == lightgreen || c == lightcyan || c == lightblue || c == lightmagenta) return 0;
	if (c == red || c == yellow || c == green || c == cyan || c == blue || c == magenta) return 1;
	if (c == darkred || c == darkyellow || c == darkgreen || c == darkcyan || c == darkblue || c == darkmagenta) return 2;
	return -1; 
}


//@requires c to be an html color code 
//@assigns
//@ensures returns the associated color value 
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


//@requires image to be a proper image and p to be a position inside the bounds of the image 
//@assigns
//@ensures returns the html color code in the position (p.x, p.y) 
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

//@requires  i to be an html color code
//@assigns
//@ensures returns true if i is a passing color, false otherwise 
bool is_passante(int i){
	int r = i >> 16;
	int g = (i - (r << 16)) >> 8;
	int b = (i - (r << 16) - (g << 8));
	return (r * 0.202 + 0.707 * g + 0.071 * b) >= 128.;
	return true;
}



//@requires i to be a properly initialized image, init html color code, traite to not be NULL 
//@assigns
//@ensures returns the size of the block situated in (x,y) 
int get_block_size(image i, int init, int x, int y, bool* traite){
	//END case: has been visited already. Finite number of pixels so will end
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


//@requires front, compare1  and compare2 to be properly initialized functions without memory leaks.
//@assign nothing
//@ensures returns true if p1 is "better" than p2 using compare1 first, compare2 second and that the final result is true according to font
//(there will always be a pixel to which front will associate true due to the nature of the 2D terrain)
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


//@requires front, compare1  and compare2 to be properly initialized functions without memory leaks. Traite to be an Array of size i.w * i.h at least initialized to false
//@assign nothing
//@ensures returns the wanted pixel according to compare1 and compare2 
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


//@requires bord and dir to be in their respective bounds
//@assign a boolean array of size i.w * i.h temporarily 
//@ensures returns the next edge pixel according to the direction and bord 
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


//@requires 
//@assign 
//@ensures p.x = |p.x| && p.y = |p.y| 
point normalize_point(image i, point p){
	int x = p.x;
	int y = p.y;
	x = (x < 0)  ? (x  + i.w) % i.w : x % i.w; 
	y = (y < 0)  ? (y  + i.h) % i.h : y % i.h;
	p.x = x;
	p.y = y;
	return p;
}

//@requires dir, bo, passant and finished to not be NULL, i to be properly initialized and start to be in bounds
//@assigns a boolean array of size i.w * i.h temporarily in its calls
//@ensures returns a pixel from the next block according to the current direction and bord. Changes the bord and direction according to the language description
//will return the starting pixel and change finished to true if it get stuck 8 times in a row (inifinitely stuck as it would loop from that point on)
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
			//blocking color
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
		//passing color
		while(is_passante(get_pixel(i, pos)) && !is_codante(get_pixel(i,pos))){
			// End when we reach a non-passing color / coding color
			// Infinite loop in case of continuous strip of passing color
			// could be fixed by checking if we ever reach a pixel that we've seen by remembering the starting pixel
			// but the (little) performance cost isn't worth it for an undetermined case in the language description
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




//@requires dir, bo, passant and finished to not be NULL, i to be properly initialized and start to be in bounds
//@assigns a boolean array of size i.w * i.h temporarily in its calls
//@ensures returns a pixel from the next block according to the current direction and bord. Changes the bord and direction according to the language description
//will return the starting pixel and change finished to true if it get stuck 8 times in a row (inifinitely stuck as it would loop from that point on)
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



void interprete(image i){
	stack* s = create_stack();
	point pos;
	pos.x = 0;
	pos.y = 0;
	int bo = 0;
	int dir = 0;
	bool finished = false;
	while(true){
		//finishes if get_next_block gets stuck 8 times in a raw
		bool passant = false;
		point next_block = get_next_block(i, pos, &dir, &bo, 0, false, &passant, &finished);
		if(finished) break;
		if(!passant){
			operate(pos, i, get_pixel(i, pos), get_pixel(i, next_block), &dir, &bo, s);
		}
		else {
		}
		pos.x = next_block.x;
		pos.y = next_block.y;
	}
	free(s);
}



void start(char* file_path){
	image i = read_image(file_path);
	interprete( i);
	free(i.pixels);
}



int main(int argc, char** argv){
	if(argc != 2){
		fprintf(stderr, "Use : interpreter </path/to/program.ppm/bmp/etc...>");
		return 1;
	}
	start(argv[1]);
	return 0;
}
