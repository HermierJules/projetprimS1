#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include <stdio.h>

stack* create_stack(){
	stack* s = malloc(sizeof(stack));
	s->top = NULL;
	s->n = 0;
	return s;
}

void push(stack* s,int i){
	maillon* m = malloc(sizeof(maillon));
	m->val = i;
	s->n++;
	m->next = s->top;
	s->top = m;
}


int pop(stack* s){
	if(s->n < 1) {
		perror("not enough elements to pop");
		exit(1);
	}
	maillon* m = s->top;
	s->top = m->next;
	s->n--;
	int val = m->val;
	free(m);
	return val;
}

bool is_stack_empty(stack* s){
	return s->top == NULL;
}

void print_stack(stack* s){
	printf("\n");
	stack* s2 = create_stack();
	while(s->n > 0){
		int k = pop(s);
		printf("|%d ", k);
		push(s2, k);
	}
	while(s2->n > 0){
		int k = pop(s2);
		push(s,k);
	}
	printf("\n");
}

void free_stack(stack* s){
	while(s->top != NULL){
		pop(s);
	}
	free(s);
}

