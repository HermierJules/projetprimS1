CC=gcc

all: interpreter

clean:
	rm stack.o operations.o  read.o

stack.o: stack.h

operations.o: operations.h stack.h


read.o: read.c stb_image.h
	$(CC) -c -o $@ read.c

debugger: debugger.c stack.o operations.o read.o 
	$(CC) -o $@ $^ -lm -lraylib
interpreter: interpreter.c stack.o operations.o read.o 
	$(CC) -o $@ $^ -lm 
