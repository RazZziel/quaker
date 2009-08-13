CC=g++
SRC=main.cpp CVector.cpp
OUT=quaker
CFLAGS=-Wall -ansi -std=c++0x
LDFLAGS=-lm -lGL -lGLU `sdl-config --cflags --libs`

all:
	$(CC) ${SRC} ${CFLAGS} -o ${OUT} ${LDFLAGS}

run: all
	./${OUT}

clean:
	@echo Cleaning up...
	@rm ${OUT}
	@echo Done.
