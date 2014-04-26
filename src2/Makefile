CC = gcc
CFLAGS = -Wall -pedantic

sangsch: sangsch.c
	$(CC) $(CFLAGS) -o $@ $^
debug: sangsch.c
	$(CC) $(CFLAGS) -DDEBUG -o sangsch $^
