# Makefile for showimage

CC = gcc
CFLAGS = $(shell sdl2-config --cflags) -Wall -O
LIBS = $(shell sdl2-config --libs) -lSDL2_image
EXE = showimage

all: $(EXE)

showimage: showimage.c Makefile
	$(CC) -o $@ $@.c $(CFLAGS) $(LIBS)

clean:
	-rm *.o $(EXE)
