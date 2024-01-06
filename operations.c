#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "stack.h"
#include <stdio.h>
//0 babord, 1 tribord
//0 nord 1 est  2 sud 3 ouest


void moins(stack* s){
	if(s->n < 2) return;
	int n1 = pop(s);
	int n2 = pop(s);
	push(s, n2 - n1);
}

void plus(stack* s){
	if(s->n < 2) return;
	int n1 = pop(s);
	int n2 = pop(s);
	push(s, n1 + n2);
}



void fois(stack* s){
	if(s->n < 2) return;
	int n1 = pop(s);
	int n2 = pop(s);
	push(s, n1 * n2);
}

void divise(stack* s){
	if(s->n < 2) return;
	int n1 = pop(s);
	int n2 = pop(s);
	push(s, n2/n1);
}

void reste(stack* s){
	if(s->n < 2) return;
	int n1 = pop(s);
	int n2 = pop(s);
	printf("%d %d\n", n1,n2);
	fflush(stdout);
	push(s, n2%n1);
}


void non(stack* s){
	if(s->n < 1) return;
	int n1 = pop(s);
	push(s, n1 == 0 ? 1 : 0);
}
void plus_grand(stack* s){
	if(s->n < 2) return;
	int n1 = pop(s);
	int n2 = pop(s);
	push(s, (n2 > n1) ? 1 : 0);
}

void bord(stack* s, int* b){
	if(s->n < 1) return;
	int bord = *b;
	int n = pop(s);

	for(int i = 0; i < n; i++){
		bord = 1 - bord;
	}
	*b = bord;
}

void direction(stack* s, int* d){
	if(s->n < 1) return;
	int k = pop(s);
	*d = (*d + k) % 4;
	while(*d < 0) *d = *d + 4; 
}

void duplique(stack* s){
	if(s->n < 1) return;
	int n = pop(s);
	push(s,n);
	push(s,n);
}


void tourne(stack* s){
	if(s->n < 2) return;
	int n = pop(s);
	int d = pop(s);
	if(s->n < d) {
		push(s,d);
		push(s,n);
		return;
	}
	for(int i = 0; i < n; i ++){
		stack* s2 = create_stack();
		int val = pop(s);
		for(int j = 1; j < d; j++){
			push(s2, pop(s));
		}
		push(s, val);
		for(int j = 1; j < d; j++){
			push(s, pop(s2));
		}
		free(s2);

	}
}


void in_num(stack* s){
	int n;
//	printf("input a number\n");
	fflush(stdout);
	//read(STDIN_FILENO, &n, sizeof(int));
	char buf[256];
	fgets(buf, 256, stdin);
	sscanf(buf,"%d\n",&n);
	push(s,n);
}
void in_char(stack* s){
	char n;
//	printf("input a char\n");
	//fflush(stdout);
	//read(STDIN_FILENO, &n, sizeof(char));
	n = getc(stdin);
	push(s,n);
}

void out_num(stack* s){
	if(s->n < 1) return;
	int n =  0;
	n = pop(s);
	printf("%d",n );
	fflush(stdout);
	//write(STDOUT_FILENO, &n, sizeof(int));
}

void out_char(stack* s){
	if(s->n < 1) return;
	char n = pop(s);
	printf("%c",n );
	fflush(stdout);
	
	//write(STDOUT_FILENO, &n, sizeof(char));
}


