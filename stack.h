#include <stdlib.h>
#include <stdbool.h>

struct maillon{
	int val;
	struct maillon* next;
};

typedef struct maillon maillon;

struct stack{
	int n;
	maillon* top;
};

typedef struct stack stack;


stack* create_stack();

void push(stack* s, int i);

int pop(stack* s);

void print_stack(stack* s);

void free_stack(stack* s);
