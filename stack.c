#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"


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
	if(s->top == NULL){
		while(true);
		exit(7);
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

void free_stack(stack* s){
	while(s->top != NULL){
		pop(s);
	}
	free(s);
}

