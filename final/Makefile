CC=gcc -Wall -Wextra

all: interpreter

stack.o: stack.h
operation.o: stack.h operation.h
read.o: read.h stb_image.h

interpreter: stack.o operation.o read.o
	$(CC) -o $@ $^ -lm

