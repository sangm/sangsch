CC = gcc
CFLAGS = -g -Wall

sim: aux
	./aux
aux: main.c auxf.o
	gcc $(CFLAGS) -o aux $^

auxf.o: auxf.c auxf.h
	gcc -c auxf.c auxf.h
